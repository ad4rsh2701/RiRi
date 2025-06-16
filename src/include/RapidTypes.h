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
