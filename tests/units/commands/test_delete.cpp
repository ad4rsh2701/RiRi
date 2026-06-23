#include "DataManager.h"
#include "doctest.h"
#include "ostream"
#include "riri/Commands.hpp"
#include "riri/RapidTypes.hpp"
#include "riri/utils/Accessors.hpp"

using namespace RiRi::Commands;

// =============================================== LISTS OF SUBCASES ===================================================
// +-------------------------------------------------------------+-----------------------------------------------------+
// |                             SUBCASE                         |                    Overload                         |
// +-------------------------------------------------------------+-----------------------------------------------------+
// | 1.  DELETE single key; key exists                           | (key) :: (span) :: (span, enableErrorBatched)       |
// |                                                             | :: (span, enableBatched)                            |
// | 2.  DELETE single key; key does not exist                   | (key) :: (span) :: (span, enableErrorBatched)       |
// |                                                             | :: (span, enableBatched)                            |
// | 3.  DELETE multiple keys; all keys exist                    | UPDATE(span)                                        |
// | 4.  DELETE multiple keys; no keys exist                     | UPDATE(span)                                        |
// | 5.  DELETE multiple keys; some exist                        | UPDATE(span)                                        |
// | 6.  DELETE multiple keys; empty span                        | UPDATE(span)                                        |
// | 7.  DELETE multiple keys; all keys exist (ErrorBatched)     | UPDATE(span, enableErrorBatched)                    |
// | 8.  DELETE multiple keys; no keys exist (ErrorBatched)      | UPDATE(span, enableErrorBatched)                    |
// | 9.  DELETE multiple keys; some exist (ErrorBatched)         | UPDATE(span, enableErrorBatched)                    |
// | 10. DELETE multiple keys; empty span (Error Batched)        | UPDATE(span, enableErrorBatched)                    |
// | 11. DELETE multiple keys; all keys exist (batched)          | UPDATE(span, enableBatched)                         |
// | 12. DELETE multiple keys; no keys exist (batched)           | UPDATE(span, enableBatched)                         |
// | 13. DELETE multiple keys; some exist (batched)              | UPDATE(span, enableBatched)                         |
// | 14. DELETE multiple keys; empty span (batched)              | UPDATE(span, enableBatched)                         |
// +-------------------------------------------------------------+-----------------------------------------------------+


