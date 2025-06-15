#pragma once

#include <variant>
#include <string_view>

#include "include/riri/Commands.hpp"
#include "ankerl/unordered_dense.h"
#include "src/include/RiRiMacros.h"



using RapidDataType = std::variant<std::string, int_fast64_t, double, bool>;


/**
 * @brief ### Main Memory Map for Rapid Data Access
 * 
 * This map uses `std::string_view` as keys and `RapidDataType` as values.
 * It is designed for fast lookups and efficient memory usage.
 * 
 * The map is initialized with a reserved size to optimize performance.
 * 
 * @note This map is used for storing rapid data types that can be strings, integers,
 * floating-point numbers, and booleans.
 * @see RapidDataType for the types of values stored in this map.
 */
GO_AWAY extern ankerl::unordered_dense::map<
    std::string_view,
    RapidDataType,
    ankerl::unordered_dense::hash<std::string_view>,
    std::equal_to<>
> MemoryMap;
// Yes, I leveled up and am using ankerl::unordered_dense::map with hash and equality
// functions for std::string_view, as intended.
// Previously, I just used ankerl::unordered_dense::map<std::string, std::string>, which was definitely not optimal.



/**
 * @brief ### Auxiliary Command Function Pointer Map
 * 
 * This map associates command names (as `std::string_view`) with their corresponding
 * function pointers of type `RiRiCommandFn`. 
 * 
 * - Used for auxiliary command handling.
 * 
 * - It allows for fast command dispatching based on user input.
 * 
 * @note - The map is initialized to support a limited number of commands efficiently.
 * @note - The map is initialized with a small reserved size to optimize performance.
 * @note - The command functions should match the signature defined by `RiRiCommandFn`.
 * @see RiRiCommandFn for the function pointer type used for commands.
 * @see Commands.h for the list of available commands and their implementations.
 */
GO_AWAY extern ankerl::unordered_dense::map<
    std::string_view,
    RiRiCommandFn,
    ankerl::unordered_dense::hash<std::string_view>,
    std::equal_to<>
> AuxCommandMap;