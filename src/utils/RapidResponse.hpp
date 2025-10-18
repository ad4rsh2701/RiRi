#pragma once

#include <cstdint>
#include <string_view>
#include <utility>
#include <memory>
#include <new>

#include "../include/RapidTypes.h"

/// This constant defines the number of errors to be tracked
static constexpr size_t TRACKING_CAPACITY = 8;

/// This constant defines the number of ValueOrStatus entries to be tracked
static constexpr size_t VALUE_TRACKING_CAPACITY = 16;

/**
 * @brief RiRi's custom response system
 *
 * Also called "(Ra)pid (Re)sponse (Sy)stem" or simply "raresy"
 *
 * - This single header handles/defines how RiRi responds to the end user when called.
 * - Response may either signify success or failure or their derivatives.
 * - This file can be used independently in other systems if you know how to (and are desperate).
 *
 * @warning DO NOT use this header file unless you know what you are doing.
 * @note If you know what you are doing, then you may use this file to build on top RiRi's internals, go wild.
 */
namespace RiRi::Response {
    /**
     * @brief Represents various status codes for responses within the RapidResponse framework.
     *
     * Status codes are categorized into multiple ranges based on their meanings:
     *
     * - SUCCESS CODES (0-99): Indicate successful operations.
     * - INFO CODES (100-199): Reserved for informational messages (currently unused).
     * - WARNING CODES (200-299): Represent warnings, such as reaching certain thresholds.
     * - BUFFER RANGE (300-399): Reserved for potential expansion of warnings or other ranges.
     * - CORE ERROR CODES (400-499): Represent errors related to core functionalities, such as missing keys or invalid arguments.
     * - GENERAL ERROR CODES (500-599): Represent parser, persistence, thread or other level issues, such as invalid input or argument counts.
     */
    enum class StatusCode : std::uint16_t {
        // SUCCESS CODES (0-99)
        OK = 0, // Covers 90% of the use case, I guess?
            // Will add more as I see fit.

        // INFO CODES (100-199)
            // Nothing here, for now.

        // WARNING CODES (200-299)
        WARN_KEY_STORE_NEARING_CAPACITY = 200,  // COMMAND LEVEL
            // We might need warning codes, and a lot of them.

        // BUFFER RANGE (300-399)
            // In case warning codes or anything else from above needs more codes, though I doubt it, but still.

        // CORE ERROR CODES (400-499)
        ERR_KEY_STORE_FULL = 400,               // COMMAND LEVEL
        ERR_UNKNOWN = 401,                      // COMMAND LEVEL // Impossible, but LIKELY without parser
        ERR_INVALID_ARGUMENT = 402,             // COMMAND LEVEL // Parser level gives better detail
        ERR_KEY_ALREADY_EXISTS = 403,           // COMMAND LEVEL
        ERR_KEY_NOT_FOUND = 404,                // COMMAND LEVEL // Also, mission complete: +5 xp
        ERR_VALUE_NOT_FOUND = 405,              // COMMAND LEVEL

        // Do I really need these? Overkill much?
        // ERR_SOME_KEYS_NOT_SET = 406,
        // ERR_MANY_KEYS_NOT_SET = 407,
        // ERR_SOME_KEYS_NOT_FOUND = 408,
        // ERR_MANY_KEYS_NOT_FOUND = 409,

        // Instead, what if I do this:
        ERR_SOME_OPERATIONS_FAILED = 406,        // COMMAND LEVEL
        ERR_MULTIPLE_OPERATIONS_FAILED = 407,    // COMMAND LEVEL

        // This would avoid branching, plus make the error codes more general,
        // over multiple types of commands, because the user already knows what command
        // they triggered. So, yes, it was very likely overkill before.

            // BILLION_DOLLAR_MISTAKE = 401: CLion's autocomplete is wild
            // These are enough for now, will add more as needed.

        // GENERAL ERROR CODES (500-599)
        ERR_INVALID_KEY = 501,              // PARSER LEVEL
        ERR_INVALID_VALUE = 502,            // PARSER LEVEL
        ERR_INVALID_COMMAND = 503,          // PARSER LEVEL
        ERR_INVALID_ARGUMENT_COUNT = 504,   // PARSER LEVEL
        ERR_DOES_NOT_TAKE_ARGUMENTS = 505,  // PARSER LEVEL
        ERR_NO_ARGUMENTS_GIVEN = 506,       // PARSER LEVEL
        ERR_INVALID_DELIMITER = 507,        // PARSER LEVEL // Last fall back error code, if none of the above.
            // This looks overkill, but parser is yet to be implemented, so let's see.
            // More to be added as RiRi grows.
            // Pretty sure I'd need more than 100
            // It looks like it's only for PARSER, but there will be THREAD, PERSISTENCE and SERVER levels too.
    };


