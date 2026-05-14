#pragma once

#include <cstdint>
#include <string>
#include <variant>
#include <ankerl/unordered_dense.h>

// I initially thought that creating this file would just be a bloat. But I guess with great structs comes great responsibility.

/**
 * @brief Represents a rapid data type that can hold various types of values.
 * 
 * - This type is used for fast data access and manipulation within the RiRi framework.
 *
 * The `RapidDataType` can hold:
 * 
 * - `std::string` for text data
 * 
 * - `int_fast64_t` for large integers
 * 
 * - `double` for floating-point numbers
 * 
 * - `bool` for boolean values
 * @note This type is defined in the `src/include/MemoryMaps.h` for storing rapid data.
 *
 */
using RapidDataType = std::variant<std::string, int_fast64_t, double, bool>;


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