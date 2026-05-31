#pragma once    // RAPIDTYPES.HPP

#include <cstdint>
#include <string>
#include <variant>


namespace RiRi {


    /**
     * @brief Represents a rapid data type that can hold various types of values.
     *
     * - This type is used for fast data access and manipulation within the RiRi framework.
     *
     * The `RapidDataType` can hold:
     *
     * - `std::string` for text data
     *
     * - `int64_t` for large integers
     *
     * - `double` for floating-point numbers
     *
     * - `bool` for boolean values
     * @note This type is defined in the `src/include/MemoryMaps.h` for storing rapid data.
     */
    using RapidDataType = std::variant<std::string, std::int64_t, double, bool>;


    /**
     * @brief Represents a key-value pair in the map.
     *
     * This struct is used to store a key and its associated value.
     * Used majorly during command parsing and command functions to represent a key-value pair.
     *
     * @var RapidNode::key The key as a `std::string`.
     * @var RapidNode::value The value as a `RapidDataType`, which can be a string, integer, double, or boolean.
     *
     * You can either pass the values normally enforcing a copy, i.e., one allocation during node creation OR
     * move the values entirely via std::move, zero allocation during node creation
     * NOTE: moving steals the buffer, the original variable's value is lost, do not rely on its value post-move
     * (and apparently this post-move-accessing isn't UB).
     */
    struct RapidNode {
        std::string key;
        RapidDataType value;
    };
    // Rule of 0 states that...
    // "If you can, write none of the special members. Let the compiler generate all of them."
    // So, one can either move values directly to RapidNode if they know what they are doing
    // Or pass the values (enforcing a copy once) normally.


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
     * - SYSTEM ERROR CODES (600+): Literally anything else
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

        // We might need warning codes and a lot of them.

        // BUFFER RANGE (300-399)
        // In case warning codes or anything else from above needs more codes, though I doubt it, but still.

        // CORE ERROR CODES (400-499)
        ERR_KEY_STORE_FULL = 400,               // COMMAND LEVEL
        ERR_UNKNOWN = 401,                      // COMMAND LEVEL // Impossible, but LIKELY without parser
        ERR_INVALID_ARGUMENT = 402,             // COMMAND LEVEL // Parser level gives better detail
        ERR_KEY_ALREADY_EXISTS = 403,           // COMMAND LEVEL
        ERR_KEY_NOT_FOUND = 404,                // COMMAND LEVEL // Also, mission complete: +5 xp
        ERR_VALUE_NOT_FOUND = 405,              // COMMAND LEVEL
            // leaving a little gap for basic ones
        ERR_SINGLE_NODE_EXPECTED = 420,         // COMMAND LEVEL

        // Do I really need these? Overkill much?
        // ERR_SOME_KEYS_NOT_SET = 406,
        // ERR_MANY_KEYS_NOT_SET = 407,
        // ERR_SOME_KEYS_NOT_FOUND = 408,
        // ERR_MANY_KEYS_NOT_FOUND = 409

        // Instead, what if I do this:
        ERR_SOME_OPERATIONS_FAILED = 406,        // COMMAND LEVEL
        ERR_MULTIPLE_OPERATIONS_FAILED = 407,    // COMMAND LEVEL

        // This would avoid branching, plus make the error codes more general,
        // over multiple types of commands, because the user already knows what command
        // they triggered. So, yes, it was very likely overkill before.

            // BILLION_DOLLAR_MISTAKE = 401: CLion's autocomplete is wild.

        // These are enough for now, will add more as needed.

        // GENERAL ERROR CODES (500-599)
        ERR_INVALID_KEY = 501,              // PARSER LEVEL
        ERR_INVALID_VALUE = 502,            // PARSER LEVEL
        ERR_INVALID_COMMAND = 503,          // PARSER LEVEL
        ERR_INVALID_ARGUMENT_COUNT = 504,   // PARSER LEVEL
        ERR_DOES_NOT_TAKE_ARGUMENTS = 505,  // PARSER LEVEL
        ERR_NO_ARGUMENTS_GIVEN = 506,       // PARSER LEVEL
        ERR_INVALID_DELIMITER = 507,        // PARSER LEVEL // Last fallback error code, if none of the above.
            // This looks overkill, but the parser is yet to be implemented, so let's see.
            // More to be added as RiRi grows.
            // Pretty sure I'd need more than 100.
            // It looks like it's only for PARSER, but there will be THREAD, PERSISTENCE and SERVER levels too.

        // SYSTEM ERROR CODES
        ERR_OUT_OF_MEMORY = 600             // SYSTEM LEVEL
            // fun
    };

} // namespace riri


// Maybe I would need this for command dispatch? who knows? archiving in file
// /**
//  * @brief Type alias for a command function that takes a vector of string views as arguments.
//  *
//  * Used to define the signature for RiRi command handlers.
//  * @param args A span of RapidNode or Key-Value pairs, representing the command arguments.
//  * @note - The command name is expected to be stripped before calling these functions.
//  */
// template <typename Response>
// GO_AWAY using RapidCommandFn = Response(*)(std::span<RapidNode> args);