    /**
     * @brief Represents a response structure within the RapidResponse framework.
     *
     * The `RapidResponse` struct encapsulates the response status and
     * provides helper functions within it to evaluate the `StatusCode` state.
     *
     * This only holds the overall status code; even in cases where multiple
     * status codes might be required, in such cases, an appropriate status
     * code showcasing the potential errors is used.
     *
     * Though let's be honest, you will still most likely use this, 90% of the time.
     *
     */
    struct RapidResponse {
        /// Represents a single status outcome — either from a single operation
        /// or an overall result when no per-operation diagnostics are needed.
        StatusCode response;


        // should be obvious but-
        /**
         * @brief Checks if the response status is successful.
         * @return `true` if the response status is `StatusCode::OK`, otherwise false.
         */
        constexpr bool ok() const noexcept {
            return response == StatusCode::OK;
        }
    };

    /**
     * @brief Represents a full response structure used to track individual error status codes
     * when a command operates on multiple inputs or when multiple errors need to be reported together.
     *
     * The `RapidResponseFull` struct does four things:
     * 1. Captures a blob or block of memory, statically.
     * 2. Makes the compiler treat the blob as if it stores elements of type `ErrorEntryType`
     *    (a pair of `string_view` and `StatusCode`).
     * 3. Constructs and stores an `ErrorEntryType` when `addFailure` is called and sets the status
     *    code to `ERR_SOME_OPERATIONS_FAILED`.
     * 4. If `addFailure` is called beyond the blob’s capacity, the status is changed to
     *    `ERR_MULTIPLE_OPERATIONS_FAILED`, and no further entries are added to the blob.
     *
     * For instance, this is what a stored response might look like:
     *  [{"key2", StatusCode::KEY_NOT_FOUND}, {"Key3", StatusCode::INVALID_ARGS}, {...}, ...]
     *
     *  Made with agony for the 10% use cases :D
     */
    class RapidResponseFull {
    public:

        // "No silly names yet?", Hey! I am trying to implement good coding habits okay
        // It's called self-explainable code, don't laugh... or ykw, here, a doc-string too, just in case
        /** @brief Represents each OPERATION_TARGET-STATUS_CODE pair; this is how RiRi will carry individual
         * status codes for each operation's target.
         *
         * Operation Target? New term?
         * Yes, not used widely at all inside RiRi, but understand that each operation has parameters or
         * targets; more specifically, the target is the parameter that distinguishes or makes the said
         * operation "identifiable".
         *
         * For instance, the following could be how one may get the response when trying to delete multiple keys
         * which don't exist in the map:
         * - `key3: ERR_KEY_NOT_FOUND`
         * - `key7: ERR_KEY_NOT_FOUND`
         */
        using ErrorEntryType = std::pair<std::string_view, StatusCode>;

        RapidResponseFull() noexcept {
            // Pointer now points to the ERROR_STORE and will move by `ErrorEntryType`
            _failures = reinterpret_cast<ErrorEntryType*>(_error_store);
            // "Good Programming Principles," they said, "it will be less repetitive," they said.
            _capacity = TRACKING_CAPACITY;
        }


    private:

        /// The overall status code defaulted to OK; if it stays OK, you are fine (probably).
        StatusCode _overall_code = StatusCode::OK;

        /// A fixed blob or block of memory, which stores `ErrorEntryType` objects.
        alignas(ErrorEntryType)
        std::byte _error_store[TRACKING_CAPACITY * sizeof(ErrorEntryType)]{};

        /// Pointer that will track error entries, initialized in constructor to point to ERROR_STORE
        ErrorEntryType *_failures = nullptr;     // Aptly named

        std::uint8_t _failure_count = 0;
        std::uint8_t _capacity = 0;
        /**
         * @brief Checks if the number of failures has exceeded the specified capacity and modifies the `OverallCode`
         * to `ERR_MULTIPLE_OPERATIONS_FAILED` if needed.
         * @return True if the number of failures exceeds or equals the capacity, otherwise false.
         */
        constexpr bool handle_overflow() noexcept {
            if (_failure_count >= _capacity) {
                _overall_code = StatusCode::ERR_MULTIPLE_OPERATIONS_FAILED;
                return true;
            }
            return false;
        }

