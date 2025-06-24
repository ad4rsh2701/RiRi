#pragma once
#include <variant>
#include <string>
#include <string_view>
#include <span>

#include "RiRiMacros.h"
#include "src/utils/RapidError.h"


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
 * @brief Type alias for a command function that takes a vector of string views as arguments and returns a result of type RiRiResult<std::string_view>.
 * 
 * Used to define the signature for RiRi command handlers.
 * @param args A vector of string views representing the command arguments.
 * @note - The command name is expected to be stripped before calling these functions.
 * @note - Aliases: `RiRiCommandFn` for consistency with the existing codebase.
 */
GO_AWAY using RapidCommandFn = RiRi::Error::RiRiResult<std::string_view>(*)(const std::span<std::string_view>);


/**
 * @brief Sigh, this is a workaround for the fact that ankerl's unordered_dense does not support transparent hash functions.
 * 
 * It allows us to use `std::string` and `std::string_view` as keys in the unordered map without needing to define separate hash functions for each type.
 * 
 * This struct provides a hash function that can handle both `std::string` and `std::string_view` seamlessly
 * by using the ankerl::unordered_dense::hash specialization for each type.
 * 
 * The `is_transparent` type is used to indicate that this hash function can be used with both `std::string` and `std::string_view` keys.
 * This is important, as `is_transparent` is not explicitly defined in the `ankerl::unordered_dense library` :(
 * 
 * I maybe wrong about that, but it doesn't seem to work without this workaround (I was wrong)
 * 
 *EDIT: `ankerl` does support it, just not by default (unlike `unordered_map`), to enable heterogeneous lookup, you'd need to define a `struct` like this.
 * 
 * @param s A `std::string` to hash.
 * @param sv A `std::string_view` to hash.
 * @return The hash value as a `size_t`.
 * @note This is used in the `MemoryMap` and `AuxCommandMap` to allow fast lookups using both `std::string` and `std::string_view` keys.
 * @note For more details refer: https://github.com/martinus/unordered_dense/tree/main?tab=readme-ov-file#324-heterogeneous-overloads-using-is_transparent
 */
GO_AWAY struct RapidHash {
    using is_transparent = void;
    using is_avalanching = void; // mark class as high quality avalanching hash

    [[nodiscard]] size_t operator()(const std::string& s) const noexcept {
        return ankerl::unordered_dense::hash<std::string>{}(s);
    }

    [[nodiscard]] size_t operator()(std::string_view sv) const noexcept {
        return ankerl::unordered_dense::hash<std::string_view>{}(sv);
    }
};