TEST_SUITE("Commands") {


    TEST_CASE("DELETE") {

        // Data
        RiRi::Internal::clearMap();
        REQUIRE(RiRi::Internal::size() == 0);

        // we will not use SET for obvious reasons
        for (int i = 0; i < 100; i++) {
            RiRi::Internal::setValue(
                std::move("key"+std::to_string(i)),
                std::move(RiRi::RapidDataType("RiRi")));
        }
        REQUIRE(RiRi::Internal::size() == 100);


        // 1
        SUBCASE("DELETE single key; key exists") {

            // Data for span
            RiRi::RapidNode nodes_1[] {{"key1"}};
            RiRi::RapidNode nodes_2[] {{"key2"}};
            RiRi::RapidNode nodes_3[] {{"key3"}};

            // (key)
            auto response_f = DELETE("key0");
            CHECK(response_f.code() == RiRi::StatusCode::OK);
            CHECK(RiRi::Internal::size() == 99);

            // (span)
            auto response_n = DELETE(nodes_1);
            CHECK(response_n.code() == RiRi::StatusCode::OK);
            CHECK(RiRi::Internal::size() == 98);

            // (span, ErrorBatched)
            auto response_e = DELETE(nodes_2, RiRi::enableErrorBatched{});
            CHECK(response_e.code() == RiRi::StatusCode::OK);
            CHECK(RiRi::Internal::size() == 97);
            CHECK(response_e.begin() == response_e.end());
            REQUIRE(response_e.totalErrorCount() == 0);

            // (span, Batched)
            auto response_b = DELETE(nodes_3, RiRi::enableBatched{});
            CHECK(response_b.code() == RiRi::StatusCode::OK);
            CHECK(RiRi::Internal::size() == 96);
            CHECK(response_b.begin() == response_b.end());
            REQUIRE(response_b.totalEntryCount() == 0);
        }

        // 2
        SUBCASE("DELETE single key; key does not exist") {
            // Data for span
            RiRi::RapidNode nodes[] {{"key101"}};

            // (key)
            auto response_f = DELETE("key101");
            CHECK(response_f.code() == RiRi::StatusCode::ERR_KEY_NOT_FOUND);
            CHECK(RiRi::Internal::size() == 100);

            // (span)
            auto response_n = DELETE(nodes);
            CHECK(response_n.code() == RiRi::StatusCode::ERR_KEY_NOT_FOUND);
            CHECK(RiRi::Internal::size() == 100);

            // (span, ErrorBatched)
            auto response_e = DELETE(nodes, RiRi::enableErrorBatched{});
            CHECK(response_e.code() == RiRi::StatusCode::ERR_SOME_OPERATIONS_FAILED);
            CHECK(RiRi::Internal::size() == 100);
            CHECK(response_e.begin() != response_e.end());
            REQUIRE(response_e.totalErrorCount() == 1);
            for (auto [key, code]: response_e) {
                CHECK(key == "key101");
                CHECK(code == RiRi::StatusCode::ERR_KEY_NOT_FOUND);
            }

            // (span, Batched)
            auto response_b = DELETE(nodes, RiRi::enableBatched{});
            CHECK(response_b.code() == RiRi::StatusCode::ERR_SOME_OPERATIONS_FAILED);
            CHECK(RiRi::Internal::size() == 100);
            CHECK(response_b.begin() != response_b.end());
            REQUIRE(response_b.totalEntryCount() == 1);
            for (auto [key, code]: response_b) {
                CHECK(key == "key101");
                REQUIRE(RiRi::Utils::unpack_field_code(&code) != nullptr);
                CHECK(*RiRi::Utils::unpack_field_code(&code) == RiRi::StatusCode::ERR_KEY_NOT_FOUND);
            }
        }


        // Data Setup for the upcoming cases
        RiRi::RapidNode existing_nodes[100];
        RiRi::RapidNode new_nodes[100];
        RiRi::RapidNode mixed_nodes[200];

        for (int i = 0; i < 100; i++) {
            mixed_nodes[i].key = std::move("key"+std::to_string(i));
            existing_nodes[i].key = std::move("key"+std::to_string(i));
            mixed_nodes[i].value = std::move(RiRi::RapidDataType(3.14));
            existing_nodes[i].value = std::move(RiRi::RapidDataType(3.14));

            new_nodes[i].key = std::move("key"+std::to_string(200+i));
            mixed_nodes[i+100].key = std::move("key"+std::to_string(200+i));
            new_nodes[i].value = std::move(RiRi::RapidDataType("raresy"));
            mixed_nodes[i+100].value = std::move(RiRi::RapidDataType("raresy"));
        }


        // 3
        SUBCASE("DELETE multiple keys; all keys exist") {
            auto response = DELETE(existing_nodes);
            CHECK(response.code() == RiRi::StatusCode::OK);
            REQUIRE(RiRi::Internal::size() == 0);
            CHECK(RiRi::Internal::getValue("key0") == nullptr);
        }

        // 4
        SUBCASE("DELETE multiple keys; no keys exist") {
            auto response = DELETE(new_nodes);
            CHECK(response.code() == RiRi::StatusCode::ERR_SOME_OPERATIONS_FAILED);
            REQUIRE(RiRi::Internal::size() == 100);
            CHECK(RiRi::Internal::getValue("key0") != nullptr);
        }

        // 5
        SUBCASE("DELETE multiple keys; some exist") {
            auto response = DELETE(mixed_nodes);
            CHECK(response.code() == RiRi::StatusCode::ERR_SOME_OPERATIONS_FAILED);
            REQUIRE(RiRi::Internal::size() == 0);
            CHECK(RiRi::Internal::getValue("key0") == nullptr);
        }

        // 6
        SUBCASE("DELETE multiple keys; empty span") {
            auto response = DELETE(std::span<RiRi::RapidNode>{}); // NOTE: {} alone is ambiguous
            CHECK(response.code() == RiRi::StatusCode::WARN_ZERO_NODES_PROVIDED);
            CHECK(RiRi::Internal::size() == 100);
            CHECK(RiRi::Internal::getValue("key0") != nullptr);
        }

        // 7
        SUBCASE("DELETE multiple keys; all keys exist (ErrorBatched)") {
            auto response = DELETE(existing_nodes, RiRi::enableErrorBatched{});
            CHECK(response.code() == RiRi::StatusCode::OK);
            REQUIRE(RiRi::Internal::size() == 0);
            CHECK(RiRi::Internal::getValue("key0") == nullptr);
            CHECK(response.begin() == response.end());
            REQUIRE(response.totalErrorCount() == 0);
            for (auto &[key, _] : existing_nodes) {
                auto val = RiRi::Internal::getValue(key);
                REQUIRE(val == nullptr);
            }
        }

        // 8
        SUBCASE("DELETE multiple keys; no keys exist (ErrorBatched)") {
            auto response = DELETE(new_nodes, RiRi::enableErrorBatched{});
            CHECK(response.code() == RiRi::StatusCode::ERR_MULTIPLE_OPERATIONS_FAILED);
            REQUIRE(RiRi::Internal::size() == 100);
            CHECK(RiRi::Internal::getValue("key0") != nullptr);
            CHECK(response.begin() != response.end());
            REQUIRE(response.totalErrorCount() == 100);
            for (auto &[key, _] : new_nodes) {
                auto val = RiRi::Internal::getValue(key);
                REQUIRE(val == nullptr);
            }   // ehhhh, why not
            auto tracked_size = response.end() - response.begin();
            CHECK(tracked_size == 8);
            int i = 0;
            for (auto [key, code]: response) {
                CHECK(key == new_nodes[i++].key);
                CHECK(code == RiRi::StatusCode::ERR_KEY_NOT_FOUND);
            }
        }

        // 9
        SUBCASE("DELETE multiple keys; some exist (ErrorBatched)") {
            auto response = DELETE(mixed_nodes, RiRi::enableErrorBatched{});
            CHECK(response.code() == RiRi::StatusCode::ERR_MULTIPLE_OPERATIONS_FAILED);
            REQUIRE(RiRi::Internal::size() == 0);
            CHECK(RiRi::Internal::getValue("key0") == nullptr);
            CHECK(response.begin() != response.end());
            REQUIRE(response.totalErrorCount() == 100);
            for (auto &[key, _] : mixed_nodes) {
                auto val = RiRi::Internal::getValue(key);
                REQUIRE(val == nullptr);
            }
            auto tracked_size = response.end() - response.begin();
            CHECK(tracked_size == 8);
            int i = 100;
            for (auto [key, code]: response) {
                CHECK(key == mixed_nodes[i++].key);
                CHECK(code == RiRi::StatusCode::ERR_KEY_NOT_FOUND);
            }
        }

        // 10
        SUBCASE("DELETE multiple keys; empty span (Error Batched)") {
            auto response = DELETE({}, RiRi::enableErrorBatched{});
            CHECK(response.ok() == false);
            CHECK(response.code() == RiRi::StatusCode::WARN_ZERO_NODES_PROVIDED);
            CHECK(response.totalErrorCount() == 0);
            REQUIRE(response.begin() == response.end());
        }

        // 11
        SUBCASE("DELETE multiple keys; all keys exist (batched)") {
            auto response = DELETE(existing_nodes, RiRi::enableBatched{});
            CHECK(response.code() == RiRi::StatusCode::OK);
            REQUIRE(RiRi::Internal::size() == 0);
            CHECK(RiRi::Internal::getValue("key0") == nullptr);
            CHECK(response.begin() == response.end());
            REQUIRE(response.totalEntryCount() == 0);
            for (auto &[key, _] : existing_nodes) {
                auto val = RiRi::Internal::getValue(key);
                REQUIRE(val == nullptr);
            }
        }

        // 12
        SUBCASE("DELETE multiple keys; no keys exist (batched)") {
            auto response = DELETE(new_nodes, RiRi::enableBatched{});
            CHECK(response.code() == RiRi::StatusCode::ERR_SOME_OPERATIONS_FAILED);
            REQUIRE(RiRi::Internal::size() == 100);
            CHECK(RiRi::Internal::getValue("key0") != nullptr);
            CHECK(response.begin() != response.end());
            REQUIRE(response.totalEntryCount() == 100);
            for (auto &[key, _] : new_nodes) {
                auto updated_val = RiRi::Internal::getValue(key);
                REQUIRE(updated_val == nullptr);
            }
            auto tracked_size = response.end() - response.begin();
            CHECK(tracked_size == response.totalEntryCount());
            int i = 0;
            for (auto &[key, code] : response) {
                CHECK(key == new_nodes[i++].key);
                REQUIRE(RiRi::Utils::unpack_field_code(&code) != nullptr);
                CHECK(*RiRi::Utils::unpack_field_code(&code) == RiRi::StatusCode::ERR_KEY_NOT_FOUND);
            }
        }

        // 13
        SUBCASE("DELETE multiple keys; some exist (batched)") {
            auto response = DELETE(mixed_nodes, RiRi::enableBatched{});
            CHECK(response.code() == RiRi::StatusCode::ERR_SOME_OPERATIONS_FAILED);
            REQUIRE(RiRi::Internal::size() == 0);
            CHECK(RiRi::Internal::getValue("key0") == nullptr);
            CHECK(response.begin() != response.end());
            REQUIRE(response.totalEntryCount() == 100);
            for (auto &[key, _] : mixed_nodes) {
                auto val = RiRi::Internal::getValue(key);
                REQUIRE(val == nullptr);
            }
            auto tracked_size = response.end() - response.begin();
            CHECK(tracked_size == response.totalEntryCount());
            int i = 100;
            for (auto &[key, code] : response) {
                CHECK(key == mixed_nodes[i++].key);
                REQUIRE(RiRi::Utils::unpack_field_code(&code) != nullptr);
                CHECK(*RiRi::Utils::unpack_field_code(&code) == RiRi::StatusCode::ERR_KEY_NOT_FOUND);
            }
        }

        // 14
        SUBCASE("DELETE multiple keys; empty span (batched)") {
            auto response = DELETE({}, RiRi::enableBatched{});
            CHECK(response.ok() == false);
            CHECK(response.code() == RiRi::StatusCode::WARN_ZERO_NODES_PROVIDED);
            CHECK(response.begin() == response.end());
            CHECK(response.totalEntryCount() == 0);
        }
    }
}