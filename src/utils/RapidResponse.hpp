#pragma once

#include <concepts>
#include <cstdint>
#include <memory>
#include <new>
#include <string_view>
#include <utility>
#include <RapidTypes.h>


template <typename T>
concept ResponseField = std::same_as<T, const RapidDataType*> || std::same_as<T, std::string_view>;

/// This constant defines the number of errors to be tracked
static constexpr size_t TRACKING_CAPACITY = 8;

/// This constant defines the number of ValueOrStatus entries to be tracked
static constexpr size_t VALUE_TRACKING_CAPACITY = 8;

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
        ORPHANED = 100,                         // Default uninitialized state (equivalent to UNSET in other similar systems).
            // Basically, the response is currently empty and pending assignment.

        // WARNING CODES (200-299)
        WARN_KEY_STORE_NEARING_CAPACITY = 200,      // COMMAND LEVEL
        WARN_RESPONSE_CONTAINS_WARNINGS = 250,      // RESPONSE LEVEL
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
        ERR_INVALID_DELIMITER = 507,        // PARSER LEVEL // Last fallback error code, if none of the above.
            // This looks overkill, but parser is yet to be implemented, so let's see.
            // More to be added as RiRi grows.
            // Pretty sure I'd need more than 100
            // It looks like it's only for PARSER, but there will be THREAD, PERSISTENCE and SERVER levels too.
    };

    // helper functions to check what a status code's type is

    [[nodiscard]] constexpr bool isSuccess(StatusCode code) noexcept {
        const auto val = static_cast<std::uint16_t>(code);
        return val < 100;
    }

    [[nodiscard]] constexpr bool isInfo(StatusCode code) noexcept {
        const auto val = static_cast<std::uint16_t>(code);
        return val >= 100 && val < 200;
    }

    [[nodiscard]] constexpr bool isWarning(StatusCode code) noexcept {
        const auto val = static_cast<std::uint16_t>(code);
        return val >= 200 && val < 400; // Includes the 300-400 buffer range for now
    }

    [[nodiscard]] constexpr bool isError(StatusCode code) noexcept {
        const auto val = static_cast<std::uint16_t>(code);
        return val >= 400; // Covers errors and beyond
    }


    /**
     * @brief Represents a status containing response class within the Rapid Response System.
     *
     * `Status` encapsulates the response status and provides helper
     * functions within it to evaluate the `StatusCode` state.
     *
     * It also provides a getter function to retrieve the status code.
     *
     * This only holds the overall status code; even in cases where multiple
     * status codes might be required, in such cases, an appropriate status
     * code showcasing the potential errors may be used.
     *
     * Though let's be honest, you will still most likely use this, 90% of the time.
     */
    class Status {

    public:
        // The default constructor initializes `_status_code` to the most common case.
        Status() noexcept : _status_code(StatusCode::OK) {}

        // Explicit constructor to initialize `_status_code` with StatusCode types.
        explicit Status(StatusCode status_code) noexcept : _status_code(status_code) {}

    private:
        /// Represents a single status outcome — either from a single operation
        /// or an overall result when no per-operation diagnostics are needed.
        StatusCode _status_code;

    public:
        /**
         * @brief Setter to set the status code of the response
         * @param status_code The status code to be set
         */
        constexpr void setCode(const StatusCode status_code) noexcept {
            _status_code = status_code;
        }

        /**
         * @brief Returns the status_code of the response
         * @return StatusCode
         */
        [[nodiscard]] constexpr StatusCode code() const noexcept {
            return _status_code;
        }

        /**
         * @brief Checks if the status code is OK.
         * @return `true` if the response status is `StatusCode::OK`, otherwise false.
         */
        [[nodiscard]] constexpr bool ok() const noexcept {
            return _status_code == StatusCode::OK;
        }
    };

    /**
     * @brief Represents a Batched Status-Error response, used to track individual error status codes
     * when a command operates on multiple inputs or when multiple errors need to be reported together.
     *
     * This essentially holds a list of "errors"
     *
     * For instance, this is what a stored response might look like:
     *  [{"key2", StatusCode::KEY_NOT_FOUND}, {"Key3", StatusCode::INVALID_ARGS}, {...}, ...]
     *
     * Also holds and tracks the total number of errors.
     *
     * Made with agony for the 10% use cases :D
     *
     * @note Success cases are not tracked, and the absence of keys in the response means the set was successful
     * unless the overall code is `ERR_MULTIPLE_OPERATIONS_FAILED`, in which case... good luck I guess (you bought
     * this on yourself honestly).
     *
     * @note The tracking capacity is manually set; by default, set to 8. This means only 8 errors will be
     * reported. If there are more errors than the tracking capacity, the overall status code becomes
     * ERR_MULTIPLE_OPERATIONS_FAILED and the rest of the errors are not tracked.
     *
     * @note This class does not own the target field's memory
     */
    template <ResponseField F>
    class StatusErrorBatchWith {

        // `StatusErrorBatchWith` does four things:
        // 1. Captures a blob or block of memory, statically.
        // 2. Makes the compiler treat the blob as if it stores elements of type `ErrorEntryType`
        //    (a pair of `ResponseField` and `StatusCode`).
        // 3. Constructs and stores an `ErrorEntryType` when `addErrorEntry` is called and sets the status
        //    code to `ERR_SOME_OPERATIONS_FAILED`.
        // 4. If `addErrorEntry` is called beyond the blob’s capacity, the status is changed to
        //    `ERR_MULTIPLE_OPERATIONS_FAILED`, and no further entries are added to the blob.

    public:

        /** @brief Represents a FIELD_TARGET-STATUS_CODE pair; this is how RiRi will carry individual
         * status codes for each operation's target.
         *
         * FIELD_TARGET is of type `ResponseField` and is usually of the intended type `string_view`. However,
         * since `ResponseField` can also be `RapidDataType`, it is also supported programmatically.
         *
         * For instance, the following could be how one may get the response when trying to delete multiple keys
         * which don't exist in the map:
         * - `key3: ERR_KEY_NOT_FOUND`
         * - `key7: ERR_KEY_NOT_FOUND`
         */
        using ErrorEntryType = std::pair<F, StatusCode>;

        constexpr StatusErrorBatchWith() noexcept {
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

        std::uint32_t _failure_count = 0;
        std::uint8_t _capacity = 0;

        /**
         * @brief Checks if the number of failures has exceeded the specified capacity and modifies the `OverallCode`
         * to `ERR_MULTIPLE_OPERATIONS_FAILED` if needed.
         * @return True if the number of failures exceeds the capacity, otherwise false.
         */
        constexpr bool handle_overflow() noexcept {
            if (_failure_count > _capacity) {
                _overall_code = StatusCode::ERR_MULTIPLE_OPERATIONS_FAILED;
                return true;
            }
            return false;
        }

    public:
        // At this point, everything should be very self-explainable.
        /**
         * @brief Adds a FIELD_TARGET-STATUS_CODE pair to the Response's memory blob,
         * only if there is no overflow.
         *
         * @param field_target : The target of an operation or an operation itself.
         * @param error_code : The error code concerning the operation target.
         */
        void addErrorEntry(F field_target, StatusCode error_code) noexcept {

            // We will always increase the failure count to keep track of the number of failures
            ++_failure_count;

            if (handle_overflow()) return;

            // A very neat use of `new`
            // Basically, we are "constructing" the pair at the `failures` position (`failure_count`).
            // Oh and yes, the memory is properly aligned and pre-allocated.
            new(&_failures[_failure_count-1]) ErrorEntryType{field_target, error_code};

            // And update the OverallCode to ERR_SOME_OPERATIONS_FAILED (406) as well.
            _overall_code = StatusCode::ERR_SOME_OPERATIONS_FAILED;
        }

        /**
         * @brief Checks whether the overall status code is `StatusCode::OK`.
         * @return True if the overall status code is `StatusCode::OK`, otherwise false.
         */
        [[nodiscard]] constexpr bool ok() const noexcept {
            return _overall_code == StatusCode::OK;
        }

        /**
         * @brief Getter to return the overall status code
         * @return _overall_code
         */
        [[nodiscard]] constexpr StatusCode code() const noexcept {
            return _overall_code;
        }

        /**
         * @brief Getter to return the total number of errors occurred, not
         * necessarily equal to the number of entries in the response buffer.
         * @return _failure_count
         */
        [[nodiscard]] constexpr std::uint32_t totalErrorCount() const noexcept {
            return _failure_count;
        }

        /**
         * @brief Resets the class state by clearing the failure count and
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
        [[nodiscard]] constexpr auto begin() const noexcept { return _failures; }

        /**
         * @brief Provides a constant iterator pointing to the end of the `failures` collection.
         * @return A pointer to one past the last error entry in the failure collection.
         */
        [[nodiscard]] constexpr auto end() const noexcept {
            // only iterate up to the capacity or failure count; whichever is lower
            return _failures + (_failure_count > _capacity ? _capacity : _failure_count);
        }

        // you're welcome for the iterators.
    };


    /**
     * @brief A single status-field response class
     * This class encapsulates the response status code and a field value of the type ResponseField.
     * @note This class does not own field's memory
     */
    template<ResponseField F>
    class StatusWith {

        /// The field value: initialized to default values
        F _field { };

        /// The status code: initialized to the default status code ORPHANED
        StatusCode _status_code = StatusCode::ORPHANED;

    public:

        /**
         * @brief Getter to return the overall status code
         * @return _status_code
         */
        [[nodiscard]] StatusCode code() const noexcept { return _status_code; }

        /**
         * @brief Checks whether the status code is `StatusCode::OK`.
         * @return True if the status code is `StatusCode::OK`, otherwise false.
         */
        [[nodiscard]] bool ok() const noexcept { return _status_code == StatusCode::OK; }

        /**
         * @brief Getter to return the field object
         * @return ResponseField type object
         */
        [[nodiscard]] F field () const noexcept { return _field; }

        /**
         * @brief Common setter to set both the status code and the field values
         * @param code The status code of type StatusCode
         * @param response_field The field of type ResponseField to set
         */
        void fill(const StatusCode code, F response_field) noexcept {
            _field = response_field;
            _status_code = code;
        }

        /**
         * @brief Setter to set the field value of the response
         * @param response_field The field of type ResponseField
         */
        void setField(F response_field) noexcept { _field = response_field; }

        /**
         * @brief Setter to set the status code of the response
         * @param code The status code of type StatusCode
         */
        void setCode(const StatusCode code) noexcept { _status_code = code; }
    };


    /**
     * @brief Represents a Batched Target_Field-Status_Field response class to track individual Target-Result
     * or target-status_codes when a command operates on multiple targets or when multiple results/status codes
     * need to be reported together.
     *
     * `StatusBatchWith`'s functionality is largely similar to `StatusErrorBatchWith` but
     * with the following changes:
     * - `EntryType` is a pair of `ResponseField` (1) and either `StatusCode` or `ResponseField` (2)
     * - The default tracking (or value-returning capacity statically) is 8.
     * - If `addStatusEntry` or `addResultEntry` is called beyond the static capacity, a dynamic array is
     *   initialized and it grows dynamically.
     *
     * (1): The target field <- Input Field
     * (2): The result field <- Output Field
     *
     * An example of a stored response:
     * [{"key2", "val2"}, {"key3", StatusCode::KEY_NOT_FOUND}, {...}, ...]
     *
    * @note Regardless of the size of the static blob, the entirety of the
    * requested result (or StatusCode) will be returned, nothing will be dropped.
    * @note This class does not own the memory of the Fields.
     */
    template <ResponseField F1, ResponseField F2>
    class StatusBatchWith{
    public:

        /// Either `ResponseField` (Result Field) or `StatusCode` will be returned per request
        using ResultOrStatus = std::variant<F2, StatusCode>;

        /**
         * @brief Represents each TARGET_FIELD-FIELD_OR_STATUS pair; this is how raresy
         * will carry individual result field (or StatusCode) for each operation target.
         *
         *  For instance, the following could be how one may get the response when trying to fetch multiple
         *  keys which may or may not exist in the map:
         * - `Key1: Val1`
         * - `Key2: ERR_KEY_NOT_FOUND`
         * - `Key3: Val3`
         */
        using EntryType = std::pair<F1, ResultOrStatus>;

        // We don't need static assert bloat when we got concepts
        // ESPECIALLY INSIDE A TEMPLATE.
        // // Just in case my future self decides to make the EntryType trivially non-destructible/copyable.
        // static_assert(std::is_trivially_destructible_v<EntryType>,
        //   "EntryType must be trivially destructible!!!");
        // static_assert(std::is_trivially_copyable_v<EntryType>,
        //   "EntryType must be trivially copyable!!!");
        // A little reminder for him when his code doesn't compile.

        constexpr StatusBatchWith() noexcept
        :_entries{reinterpret_cast<EntryType *>(_static_store)},
        _entry_count{0},
        _capacity{VALUE_TRACKING_CAPACITY}
        {
            // *insert shrug here*
        }

    private:
        /// The initial status code `ORPHANED`
        StatusCode _overall_code = StatusCode::ORPHANED;

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

            // Copy elements from the old buffer to the new buffer (safe to do so; EntryType is trivially copyable)
            std::memcpy(new_dynamic_store.get(), _entries, _entry_count * sizeof(EntryType));
            // Using `memcpy` for now (trivially copyable types only, enforced by static_assert above).
            // Will switch to `std::uninitialized_move` if EntryType becomes non-trivial in the future.

            // We move the new buffer into our class's ownership.
            _dynamic_store = std::move(new_dynamic_store);
            // If we were previously on the heap, the old _heap_store's
            // destructor is called automatically when we re-assign it,
            // freeing the old memory; perks of unique_ptr

            _entries = _dynamic_store.get();   // Point to the new buffer
            _capacity = new_capacity;          // Update capacity
        }

        /**
         * @brief Internal helper for determining the general status code based on status code
         * @param status_code The StausCode to find the general code of.
         * @return StatusCode
         */
        [[nodiscard]] static constexpr StatusCode determine_general_code(const StatusCode status_code) noexcept {
            const auto val = static_cast<std::uint16_t>(status_code);
            if (val < 200) return StatusCode::OK;         // success + info
            if (val < 400) return StatusCode::WARN_RESPONSE_CONTAINS_WARNINGS;
            return StatusCode::ERR_SOME_OPERATIONS_FAILED;
        }

        /**
         * @brief Internal overall code modifier based on status code
         * @param status_code The added StatusCode to update the overall code accordingly.
         */
        constexpr void escalate_overall_code(const StatusCode status_code) noexcept {

            // 0. nuke case, do nothing since nothing can escalate this
            if (_overall_code == StatusCode::ERR_MULTIPLE_OPERATIONS_FAILED) { return; }

            const StatusCode general_code = determine_general_code(status_code);

            // A tiny branch for cases like OK getting passed
            // eh never mind, replacing a full register is faster than branching
            // No wait, we need this, so that it doesn't go to other heavier branches
            // if (_overall_code == general_code) { return; }
            // actually really nvm two branches is better anyway, and since
            // one of the branches is just checking overall code, the compiler
            // will never branch wrong with just two cases, the second branch isn't
            // that heavy, and for same general codes, it never even executes into
            // that branch anyway. <- What being at the office does to a guy

            // 1. If ORPHANED, just set
            if (_overall_code == StatusCode::ORPHANED) {
                _overall_code = general_code;
                return;
            }

            // 2. Otherwise, update based on severity ranking
            // OK (0) < WARN_ (250) < ERR_ (406)
            if (static_cast<std::uint16_t>(general_code) > static_cast<std::uint16_t>(_overall_code)) {
                _overall_code = general_code;
            }
        }

    public:

        /**
         * @brief Getter to return the overall status code
         * @return _overall_code
         */
        [[nodiscard]] constexpr StatusCode code() const noexcept {
            return _overall_code;
        }

        /**
         * @brief Checks whether the overall status code is `StatusCode::OK`.
         * @return True if the overall status code is `StatusCode::OK`, otherwise false.
         */
        [[nodiscard]] constexpr bool ok() const noexcept {
            return _overall_code == StatusCode::OK;
        }

        /**
         * @brief Adds a TARGET_FIELD-RESPONSE_FIELD pair to the Response's memory blob.
         *
         * @param target_field: The target of an operation or an operation itself.
         * @param result_field: The result concerning the target.
         */
        void addResultEntry(F1 target_field, F2 result_field) noexcept {
            // We are going to update the OverallCode early, since this is the only error
            // code this function can return.
            // Update: We don't need to update the status code at all. This is the "OK" case,
            // which is the default status code.
            // OverallCode = StatusCode::ERR_SOME_OPERATIONS_FAILED;

            if (_entry_count >= _capacity) {
                dynamically_grow();
            }

            // Construct OPERATION_TARGET-VALUE in STORE at entry_count.
            new(&_entries[_entry_count]) EntryType{target_field, result_field};
            ++_entry_count;

            // Since both fields are being added, clearly everything is okay! So, we try to
            // escalate the overall-code with OK to overwrite the initial case
            escalate_overall_code(StatusCode::OK);
        }

        /**
         * @brief Adds a TARGET_field-STATUS_CODE pair to the Response's memory blob.
         * @param target_field : The target of an operation or an operation itself
         * @param status_code : The status code concerning the target
         */
        void addStatusEntry(F1 target_field, StatusCode status_code) noexcept {
            // shrugieeee

            if (_entry_count >= _capacity) {
                dynamically_grow();
            }

            // Construct OPERATION_TARGET-STATUS_CODE in STORE at entry_count.
            new(&_entries[_entry_count]) EntryType{target_field, status_code};
            ++_entry_count;

            escalate_overall_code(status_code);
        }

        /**
        * @brief Provides a constant iterator pointing to the beginning of the `entries` collection.
        * @return A pointer to the first error entry in the `entries` collection.
        */
        [[nodiscard]] constexpr auto begin() const noexcept { return _entries; }

        /**
         * @brief Provides a constant iterator pointing to the end of the `entries` collection.
         * @return A pointer to one past the last error entry in the `entries` collection.
         */
        [[nodiscard]] constexpr auto end() const noexcept { return _entries + _entry_count; }

    };
}

// DEV NOTE: inside the `RapidResponseFull` struct, under the `addFailure()` functions, I think it should modify the
// overall code to `ERR_SOME_OPERATIONS_FAILED` if no overflow too, but I am not too sure how this will work out yet,
// but implementing command layers next should give a better idea (oh wait the reader helpers remain too, F).
// Update: Yes, it should, let's not introduce complexity over at other layers, let this layer handle the 90% responses.
// Yes, this reduces customizability, but RiRi is open source... modify and compile it yourself???

// And we can shrink the minimum size of the RapidResponseValue class to less than 544 bits by using a union instead of
// two stores. Which will save us around... I think 16-24 bits? I am not sure, but essentially, making it all fit under
// 64 bytes (that's 512 bits) will make it fit under one CPU cache lane, which is a "nice to have" (it sounds good).
// Though I will not make this change yet, cuz this class's objects will definitely not be of the "minimum size" most of
// the time. Just writing here so I don't forget.