#pragma once

#include <variant>
#include <string_view>

#include "src/include/RapidTypes.h"
#include "ankerl/unordered_dense.h"
#include "src/include/RiRiMacros.h"


/**
 * @brief ### WARNING: INTERNAL ZONE
 * 
 * If you are reading this, then you are in the internal zone of RiRi.
 * And you are 100% going to get an error or break something.
 * 
 * PLEASE DO NOT use internal maps defined here directly — they're NOT part of the public API.
 * 
 * - If you want to use `ankerl::unordered_dense::map`, please use it in your own code by including the appropriate headers (found [here](https://github.com/martinus/unordered_dense/blob/main/include/ankerl/unordered_dense.h)).
 * 
 * - If you really want to break something, go ahead or you can just use the public API, which is much safer and more stable.
 * 
 * - Or you can create a fork of RiRi (here: https://github.com/ad4rsh2701/riri) and modify it as you wish.
 * 
 * BACK OFF!
 */
namespace RiRi::Internal {
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
     * function pointers of type `RapidCommandFn`. 
     * 
     * - Used for auxiliary command handling.
     * 
     * - It allows for fast command dispatching based on user input.
     * 
     * @note - The map is initialized to support a limited number of commands efficiently.
     * @note - The map is initialized with a small reserved size to optimize performance.
     * @note - The command functions should match the signature defined by `RapidCommandFn`.
     * @see RapidCommandFn for the function pointer type used for commands.
     * @see Commands.h for the list of available commands and their implementations.
     */
    GO_AWAY extern ankerl::unordered_dense::map<
        std::string_view,
        RapidCommandFn,
        ankerl::unordered_dense::hash<std::string_view>,
        std::equal_to<>
    > AuxCommandMap;

}