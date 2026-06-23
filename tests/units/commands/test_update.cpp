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
// | 1.  UPDATE single key; key exists                           | (key, value) :: (span) :: (span, enableErrorBatched)|
// |                                                             | :: (span, enableBatched)                            |
// | 2.  UPDATE single key; key does not exist                   | (key, value) :: (span) :: (span, enableErrorBatched)|
// |                                                             | :: (span, enableBatched)                            |
// | 3.  UPDATE multiple keys; all keys exist                    | UPDATE(span)                                        |
// | 4.  UPDATE multiple keys; no keys exist                     | UPDATE(span)                                        |
// | 5.  UPDATE multiple keys; some exist                        | UPDATE(span)                                        |
// | 6.  UPDATE multiple keys; empty span                        | UPDATE(span)                                        |
// | 7.  UPDATE multiple keys; all keys exist (ErrorBatched)     | UPDATE(span, enableErrorBatched)                    |
// | 8.  UPDATE multiple keys; no keys exist (ErrorBatched)      | UPDATE(span, enableErrorBatched)                    |
// | 9.  UPDATE multiple keys; some exist (ErrorBatched)         | UPDATE(span, enableErrorBatched)                    |
// | 10. UPDATE multiple keys; empty span (Error Batched)        | UPDATE(span, enableErrorBatched)                    |
// | 11. UPDATE multiple keys; all keys exist (batched)          | UPDATE(span, enableBatched)                         |
// | 12. UPDATE multiple keys; no keys exist (batched)           | UPDATE(span, enableBatched)                         |
// | 13. UPDATE multiple keys; some exist (batched)              | UPDATE(span, enableBatched)                         |
// | 14. UPDATE multiple keys; empty span (batched)              | UPDATE(span, enableBatched)                         |
// +-------------------------------------------------------------+-----------------------------------------------------+

// NOTE: WE WILL NOT BE CHECKING FOR BOUNDARY CASES, SINCE TESTS FOR
//       RAPIDRESPONSE.HPP HAVE ALREADY COVERED IT. WE WILL GO BIG.

