#include "doctest.h"
#include "riri/utils/Accessors.hpp"
#include "riri/RapidTypes.hpp"
#include <cstdint>
#include <string>
#include <ostream>

using namespace RiRi::Utils;

TEST_SUITE("UTILS") {

    TEST_CASE("ACCESSORS") {

        auto node_str = RiRi::RapidNode{"key", "value"};
        auto node_int = RiRi::RapidNode{"key", std::int64_t{37}};
        auto node_double = RiRi::RapidNode{"key", 3.14};
        auto node_bool = RiRi::RapidNode{"key", false};

        RiRi::RapidNode nodes[] {node_str, node_int, node_double, node_bool};

        // 1
        SUBCASE("unpack_as<string>") {
            int i = 0;
            for (auto& [_, val]: nodes) {
                if (i == 0) {
                    REQUIRE(unpack_as<std::string>(&val) != nullptr);
                    CHECK(*unpack_as<std::string>(&val) == "value");
                }
                else {
                    REQUIRE(unpack_as<std::string>(&val) == nullptr);
                }
                i++;
            }
        }

        // 2
        SUBCASE("unpack_as<int64_t>") {
            int i = 0;
            for (auto& [_, val]: nodes) {
                if (i == 1) {
                    REQUIRE(unpack_as<std::int64_t>(&val) != nullptr);
                    CHECK(*unpack_as<std::int64_t>(&val) == 37);
                }
                else {
                    REQUIRE(unpack_as<std::int64_t>(&val) == nullptr);
                }
                i++;
            }
        }

        // 3
        SUBCASE("unpack_as<double>") {
            int i = 0;
            for (auto& [_, val]: nodes) {
                if (i == 2) {
                    REQUIRE(unpack_as<double>(&val) != nullptr);
                    CHECK(*unpack_as<double>(&val) == 3.14);
                }
                else {
                    REQUIRE(unpack_as<double>(&val) == nullptr);
                }
                i++;
            }
        }

        // 4
        SUBCASE("unpack_as<bool>") {
            int i = 0;
            for (auto& [_, val]: nodes) {
                if (i == 3) {
                    REQUIRE(unpack_as<bool>(&val) != nullptr);
                    CHECK(*unpack_as<bool>(&val) == false);
                }
                else {
                    REQUIRE(unpack_as<bool>(&val) == nullptr);
                }
                i++;
            }
        }

        // 5
        // this subcase is basically a hack/workaround for testing
        // the types like ResultOrStatus, which are present in RaReSy
        // (used internally).
        SUBCASE("unpack_field") {

            // The variants are formatted exactly as in RaReSy

            // std::variant<const RapidDataType*, StatusCode>;
            detail::VariantLike_rdt data {&node_str.value};
            REQUIRE(unpack_field(&data) != nullptr);
            CHECK(*unpack_field(&data) == node_str.value);

            // std::variant<std::string_view, StatusCode>;
            detail::VariantLike_strv data_2 {node_str.key};
            REQUIRE(unpack_field(&data_2) != nullptr);
            CHECK(*unpack_field(&data_2) == node_str.key);

            // std::variant<std::monostate, StatusCode>;
            detail::VariantLike_mono data_3 {};
            REQUIRE(unpack_field(&data_3) != nullptr);
            CHECK(*unpack_field(&data_3) == std::monostate());

        }   // done so to check the helpers without getting raresy here

        // 6
        // same logic here
        SUBCASE("unpack_field_code") {
            // std::variant<const RapidDataType*, StatusCode>;
            detail::VariantLike_rdt data {RiRi::StatusCode::ERR_UNKNOWN};
            REQUIRE(unpack_field_code(&data) != nullptr);
            CHECK(*unpack_field_code(&data) == RiRi::StatusCode::ERR_UNKNOWN);

            // std::variant<std::string_view, StatusCode>;
            detail::VariantLike_strv data_2 {RiRi::StatusCode::ERR_UNKNOWN};
            REQUIRE(unpack_field_code(&data_2) != nullptr);
            CHECK(*unpack_field_code(&data_2) == RiRi::StatusCode::ERR_UNKNOWN);

            // std::variant<std::monostate, StatusCode>;
            detail::VariantLike_mono data_3 {RiRi::StatusCode::ERR_UNKNOWN};
            REQUIRE(unpack_field_code(&data_3) != nullptr);
            CHECK(*unpack_field_code(&data_3) == RiRi::StatusCode::ERR_UNKNOWN);
        }

        // 7
        SUBCASE ("to_string") {

            // (RapidDataTpye*)
            CHECK(to_string(&node_str.value) == std::string("value"));
            CHECK(to_string(&node_int.value) == std::string("37"));
            CHECK(to_string(&node_double.value) == std::string("3.14"));
            CHECK(to_string(&node_bool.value) == std::string("false"));

            // (StatusCode) TODO
            // Hmm, we need a single source of truth for status codes.
            // With the current impl. I can't think of any way to check
            // if all the corresponding code strings are present in to_string.
            // For now, we will just check what an undefined status code does
            CHECK(to_string(static_cast<RiRi::StatusCode>(555)) == "UNKNOWN-CODE-555");
        }
    }
}