    public:
        // At this point, everything should be very self-explainable.
        /**
         * @brief Adds a OPERATION_TARGET-STATUS_CODE pair to the Response's memory blob,
         * only if there is no overflow.
         *
         * @param operation_target : The target of an operation or an operation itself.
         * @param error_code : The error code concerning the operation target.
         */
        void addFailure(std::string_view operation_target, StatusCode error_code) noexcept {
            if (handle_overflow()) return;

            // A very neat use of `new` (I was not expecting this).
            // Basically, we are "constructing" the pair at the `failures` position (`failure_count`).
            // Oh and yes, the memory is properly aligned and pre-allocated.
            new(&_failures[_failure_count]) ErrorEntryType{operation_target, error_code};

            // And update the OverallCode to ERR_SOME_OPERATIONS_FAILED (406) as well.
            _overall_code = StatusCode::ERR_SOME_OPERATIONS_FAILED;

            ++_failure_count;
        }

        /**
         * @brief Checks whether the overall status code is `StatusCode::OK`.
         * @return True if the overall status code is `StatusCode::OK`, otherwise false.
         */
        constexpr bool ok() const noexcept {
            return _overall_code == StatusCode::OK;
        }

        /**
         * @brief Getter to return the overall status code
         * @return _overall_code
         */
        constexpr StatusCode code() const noexcept {
            return _overall_code;
        }

        /**
         * @brief Resets the RapidResponseFull state by clearing the failure count and
         * setting the overall status code to `StatusCode::OK`.
         */
        constexpr void reset() noexcept {
            _failure_count = 0;
            _overall_code = StatusCode::OK;

            // No destructors needed since we only hold trivially destructible types
        }

        /**
         * @brief Provides a constant iterator pointing to the beginning of the `failures` collection.
         * @return A pointer to the first error entry in the failure collection.
         */
        constexpr auto begin() const noexcept { return _failures; }

        /**
         * @brief Provides a constant iterator pointing to the end of the `failures` collection.
         * @return A pointer to one past the last error entry in the failure collection.
         */
        constexpr auto end() const noexcept { return _failures + _failure_count; }

        // you're welcome for the iterators.
    };


    /**
     * @brief Represents a value-returning response class to track individual values and error
     * status codes when a command operates on multiple inputs or when multiple errors/values need
     * to be reported together.
     *
     * The `RapidResponseValue` struct functionality is largely similar to `RapidResponseFull` but
     * with the following changes:
     * - `EntryType` is a pair of `string_view` and either `StatusCode` or `RapidDataType*`
     * - The default tracking (or value-returning capacity statically) is 16.
     * - If `addFailure` or `addValue` is called beyond the static capacity, a dynamic array is initialized
     *   and it grows dynamically.
     *
     * An example of a stored response containing a value and StatusCode:
     * [{"key2", "val2"}, {"key3", StatusCode::KEY_NOT_FOUND}, {...}, ...]
     *
    * @note Regardless of the size of the static blob, the entirety of requested values (or StatusCode)
    * will be returned, nothing will be dropped.
     */
    class RapidResponseValue{
    public:

        /// Either `Value` or `StatusCode` will be returned per fetch request
        using ValueOrStatus = std::variant<RapidDataType*, StatusCode>;

        /**
         * @brief Represents each OPERATIONS_TARGET-VALUE_OR_STATUS pair; this is how RiRi will carry
         * individual return values (or StatusCode in case of errors) for each operation target.
         *
         *  For instance, the following could be how one may get the response when trying to fetch multiple
         *  keys which may or may not exist in the map:
         * - `Key1: Val1`
         * - `Key2: ERR_KEY_NOT_FOUND`
         * - `Key3: Val3`
         */
        using EntryType = std::pair<std::string_view, ValueOrStatus>;

        // Just in case my future self decides to make the EntryType trivially non-copyable.
        static_assert(std::is_trivially_copyable_v<EntryType>,
          "EntryType must be trivially copyable!!! Don't Forget!");
        // A little reminder for him when his code doesn't compile.

        constexpr RapidResponseValue() noexcept
        :_entries{reinterpret_cast<EntryType *>(_static_store)},
        _entry_count{0},
        _capacity{VALUE_TRACKING_CAPACITY}
        {
            // *insert shrug here*
        }

    private:
        /// The overall status code defaulted to `OK`. If errors, changes to `ERR_SOME_OPERATIONS_FAILED`
        StatusCode _overall_code = StatusCode::OK;

        /// A fixed blob or block of memory, which stores `EntryType` objects.
        alignas(EntryType)
        std::byte _static_store[sizeof(EntryType)*VALUE_TRACKING_CAPACITY]{};

