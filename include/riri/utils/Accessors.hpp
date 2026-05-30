#pragma once    // ACCESSORS.HPP

#include <concepts>
#include <string>
#include <format>

#include "riri/RapidResponse.hpp"
#include "riri/RapidTypes.hpp"

namespace RiRi::Utils {

    template <typename T>
    concept Accessible = std::same_as<T, std::string>
                      || std::same_as<T, int64_t>
                      || std::same_as<T, bool>
                      || std::same_as<T, double>;

    /**
     *
     * @tparam T
     * @param data
     * @return
     */
    template <Accessible T>
    const T* unpack_as(const RapidDataType* data) {
        if (!data) return nullptr;
        return std::get_if<T>(data);
    }


    /**
     *
     * @param data
     * @return
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
     *
     * @param code
     * @return
     */
    inline std::string to_string(const Response::StatusCode code) {
        // Macro taking one argument of type StatusCode and stringify-ing it
        #define CASE(x) case Response::StatusCode::x: return #x     // purely done so it's easier to insert cases
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
            default: return std::format("UNKNOWN({})", static_cast<uint16_t>(code));
        }
        #undef CASE
    }
}