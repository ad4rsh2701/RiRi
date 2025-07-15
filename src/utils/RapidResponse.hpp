#pragma once

#include <cstdint>
#include <string_view>
#include <utility>
#include <new>


/// This constant defines the number of errors to be tracked
static constexpr size_t TRACKING_CAPACITY = 8;

/**
 * @brief RiRi's custom response system
 *
 * Also called "(Ra)pid (Re)sponse (Sy)stem" or simply "raresy"
 *
 * - This single header handles/defines how RiRi responds to the end user when called.
 * - Response may either signify success or failure or their derivatives.
 * - This file can be used independently, in other systems if you know how to (and are desperate).
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
    enum class StatusCode : std::uint8_t {
        // SUCCESS CODES (0-99)
        OK = 0, // Covers 90% of the use case, I guess?
            // Will add more as I see fit.

        // INFO CODES (100-199)
            // Nothing here, for now.

        // WARNING CODES (200-299)
        WARN_KEY_STORE_NEARING_CAPACITY = 200,  // COMMAND LEVEL
            // We might need warning codes, and a lot of them.

        // BUFFER RANGE (300-399)
            // In case warning codes or anything else from above need more codes, though I doubt it, but still.

        // CORE ERROR CODES (400-499)
        ERR_KEY_STORE_FULL = 400,               // COMMAND LEVEL
        ERR_UNKNOWN = 401,                      // COMMAND LEVEL // Impossible, but LIKELY without parser
        ERR_INVALID_ARGUMENT = 402,             // COMMAND LEVEL // Parser level gives better detail
        ERR_KEY_ALREADY_EXISTS = 403,           // COMMAND LEVEL
        ERR_KEY_NOT_FOUND = 404,                // COMMAND LEVEL // Also, mission complete: +5 xp
        ERR_VALUE_NOT_FOUND = 405,              // COMMAND LEVEL

        // Do I really need these? Overkill much?
        // ERR_SOME_KEYS_NOT_SET = 406,          // COMMAND LEVEL
        // ERR_MANY_KEYS_NOT_SET = 407,          // COMMAND LEVEL
        // ERR_SOME_KEYS_NOT_FOUND = 408,        // COMMAND LEVEL
        // ERR_MANY_KEYS_NOT_FOUND = 409,        // COMMAND LEVEL

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
    struct RapidResponseFull {
        /// The overall status code defaulted to OK; if it stays OK, you are fine (probably).
        StatusCode OverallCode = StatusCode::OK;

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
         * For instance, the following could be how one may get the response when trying to fetch multiple keys
         * which don't exist in the map:
         * - `key3: KEY_NOT_FOUND`
         * - `key7: KEY_NOT_FOUND`
         */
        using ErrorEntryType = std::pair<std::string_view, StatusCode>;


        /// A fixed blob or block of memory, which stores `ErrorEntryType` objects.
        alignas(ErrorEntryType)
        std::byte ERROR_STORE[TRACKING_CAPACITY * sizeof(ErrorEntryType)];

        /// Pointer that will track error entries, initialized in constructor to point to ERROR_STORE
        ErrorEntryType *failures = nullptr;     // Aptly name

        std::uint8_t failure_count = 0;
        std::uint8_t capacity = 0;

        RapidResponseFull() noexcept {
            // Pointer now points to the ERROR_STORE and will move by `ErrorEntryType`
            failures = reinterpret_cast<ErrorEntryType*>(ERROR_STORE);
            // "Good Programming Principles," they said, "it will be less repetitive", they said.
            capacity = TRACKING_CAPACITY;
        }

        /**
         * @brief Checks whether the overall status code is `StatusCode::OK`.
         * @return True if the overall status code is `StatusCode::OK`, otherwise false.
         */
        constexpr bool ok() const noexcept {
            return OverallCode == StatusCode::OK;
        }


        /**
         * @brief Checks if the number of failures has exceeded the specified capacity and modifies the `OverallCode`
         * to `ERR_MULTIPLE_OPERATIONS_FAILED` if needed.
         * @return True if the number of failures exceeds or equals the capacity, otherwise false.
         */
        constexpr bool isOverflow() noexcept {
            if (failure_count >= capacity) {
                OverallCode = StatusCode::ERR_MULTIPLE_OPERATIONS_FAILED;
                return true;
            }
            return false;
        }


        // At this point, everything should be very self-explainable.

        /**
         * @brief Adds a OPERATION_TARGET-STATUS_CODE pair to the Response's memory blob,
         * only if there is no overflow.
         *
         * @param operation_target : The target of an operation or an operation itself.
         * @param error_code : The error code concerning the operation target.
         */
        void addFailure(std::string_view operation_target, StatusCode error_code) noexcept {
            if (isOverflow()) return;

            // A very neat use of `new` (I was not expecting this).
            // Basically, we are "constructing" the pair at the `failures` position (`failure_count`).
            // Oh and yes, the memory is properly aligned and pre-allocated.
            new(&failures[failure_count]) std::pair{operation_target, error_code};

            ++failure_count;
        }

        /**
         * @brief Resets the RapidResponseFull state by clearing the failure count and
         * setting the overall status code to `StatusCode::OK`.
         */
        constexpr void reset() noexcept {
            failure_count = 0;
            OverallCode = StatusCode::OK;

            // No destructors needed since we only hold trivially destructible types
        }

        /**
         * @brief Provides a constant iterator pointing to the beginning of the `failures` collection.
         * @return A pointer to the first error entry in the failure collection.
         */
        constexpr auto begin() const noexcept { return failures; }

        /**
         * @brief Provides a constant iterator pointing to the end of the `failures` collection.
         * @return A pointer to one past the last error entry in the failure collection.
         */
        constexpr auto end() const noexcept { return failures + failure_count; }

        // you're welcome for the iterators.
    };
}

// DEV NOTE: inside the `RapidResponseFull` struct, under the `addFailure()` functions, I think it should modify the
// overall code to `ERR_SOME_OPERATIONS_FAILED` if no overflow too, but I am not too sure how this will work out yet,
// but implementing command layers next should give a better idea (oh wait the reader helpers remain too, F).