        /// A unique pointer to manage an array of type EntryType
        std::unique_ptr<EntryType[]> _dynamic_store = nullptr;

        /// Pointer that will track entries, initialized in constructor to point to STORE
        EntryType *_entries = nullptr;
        std::uint32_t _entry_count;
        std::uint32_t _capacity;

        /**
         * @brief Increases the dynamic storage capacity for entries when the current capacity is not enough.
         *
         * This method allocates a new buffer of larger size (by a factor of 1.5x) while copying the previous
         * existing elements of the older buffer.
         *
         * @note This operation is noexcept and assumes that `EntryType` is trivially copyable.
         */
        void dynamically_grow() noexcept {
            // Increases capacity dynamically (RIP ns performance, hello μs)

            // Increasing the capacity by 1.5x (new buffer RAHH)
            const std::uint32_t new_capacity = _capacity + _capacity / 2 + 8; // The +8 helps for small initial sizes

            // Allocate a new heap buffer of the new capacity.
            auto new_dynamic_store = std::make_unique<EntryType[]>(new_capacity);

            // Copy elements from old buffer to new buffer (safe to do so; EntryType is trivially copyable)
            std::memcpy(new_dynamic_store.get(), _entries, _entry_count * sizeof(EntryType));

            // We move the new buffer into our class's ownership.
            _dynamic_store = std::move(new_dynamic_store);
            // If we were previously on the heap, the old _heap_store's
            // destructor is called automatically when we re-assign it,
            // freeing the old memory; perks of unique_ptr

            _entries = _dynamic_store.get();   // Point to the new buffer
            _capacity = new_capacity;          // Update capacity
        }

    public:

        /**
         * @brief Getter to return the overall status code
         * @return _overall_code
         */
        constexpr StatusCode code() const noexcept {
            return _overall_code;
        }

        /**
         * @brief Checks whether the overall status code is `StatusCode::OK`.
         * @return True if the overall status code is `StatusCode::OK`, otherwise false.
         */
        constexpr bool ok() const noexcept {
            return _overall_code == StatusCode::OK;
        }

        /**
         * @brief Adds a OPERATION_TARGET-VALUE pair to the Response's memory blob.
         *
         * @param operation_target : The target of an operation or an operation itself.
         * @param entry : The value concerning the target.
         */
        void addValue(std::string_view operation_target, RapidDataType* entry) noexcept {
            // We are going to update the OverallCode early, since this is the only error
            // code this function can return.
            // Update: We don't need to update the status code at all. This is the "OK" case,
            // which is the default status code.
            // OverallCode = StatusCode::ERR_SOME_OPERATIONS_FAILED;

            if (_entry_count >= _capacity) {
                dynamically_grow();
            }

            // Construct OPERATION_TARGET-VALUE in STORE at entry_count.
            new(&_entries[_entry_count]) EntryType{operation_target, entry};
            ++_entry_count;
        }

        /**
         * @brief Adds a OPERATION_TARGET-STATUS_CODE pair to the Response's memory blob.
         * @param operation_target : The target of an operation or an operation itself
         * @param status_code : The value concerning the target
         */
        void addStatus(std::string_view operation_target, StatusCode status_code) noexcept {
            // shrugieeee
            _overall_code = StatusCode::ERR_SOME_OPERATIONS_FAILED;

            if (_entry_count >= _capacity) {
                dynamically_grow();
            }

            // Construct OPERATION_TARGET-STATUS_CODE in STORE at entry_count.
            new(&_entries[_entry_count]) EntryType{operation_target, status_code};
            ++_entry_count;
        }

        /**
        * @brief Provides a constant iterator pointing to the beginning of the `entries` collection.
        * @return A pointer to the first error entry in the `entries` collection.
        */
        constexpr auto begin() const noexcept { return _entries; }

        /**
         * @brief Provides a constant iterator pointing to the end of the `entries` collection.
         * @return A pointer to one past the last error entry in the `entries` collection.
         */
        constexpr auto end() const noexcept { return _entries + _entry_count; }

    };
}

// DEV NOTE: inside the `RapidResponseFull` struct, under the `addFailure()` functions, I think it should modify the
// overall code to `ERR_SOME_OPERATIONS_FAILED` if no overflow too, but I am not too sure how this will work out yet,
// but implementing command layers next should give a better idea (oh wait the reader helpers remain too, F).
// Update: Yes, it should, let's not introduce complexity over at other layers, let this layer handle the 90% responses.
// Yes, this reduces customizability, but RiRi is open source... modify and compile it yourself???