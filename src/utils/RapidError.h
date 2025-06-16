#pragma once

#include <string_view>
#include <expected>   // C++23: Like Rust's Result<T, E>
#include <utility>    // for std::forward

#include "src/include/RiRiMacros.h"  // For GO_AWAY macro

/**
 * @brief ## INTERNAL ERROR SYSTEM
 * 
 * If you are reading this, then you are in the internal zone of RiRi.
 * 
 * This module provides utilities for error handling within the RiRi framework.
 * 
 * @note - This is an internal zone of RiRi, and you should not use it directly EVEN IF you know what you're doing.
 */
namespace RiRi::Error {
    
    
    /**
     * @brief #### Enumeration of error codes for RiRi operations
     * 
     * This enumeration defines the various error codes that can
     * be returned by RiRi operations.
     */
    GO_AWAY enum class RiRiError : std::uint8_t {
        Success = 0,
        KeyNotFound = 1,
        InvalidArgs = 2,
        EmptyStore = 3,
        ParseError = 4,
        OutOfMemory = 5,
        ValueNotFound = 6
    };


    /**
     * @brief #### Get the error message for a given `RiRiError`
     * 
     * This function returns a string_view representing the error message
     * associated with the specified RiRiError.
     */
    GO_AWAY std::string_view get_error_message(RiRiError error) noexcept {
        switch (error) {
            case RiRiError::Success:
                return "Success";
            case RiRiError::KeyNotFound:
                return "Key not found";
            case RiRiError::InvalidArgs:
                return "Invalid arguments";
            case RiRiError::EmptyStore:
                return "Map is empty";
            case RiRiError::ParseError:
                return "Parse error";
            case RiRiError::OutOfMemory:
                return "Out of memory";
            case RiRiError::ValueNotFound:
                return "Value not found";
            default:
                return "Unknown error";
        }
    }


    /**
     * @brief Type alias for a command function (`GET`, `SEARCH`, `GET_ALL`, etc.) that returns a result of type `RiRiResult<T>`.
     * 
     * This is used for functions that return a value of type `T` or an error.
     */
    template<typename T>
    using RiRiResult = std::expected<T, RiRiError>;


    /**
     * @brief Type alias for a status function (`SET`, `UPDATE`, `DELETE`, etc.) that returns a result of type `RiRiStatus`.
     * 
     * This is used for functions that do not return a value but indicate success or failure.
     */
    using RiRiStatus = std::expected<void, RiRiError>;


    /**
     * @brief Create a successful result with a value.
     * 
     * This function is used to create a successful result containing a value of type `T`.
     */
    template<typename T>
    [[nodiscard]]
    constexpr RiRiResult<T> riri_ok(T&& value) noexcept {
        return RiRiResult<T>(std::forward<T>(value));
    }


    /**
     * @brief Create a failed result with an error.
     * 
     * This function is used to create a failed result containing an error of type `RiRiError`.
     */
    template<typename T>
    [[nodiscard]]
    constexpr RiRiResult<T> riri_err(RiRiError error) noexcept {
        return std::unexpected(error);
    }


    /**
     * @brief Create a successful status (void).
     * 
     * This function is used to create a successful status, indicating that the operation was successful.
     */
    [[nodiscard]]
    constexpr RiRiStatus riri_success() noexcept {
        return RiRiStatus();  // Empty expected = success
    }


    /**
     * @brief Create a failed status (void).
     * 
     * This function is used to create a failed status, indicating that the operation failed.
     */
    [[nodiscard]]
    constexpr RiRiStatus riri_failure(RiRiError error) noexcept {
        return std::unexpected(error);
    }

    
    // Difference between riri_err and riri_failure?
    // - `riri_err` is used for functions that return a value (RiRiResult<T>).
    // - `riri_failure` is used for functions that do not return a value (RiRiStatus).
    // "But we can use riri_err for both cases, right?"
    // Yes, we can use `riri_err` for both cases, but it's more idiomatic to use `riri_failure` for functions that return a status (void).
    // "Idiomatic?" Yes, essentially, better readability and clarity in the codebase.


    // "[[nodiscard]] what?" It is used to indicate that the return value of the function should not be ignored.
    // We don't want users to ignore the result of these functions, as they indicate success or failure.
}