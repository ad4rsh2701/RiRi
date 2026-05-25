#pragma once    // MEMORYMAPS.H

#include <variant>
#include <string>
#include <string_view>
#include <RapidTypes.h>
#include <RiRiMacros.h>
#include <ankerl/unordered_dense.h>


/**
 * @brief ### WARNING: INTERNAL ZONE!
 * 
 * If you are reading this, then you are in the internal zone of RiRi.
 * And you are 100% going to get an error or break something.
 * 
 * PLEASE DO NOT use internal maps defined here directly — they're NOT part of the public API.
 * 
 * - If you want to use `ankerl::unordered_dense::map`, please use it in your own code by including the appropriate headers (found [here](https://github.com/martinus/unordered_dense/blob/main/include/ankerl/unordered_dense.h)).
 * 
 * - If you really want to break something, go ahead, you can create a fork of RiRi (here: https://github.com/ad4rsh2701/riri) and modify it as you wish.
 * 
 * BACK OFF!
 */
namespace RiRi::Internal {

    /**
     * @brief Sigh, this is a workaround for the fact that ankerl's unordered_dense does not take transparent hash functions by default.
     *
     * It allows us to use `std::string` and `std::string_view` as keys in the unordered map without needing to define separate hash functions for each type.
     *
     * This struct provides a hash function that can handle both `std::string` and `std::string_view`
     * by using the ankerl::unordered_dense::hash specialization for each type.
     *
     * The `is_transparent` type is used to indicate that this hash function can be used with both `std::string` and `std::string_view` keys.
     * This is important, as `is_transparent` is not explicitly defined in the `ankerl::unordered_dense library` :(
     *
     * I am maybe wrong about that, but it doesn't seem to work without this workaround (I was wrong)
     *
     * EDIT: `ankerl` does support it, just not by default (unlike `unordered_map`), to enable heterogeneous lookup, you'd need to define a `struct` like this.
     *
     * Standard procedure, I guess?
     *
     * @param s A `std::string` to hash.
     * @param sv A `std::string_view` to hash.
     * @return The hash value as a `size_t`.
     * @note This is used in the `MemoryMap` and `AuxCommandMap` to allow fast lookups using both `std::string` and `std::string_view` keys.
     * @note For more details refer: https://github.com/martinus/unordered_dense/tree/main?tab=readme-ov-file#324-heterogeneous-overloads-using-is_transparent
     */
    GO_AWAY struct RapidHash {
        using is_transparent = void;
        using is_avalanching = void; // mark class as high-quality avalanching hash

        [[nodiscard]] size_t operator()(const std::string& s) const noexcept {
            return ankerl::unordered_dense::hash<std::string>{}(s);
        }

        [[nodiscard]] size_t operator()(const std::string_view sv) const noexcept {
            return ankerl::unordered_dense::hash<std::string_view>{}(sv);
        }
    };


    /**
     * @brief ### Main Memory Map for Rapid Data Access.
     *
     * This map uses `std::string` as keys and `RapidDataType` as values.
     * It is designed for fast lookups and efficient memory usage.
     * 
     * The map is initialized with a reserved size to optimize performance.
     * 
     * @note This map is used for storing rapid data types that can be strings, integers,
     * floating-point numbers, and booleans.
     * @see RapidDataType for the types of values stored in this map.
     */
    GO_AWAY extern ankerl::unordered_dense::map<
        std::string,
        RapidDataType,
        RapidHash,
        std::equal_to<>
    > MemoryMap;

    // We are using ankerl::unordered_dense::map<std::string, RapidDataType> with a custom hash.
    // This allows us to store various types of data in the map, including strings, integers, doubles, and booleans.
    // Why a custom hash?
    // Because ankerl::unordered_dense::map does not support transparent hash "by default". You'd need to define a custom
    // struct to enable heterogeneous lookup.
    // More details here: https://github.com/martinus/unordered_dense/tree/main?tab=readme-ov-file#324-heterogeneous-overloads-using-is_transparent



    // We don't need this right now, we are not parsing yet...
    // /**
    //  * @brief ### Auxiliary Command Function Pointer Map
    //  *
    //  * This map associates command names (as `std::string`) with their corresponding
    //  * function pointers of type `RapidCommandFn`.
    //  *
    //  * - Used for auxiliary command handling.
    //  *
    //  * - It allows for fast command dispatching based on user input.
    //  *
    //  * @note - The map is initialized to support a limited number of commands efficiently.
    //  * @note - The map is initialized with a small reserved size to optimize performance.
    //  * @note - The command functions should match the signature defined by `RapidCommandFn`.
    //  * @see RapidCommandFn for the function pointer type used for commands.
    //  * @see Commands.h for the list of available commands and their implementations.
    //  */
    // GO_AWAY extern ankerl::unordered_dense::map<
    //     std::string,
    //     RapidHash,
    //     std::equal_to<>
    // > AuxCommandMap;

} // namespace RiRi::Internal
