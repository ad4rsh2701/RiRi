#pragma once

#include <cstdint>
#include <string_view>
#include <utility>
#include <new>

enum class StatusCode : std::uint8_t {
    // Success codes (0-99)
    OK = 0,
    OK_WITH_ERRORS = 1,

    // Client errors (100-199)
    INVALID_ARGUMENT = 100,
    NOT_FOUND = 101,
    TYPE_ERROR = 102,
    TOO_MANY_ERRORS = 103,

    // Server errors (200+)
    INTERNAL_ERROR = 200,
    MULTI_FAILED = 201,
};


struct RapidResponse {
    StatusCode response;

    constexpr bool ok() const noexcept {
        return response == StatusCode::OK;
    }
};

struct RiRiResponseContainer {
    static constexpr size_t INLINE_CAPACITY = 4;

    StatusCode overall_code = StatusCode::OK;

    alignas(std::pair<std::string_view, StatusCode>)
    // weird looking syntax, but not actually weird
    char inline_storage[INLINE_CAPACITY * sizeof(std::pair<std::string_view, StatusCode>)];

    std::pair<std::string_view, StatusCode>* failures = nullptr;
    std::uint8_t failure_count = 0;
    std::uint8_t capacity = 0;

    RiRiResponseContainer() noexcept {
        // weird looking syntax and actually weird
        failures = reinterpret_cast<std::pair<std::string_view, StatusCode>*>(inline_storage);
        capacity = INLINE_CAPACITY;
    }

    void addFailure(std::string_view key, StatusCode code) {
        if (failure_count >= capacity) {
            overall_code = StatusCode::TOO_MANY_ERRORS;
            return;
        }
        // weird looking syntax and actually not not weird?
        new (&failures[failure_count]) std::pair{key, code};
        ++failure_count;
    }

    void reset() {
        failure_count = 0;
        overall_code = StatusCode::OK;
        // No destructors needed since we only hold trivially destructible types
    }

    // for iteration
    auto begin() const noexcept { return failures; }
    auto end()   const noexcept { return failures + failure_count; }
};