TEST_SUITE("Commands") {

    TEST_CASE("UPDATE") {

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
        SUBCASE("Update single key; key exists") {
            // Data for Node
            RiRi::RapidNode nodes_1[] {{"key2", RiRi::RapidDataType("raresy")}};
            RiRi::RapidNode nodes_2[] {{"key3", RiRi::RapidDataType("ad4rsh2701")}};
            RiRi::RapidNode nodes_3[] {{"key4", RiRi::RapidDataType("ShadowsLure")}};

            // (key, value)
            auto response_f = UPDATE("key1", 3.14);
            CHECK(response_f.ok() == true);
            auto updated_val = RiRi::Internal::getValue("key1");
            REQUIRE(updated_val != nullptr);
            CHECK(*updated_val == RiRi::RapidDataType(3.14));

            // (span)
            auto response_n = UPDATE(nodes_1);
            CHECK(response_n.ok() == true);
            auto updated_val2 = RiRi::Internal::getValue("key2");
            REQUIRE(updated_val2 != nullptr);
            CHECK(*updated_val2 == RiRi::RapidDataType("raresy"));
                // we will not and CANNOT use nodes[0].value to check!
                // once a node is used, do not rely on the data
                // present in it!!! It may have been moved!!

            // (span, errorBatched)
            auto response_e = UPDATE(nodes_2, RiRi::enableErrorBatched{});
            CHECK(response_e.ok() == true);
            auto updated_val3 = RiRi::Internal::getValue("key3");
            REQUIRE(updated_val3 != nullptr);
            CHECK(*updated_val3 == RiRi::RapidDataType("ad4rsh2701"));
            CHECK(response_e.totalErrorCount() == 0);
            CHECK(response_e.begin() == response_e.end());

            // (span, Batched)
            auto response_b = UPDATE(nodes_3, RiRi::enableBatched{});
            CHECK(response_b.ok() == true);
            auto updated_val4 = RiRi::Internal::getValue("key4");
            REQUIRE(updated_val4 != nullptr);
            CHECK(*updated_val4 == RiRi::RapidDataType("ShadowsLure"));
            CHECK(response_b.totalEntryCount() == 0);
            CHECK(response_b.begin() == response_b.end());
        }

        // 2
        SUBCASE("Update single key; key does not exist") {
            // Data for Node
            RiRi::RapidNode nodes[] {{"key101", RiRi::RapidDataType("raresy")}};

            // (key, value)
            auto response_f = UPDATE("key101", 3.14);
            CHECK(response_f.ok() == false);
            auto updated_val = RiRi::Internal::getValue("key101");
            CHECK(updated_val == nullptr);
            CHECK(response_f.code() == RiRi::StatusCode::ERR_KEY_NOT_FOUND);

            // (span)
            auto response_n = UPDATE(nodes);
            CHECK(response_n.ok() == false);
            auto updated_val2 = RiRi::Internal::getValue("key101");
            CHECK(response_n.errorCount() == 1);
            CHECK(updated_val2 == nullptr);
            CHECK(response_n.code() == RiRi::StatusCode::ERR_KEY_NOT_FOUND);

            // (span, errorBatched)
            auto response_e = UPDATE(nodes, RiRi::enableErrorBatched{});
            CHECK(response_e.ok() == false);
            auto updated_val3 = RiRi::Internal::getValue("key101");
            CHECK(response_e.totalErrorCount() == 1);
            CHECK(updated_val3 == nullptr);
            CHECK(response_e.code() == RiRi::StatusCode::ERR_SOME_OPERATIONS_FAILED);
            for (const auto& [target, code] : response_e) {
                CHECK(target == "key101");
                CHECK(code == RiRi::StatusCode::ERR_KEY_NOT_FOUND);
            }

            // (span, Batched)
            auto response_b = UPDATE(nodes, RiRi::enableBatched{});
            CHECK(response_b.ok() == false);
            auto updated_val4 = RiRi::Internal::getValue("key101");
            CHECK(response_b.totalEntryCount() == 1);
            CHECK(updated_val4 == nullptr);
            CHECK(response_b.code() == RiRi::StatusCode::ERR_SOME_OPERATIONS_FAILED);
            for (const auto& [target, field] : response_b) {
                CHECK(target == "key101");
                auto field_code = RiRi::Utils::unpack_field_code(&field);
                REQUIRE(field_code != nullptr);
                CHECK(*field_code == RiRi::StatusCode::ERR_KEY_NOT_FOUND);
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
        SUBCASE("Update multiple keys; all keys exist") {
            // (span)
            auto response = UPDATE(existing_nodes);
            CHECK(response.ok() == true);
            CHECK(response.errorCount() == 0);
            REQUIRE(existing_nodes[0].key == "key0");
            for (auto &[key, _] : existing_nodes) {
                auto updated_val = RiRi::Internal::getValue(key);
                REQUIRE(updated_val != nullptr);
                CHECK(*updated_val == RiRi::RapidDataType(3.14));
            }
        }

        // 4
        SUBCASE("Update multiple keys; no keys exist") {
            // (span)
            auto response = UPDATE(new_nodes);
            CHECK(response.ok() == false);
            CHECK(response.errorCount() == 100);
            for (auto &[key, _] : new_nodes) {
                auto updated_val = RiRi::Internal::getValue(key);
                CHECK(updated_val == nullptr);
            }
            CHECK(response.code() == RiRi::StatusCode::ERR_SOME_OPERATIONS_FAILED);
                // SOME OPS FAILED doesn't make sense for ALL OPS FAILED, just noting
        }

        // 5
        SUBCASE("Update multiple keys; some exist") {
            // (span)
            auto response = UPDATE(mixed_nodes);
            CHECK(response.ok() == false);
            CHECK(response.errorCount() == 100);
            REQUIRE(mixed_nodes[0].key == "key0");
            for (int i = 0; i < 100; i++) {
                auto updated_val = RiRi::Internal::getValue(mixed_nodes[i].key);
                REQUIRE(updated_val != nullptr);
                CHECK(*updated_val == RiRi::RapidDataType(3.14));
            }
            for (int i = 100; i < 200; i++) {
                auto updated_val = RiRi::Internal::getValue(mixed_nodes[i].key);
                CHECK(updated_val == nullptr);
            }
            CHECK(response.code() == RiRi::StatusCode::ERR_SOME_OPERATIONS_FAILED);
        }

        // 6
        SUBCASE("Update multiple keys; empty span") {
            auto response = UPDATE({});
            CHECK(response.ok() == false);
            CHECK(response.code() == RiRi::StatusCode::WARN_ZERO_NODES_PROVIDED);
            CHECK(response.errorCount() == 0);
        }

        // 7
        SUBCASE("Update multiple keys; all keys exist (ErrorBatched)") {
            auto response = UPDATE(existing_nodes, RiRi::enableErrorBatched{});
            CHECK(response.ok() == true);
            CHECK(response.code() == RiRi::StatusCode::OK);
            CHECK(response.totalErrorCount() == 0);
            REQUIRE(existing_nodes[0].key == "key0");
            for (auto &[key, _] : existing_nodes) {
                auto updated_val = RiRi::Internal::getValue(key);
                REQUIRE(updated_val != nullptr);
                CHECK(*updated_val == RiRi::RapidDataType(3.14));
            }
            REQUIRE(response.begin() == response.end());
        }

        // 8
        SUBCASE("Update multiple keys; no keys exist (ErrorBatched)") {
            auto response = UPDATE(new_nodes, RiRi::enableErrorBatched{});
            CHECK(response.ok() == false);
            CHECK(response.code() == RiRi::StatusCode::ERR_MULTIPLE_OPERATIONS_FAILED);
            CHECK(response.totalErrorCount() == 100);
            for (auto &[key, _] : new_nodes) {
                auto updated_val = RiRi::Internal::getValue(key);
                REQUIRE(updated_val == nullptr);
            }
            REQUIRE(response.begin() != response.end());
            auto tracked_size = response.end() - response.begin();
            CHECK(tracked_size == 8);
            int i = 0;
            for (auto &[key, code] : response) {
                CHECK(key == new_nodes[i++].key);
                CHECK(code == RiRi::StatusCode::ERR_KEY_NOT_FOUND);
            }
        }

        // 9
        SUBCASE("Update multiple keys; some exist (ErrorBatched)") {
            auto response = UPDATE(mixed_nodes, RiRi::enableErrorBatched{});
            CHECK(response.ok() == false);
            CHECK(response.code() == RiRi::StatusCode::ERR_MULTIPLE_OPERATIONS_FAILED);
            CHECK(response.totalErrorCount() == 100);
            REQUIRE(mixed_nodes[0].key == "key0");
            for (int i = 0; i < 100; i++) {
                auto updated_val = RiRi::Internal::getValue(mixed_nodes[i].key);
                REQUIRE(updated_val != nullptr);
                CHECK(*updated_val == RiRi::RapidDataType(3.14));
            }
            for (int i = 100; i < 200; i++) {
                auto updated_val = RiRi::Internal::getValue(mixed_nodes[i].key);
                REQUIRE(updated_val == nullptr);
            }
            REQUIRE(response.begin() != response.end());
            auto tracked_size = response.end() - response.begin();
            CHECK(tracked_size == 8);
            int i = 100;
            for (auto &[key, code] : response) {
                CHECK(key == mixed_nodes[i++].key);
                CHECK(code == RiRi::StatusCode::ERR_KEY_NOT_FOUND);
            }
        }

        // 10
        SUBCASE("Update multiple keys; empty span") {
            auto response = UPDATE({}, RiRi::enableErrorBatched{});
            CHECK(response.ok() == false);
            CHECK(response.code() == RiRi::StatusCode::WARN_ZERO_NODES_PROVIDED);
            CHECK(response.totalErrorCount() == 0);
            REQUIRE(response.begin() == response.end());
        }

        // 11
        SUBCASE("Update multiple keys; all keys exist (batched)") {
            auto response = UPDATE(existing_nodes, RiRi::enableBatched{});
            CHECK(response.ok() == true);
            CHECK(response.code() == RiRi::StatusCode::OK);
            CHECK(response.totalEntryCount() == 0);
            REQUIRE(existing_nodes[0].key == "key0");
            for (auto &[key, _] : existing_nodes) {
                auto updated_val = RiRi::Internal::getValue(key);
                REQUIRE(updated_val != nullptr);
                CHECK(*updated_val == RiRi::RapidDataType(3.14));
            }
            REQUIRE(response.begin() == response.end());
        }

        // 12
        SUBCASE("Update multiple keys; no keys exist (batched)") {
            auto response = UPDATE(new_nodes, RiRi::enableBatched{});
            CHECK(response.ok() == false);
            CHECK(response.code() == RiRi::StatusCode::ERR_SOME_OPERATIONS_FAILED);
            CHECK(response.totalEntryCount() == 100);
            for (auto &[key, _] : new_nodes) {
                auto updated_val = RiRi::Internal::getValue(key);
                REQUIRE(updated_val == nullptr);
            }
            REQUIRE(response.begin() != response.end());
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
        SUBCASE("Update multiple keys; some exist (batched)") {
            auto response = UPDATE(mixed_nodes, RiRi::enableBatched{});
            CHECK(response.ok() == false);
            CHECK(response.code() == RiRi::StatusCode::ERR_SOME_OPERATIONS_FAILED);
            CHECK(response.totalEntryCount() == 100);
            REQUIRE(mixed_nodes[0].key == "key0");
            for (int i = 0; i < 100; i++) {
                auto updated_val = RiRi::Internal::getValue(mixed_nodes[i].key);
                REQUIRE(updated_val != nullptr);
                CHECK(*updated_val == RiRi::RapidDataType(3.14));
            }
            for (int i = 100; i < 200; i++) {
                auto updated_val = RiRi::Internal::getValue(mixed_nodes[i].key);
                REQUIRE(updated_val == nullptr);
            }
            REQUIRE(response.begin() != response.end());
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
        SUBCASE("Update multiple keys; empty span (batched)") {
            auto response = UPDATE({}, RiRi::enableBatched{});
            CHECK(response.ok() == false);
            CHECK(response.code() == RiRi::StatusCode::WARN_ZERO_NODES_PROVIDED);
            CHECK(response.totalEntryCount() == 0);
            REQUIRE(response.begin() == response.end());
        }
    }
}