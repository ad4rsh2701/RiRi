#pragma once    // ACCESSORS.HPP

#include <concepts>
#include <string>
#include <format>
#include <variant>

#include "RiRiMacros.h"
#include "riri/RapidTypes.hpp"

namespace RiRi::Utils {


    // Constraining the type to be the same as the types of `RapidDataType` variant
    template <typename T>
    concept Accessible = std::same_as<T, std::string>
                      || std::same_as<T, std::int64_t>
                      || std::same_as<T, bool>
                      || std::same_as<T, double>;
    // I wonder if we can static_assert the types in RapidDataType
    // to be the same as the types of Accessible... probably no.

    namespace detail {
        GO_AWAY using VariantLike_rdt = std::variant<const RapidDataType*, StatusCode>;
        GO_AWAY using VariantLike_strv = std::variant<std::string_view, StatusCode>;
        GO_AWAY using VariantLike_mono = std::variant<std::monostate, StatusCode>;
    } // namespace detail

    /**
     * @brief Helper to unpack the variant and attempt to access the data as the specified type.
     *
     * Returns a pointer to the value held by the provided `RapidDataType` only if
     * it currently contains a value of type `T`.
     *
     * If `data` is `nullptr`, or if the stored value has a different type, `nullptr` is returned.
     *
     * @tparam T The type to retrieve from the variant. Must satisfy the `Accessible` concept.
     * @param data Pointer to the `RapidDataType` variant to unpack
     * @return Pointer to the contained value of type `T`, or `nullptr` if unavailable
     */
    template <Accessible T>
    const T* unpack_as(const RapidDataType* data) {
        if (!data) return nullptr;
        return std::get_if<T>(data);
    }


    inline const RapidDataType* unpack_field(const detail::VariantLike_rdt* F2) {
        if (!F2) return nullptr;
        if (auto* ptr = std::get_if<const RapidDataType*>(F2)) {
            return *ptr;
        }
        return nullptr;
    }
    inline const StatusCode* unpack_field_code(const detail::VariantLike_rdt* F2) {
        if (!F2) return nullptr;
        return std::get_if<StatusCode>(F2);
    }


    inline const std::string_view* unpack_field(const detail::VariantLike_strv* F2) {
        if (!F2) return nullptr;
        return std::get_if<std::string_view>(F2);
    }
    inline const StatusCode* unpack_field_code(const detail::VariantLike_strv* F2) {
        if (!F2) return nullptr;
        return std::get_if<StatusCode>(F2);
    }

    
    inline const std::monostate* unpack_field(const detail::VariantLike_mono* F2) {
        if (!F2) return nullptr;
        return std::get_if<std::monostate>(F2);
    }
    inline const StatusCode* unpack_field_code(const detail::VariantLike_mono* F2) {
        if (!F2) return nullptr;
        return std::get_if<StatusCode>(F2);
    }


    /**
     * @brief Converts a `RapidDataType` value to its string representation
     *
     * If `data` is `nullptr`, returns `"null"`. Otherwise, visits the contained
     * variant value and formats it as a string.
     *
     * Boolean values are converted to `"true"` or `"false"`.
     *
     * @param data Pointer to the `RapidDataType` variant to convert to string.
     * @return String representation of the contained value, or `"null"` if `data` is `nullptr`.
     */
    inline std::string to_string(const RapidDataType* data) {
        if (!data) return std::format("null");   // if nullptr
        // deference and visit the variant
        return std::visit([]<Accessible T>(const T& val)-> std::string {
            // if type is bool
            if constexpr (std::same_as<T, bool>) {
                return std::format("{}", val ? "true" : "false");
            }
            return std::format("{}", val);  // for the non-bool types
        }, *data);
    }

    /**
     * @brief Converts a status code to its string representation.
     *
     * Returns the symbolic name of a known `StatusCode` enum. If the status code is
     * not recognized, returns a fallback string in the form `"UNKNOWN-CODE-<code>"`
     *
     * @param code The `StatusCode` value to convert.
     * @return String representation of the status code.
     */
    inline std::string to_string(const StatusCode code) {
        // Macro taking one argument of type StatusCode and stringify-ing it
        #define CASE(x) case StatusCode::x: return #x     // purely done so it's easier to insert cases
        switch (code) {
            CASE(OK);
            CASE(ORPHANED);
            CASE(WARN_KEY_STORE_NEARING_CAPACITY);
            CASE(WARN_RESPONSE_CONTAINS_WARNINGS);
            CASE(ERR_KEY_STORE_FULL);
            CASE(ERR_UNKNOWN);
            CASE(ERR_INVALID_ARGUMENT);
            CASE(ERR_KEY_ALREADY_EXISTS);
            CASE(ERR_KEY_NOT_FOUND);
            CASE(ERR_VALUE_NOT_FOUND);
            CASE(ERR_SINGLE_NODE_EXPECTED);
            CASE(ERR_SOME_OPERATIONS_FAILED);
            CASE(ERR_MULTIPLE_OPERATIONS_FAILED);
            CASE(ERR_INVALID_KEY);
            CASE(ERR_INVALID_VALUE);
            CASE(ERR_INVALID_COMMAND);
            CASE(ERR_INVALID_ARGUMENT_COUNT);
            CASE(ERR_DOES_NOT_TAKE_ARGUMENTS);
            CASE(ERR_NO_ARGUMENTS_GIVEN);
            CASE(ERR_INVALID_DELIMITER);
            CASE(ERR_OUT_OF_MEMORY);
            default: return std::format("UNKNOWN-CODE-{}", static_cast<std::uint16_t>(code));
        }
        #undef CASE
    }

} // namespace RiRi::Utils