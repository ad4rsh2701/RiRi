#pragma once

#include <cstdint>
#include <string_view>
#include <utility>
#include <new>

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
     * @enum RiRi::Response::StatusCode
     * Represents various status codes for responses within the RapidResponse framework.
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
        ERR_SOME_KEYS_NOT_SET = 406,            // COMMAND LEVEL
        ERR_MANY_KEYS_NOT_SET = 407,            // COMMAND LEVEL
        ERR_SOME_KEYS_NOT_FOUND = 408,          // COMMAND LEVEL
        ERR_MANY_KEYS_NOT_FOUND = 409,          // COMMAND LEVEL
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


    struct RapidResponse {
        StatusCode response;

        constexpr bool ok() const noexcept {
            return response == StatusCode::OK;
        }
    };

    struct RiRiResponseContainer {
        static constexpr size_t INLINE_CAPACITY = 4;

        StatusCode overall_code = StatusCode::OK;

        alignas(std::pair<std::string_view, StatusCode>)
        // weird looking syntax, but not actually weird
        char inline_storage[INLINE_CAPACITY * sizeof(std::pair<std::string_view, StatusCode>)];

        std::pair<std::string_view, StatusCode>* failures = nullptr;
        std::uint8_t failure_count = 0;
        std::uint8_t capacity = 0;

        RiRiResponseContainer() noexcept {
            // weird looking syntax and actually weird
            failures = reinterpret_cast<std::pair<std::string_view, StatusCode>*>(inline_storage);
            capacity = INLINE_CAPACITY;
        }

        void addFailure(std::string_view key, StatusCode code) {
            if (failure_count >= capacity) {
                overall_code = StatusCode::ERR_MANY_KEYS_NOT_SET;
                return;
            }
            // weird looking syntax and actually not not weird?
            new (&failures[failure_count]) std::pair{key, code};
            ++failure_count;
        }

        void reset() {
            failure_count = 0;
            overall_code = StatusCode::OK;
            // No destructors needed since we only hold trivially destructible types
        }

        // for iteration
        auto begin() const noexcept { return failures; }
        auto end()   const noexcept { return failures + failure_count; }
    };
}