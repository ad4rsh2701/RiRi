#include "DataManager.h"
#include "doctest.h"
#include <ostream>
#include "riri/Commands.hpp"
#include "riri/RapidTypes.hpp"
#include "riri/utils/Accessors.hpp"
#include <array>

using namespace RiRi::Commands;

// =============================================== LISTS OF SUBCASES ===================================================
// +-------------------------------------------------------------+-----------------------------------------------------+
// |                             SUBCASE                         |                    Overload                         |
// +-------------------------------------------------------------+-----------------------------------------------------+
// | 1.  SET single key; unique key                              | (key, value) :: (span) :: (span, enableErrorBatched)|
// |                                                             | :: (span, enableBatched)                            |
// | 2.  SET single key; duplicate key                           | (key, value) :: (span) :: (span, enableErrorBatched)|
// |                                                             | :: (span, enableBatched)                            |
// | 3.  SET multiple keys; unique keys                          | SET(span)                                           |
// | 4.  SET multiple keys; duplicate keys                       | SET(span)                                           |
// | 5.  SET multiple keys; mixed keys                           | SET(span)                                           |
// | 6.  SET multiple keys; empty span                           | SET(span)                                           |
// | 7.  SET multiple keys; unique keys (ErrorBatched)           | SET(span, enableErrorBatched)                       |
// | 8.  SET multiple keys; duplicate keys (ErrorBatched)        | SET(span, enableErrorBatched)                       |
// | 9.  SET multiple keys; mixed keys (ErrorBatched)            | SET(span, enableErrorBatched)                       |
// | 10. SET multiple keys; empty span (Error Batched)           | SET(span, enableErrorBatched)                       |
// | 11. SET multiple keys; unique keys (batched)                | SET(span, enableBatched)                            |
// | 12. SET multiple keys; duplicate keys (batched)             | SET(span, enableBatched)                            |
// | 13. SET multiple keys; mixed keys (batched)                 | SET(span, enableBatched)                            |
// | 14. SET multiple keys; empty span (batched)                 | SET(span, enableBatched)                            |
// +-------------------------------------------------------------+-----------------------------------------------------+

// NOTE: WE WILL NOT BE CHECKING FOR BOUNDARY CASES, SINCE TESTS FOR
//       RAPIDRESPONSE.HPP HAVE ALREADY COVERED IT. WE WILL GO BIG.


TEST_SUITE ("Commands") {

    TEST_CASE ("SET") {

        // Clean slate
        RiRi::Internal::clearMap();
        REQUIRE (RiRi::Internal::size() == 0);

        // Data
        for (int i = 0; i < 100; i++) {
            RiRi::Internal::setValue(
                std::move("key"+std::to_string(i)),
                std::move(RiRi::RapidDataType("RiRi")));
        }   // key0...key99 are already in the map
            // new values -> "raresy" or 3.14, existing values -> "RiRi"

        // 1
        SUBCASE ("SET single key; unique key") {
            // (key, value)
            auto response_f = SET("key100", "raresy");
            CHECK(response_f.ok() == true);
            CHECK(RiRi::Internal::size() == 101);
            REQUIRE(RiRi::Internal::getValue("key100") != nullptr);
            CHECK(*RiRi::Internal::getValue("key100") == RiRi::RapidDataType("raresy"));

            // (span)
            RiRi::RapidNode nodes[] {{"key101", "raresy"}};
            auto response_n = SET(nodes);
            CHECK(response_n.ok() == true);
            CHECK(RiRi::Internal::size() == 102);
            REQUIRE(RiRi::Internal::getValue("key101") != nullptr);
            CHECK(*RiRi::Internal::getValue("key101") == RiRi::RapidDataType("raresy"));

            // (span, ErrorBatched)
            RiRi::RapidNode nodes_2[] {{"key102", "raresy"}};
            auto response_e = SET(nodes_2, RiRi::enableErrorBatched{});
            CHECK(response_e.ok() == true);
            CHECK(RiRi::Internal::size() == 103);
            REQUIRE(RiRi::Internal::getValue("key102") != nullptr);
            CHECK(response_e.totalErrorCount() == 0);
            CHECK(response_e.begin() == response_e.end());

            // (span, Batched)
            RiRi::RapidNode nodes_3[] {{"key103", "raresy"}};
            auto response_b = SET(nodes_3, RiRi::enableBatched{});
            CHECK(response_b.ok() == true);
            CHECK(RiRi::Internal::size() == 104);
            REQUIRE(RiRi::Internal::getValue("key103") != nullptr);
            CHECK(*RiRi::Internal::getValue("key103") == RiRi::RapidDataType("raresy"));
            CHECK(response_b.totalEntryCount() == 0);
            CHECK(response_b.begin() == response_b.end());
        }

        // 2
        SUBCASE ("SET single key; duplicate key") {
            // (key, value)
            auto response_f = SET("key1", "raresy");
            CHECK(response_f.ok() == false);
            CHECK(response_f.code() == RiRi::StatusCode::ERR_KEY_ALREADY_EXISTS);
            CHECK(RiRi::Internal::size() == 100);
            REQUIRE(RiRi::Internal::getValue("key1") != nullptr);
            CHECK(*RiRi::Internal::getValue("key1") != RiRi::RapidDataType("raresy"));
            CHECK(*RiRi::Internal::getValue("key1") == RiRi::RapidDataType("RiRi"));    // original value retained

            // (span)
            RiRi::RapidNode nodes[] {{"key1", "raresy"}};
            auto response_n = SET(nodes);
            CHECK(response_n.ok() == false);
            CHECK(response_n.code() == RiRi::StatusCode::ERR_KEY_ALREADY_EXISTS);
            CHECK(RiRi::Internal::size() == 100);
            REQUIRE(RiRi::Internal::getValue("key1") != nullptr);
            CHECK(*RiRi::Internal::getValue("key1") != RiRi::RapidDataType("raresy"));
            CHECK(*RiRi::Internal::getValue("key1") == RiRi::RapidDataType("RiRi"));

            // (span, ErrorBatched)
            RiRi::RapidNode nodes_2[] {{"key1", "raresy"}};
            auto response_e = SET(nodes_2, RiRi::enableErrorBatched{});
            CHECK(response_e.ok() == false);
            CHECK(RiRi::Internal::size() == 100);
            REQUIRE(RiRi::Internal::getValue("key1") != nullptr);
            CHECK(*RiRi::Internal::getValue("key1") != RiRi::RapidDataType("raresy"));
            CHECK(*RiRi::Internal::getValue("key1") == RiRi::RapidDataType("RiRi"));
            CHECK(response_e.totalErrorCount() == 1);
            CHECK(response_e.code() == RiRi::StatusCode::ERR_SOME_OPERATIONS_FAILED);
            CHECK(response_e.begin() != response_e.end());
            for (auto &[key, code]: response_e) {
                CHECK(key == "key1");
                CHECK(code == RiRi::StatusCode::ERR_KEY_ALREADY_EXISTS);
            }
        }

        // data for upcoming cases
        std::array<RiRi::RapidNode, 100> unique;
        std::array<RiRi::RapidNode, 100> duplicate;
        std::array<RiRi::RapidNode, 200> mixed;

        for (int i = 0; i < 100; i++) {
            unique[i].key = "key" + std::to_string(100 + i);
            unique[i].value = RiRi::RapidDataType("raresy");
            duplicate[i].key = "key" + std::to_string(i);
            duplicate[i].value = RiRi::RapidDataType("raresy");
            mixed[i].key = "key" + std::to_string(i);
            mixed[i].value = RiRi::RapidDataType(3.14);
            mixed[i+100].key = "key" + std::to_string(100 + i);
            mixed[i+100].value = RiRi::RapidDataType(3.14);
        }

        // keeping a copy of the nodes for reference
        auto unique_ref = unique;
        auto duplicate_ref = duplicate;
        auto mixed_ref = mixed;
            // Why? Because SET moves the data from the nodes.
            // Usually the workflow would be to make a copy of your
            // original key-values and then pass them over to RiRi

        // 3
        SUBCASE ("SET multiple keys; unique keys") {
            // (span)
            auto response = SET(unique);
            CHECK(response.ok() == true);
            CHECK(RiRi::Internal::size() == 200);
            for (auto &[key, _] : unique_ref) {
                    // using unique_ref instead of unique (unique is now empty)
                REQUIRE(RiRi::Internal::getValue(key) != nullptr);
                CHECK(*RiRi::Internal::getValue(key) == RiRi::RapidDataType("raresy"));
            }
        }

        // 4
        SUBCASE ("SET multiple keys; duplicate keys") {
            // (span)
            auto response = SET(duplicate);
            CHECK(response.ok() == false);
            CHECK(response.code() == RiRi::StatusCode::ERR_SOME_OPERATIONS_FAILED);
            CHECK(RiRi::Internal::size() == 100);
            for (auto &[key, _] : duplicate_ref) {
                REQUIRE(RiRi::Internal::getValue(key) != nullptr);
                CHECK(*RiRi::Internal::getValue(key) != RiRi::RapidDataType("raresy"));
                CHECK(*RiRi::Internal::getValue(key) == RiRi::RapidDataType("RiRi"));
            }
        }

        // 5
        SUBCASE ("SET multiple keys; mixed keys") {
            // (span)
            auto response = SET(mixed);
            CHECK(response.ok() == false);
            CHECK(response.code() == RiRi::StatusCode::ERR_SOME_OPERATIONS_FAILED);
            CHECK(RiRi::Internal::size() == 200);
            for (auto &[key, _] : unique_ref) {
                REQUIRE(RiRi::Internal::getValue(key) != nullptr);
                CHECK(*RiRi::Internal::getValue(key) == RiRi::RapidDataType(3.14));
            }
            for (auto &[key, _] : duplicate_ref) {
                REQUIRE(RiRi::Internal::getValue(key) != nullptr);
                CHECK(*RiRi::Internal::getValue(key) == RiRi::RapidDataType("RiRi"));
            }
            // we are using unique_ref and duplicate_ref, since mixed contains all the keys
            // present in these, purely for bifurcation purpose.
        }

        // 6
        SUBCASE ("SET multiple keys; empty span") {
            // (span)
            auto response = SET({});
            CHECK(response.ok() == false);
            CHECK(response.code() == RiRi::StatusCode::WARN_ZERO_NODES_PROVIDED);
            CHECK(RiRi::Internal::size() == 100);
            for (auto &[key, _] : duplicate_ref) {
                REQUIRE(RiRi::Internal::getValue(key) != nullptr);
                CHECK(*RiRi::Internal::getValue(key) == RiRi::RapidDataType("RiRi"));
                    // ensuring that no data got touched.
            }
        }

        // 7
        SUBCASE ("SET multiple keys; unique keys (ErrorBatched)") {
            // (span, ErrorBatched)
            auto response = SET(unique, RiRi::enableErrorBatched{});
            CHECK(response.ok() == true);
            CHECK(RiRi::Internal::size() == 200);
            CHECK(response.totalErrorCount() == 0);
            CHECK(response.begin() == response.end());
            for (auto &[key, _] : unique_ref) {
                REQUIRE(RiRi::Internal::getValue(key) != nullptr);
                CHECK(*RiRi::Internal::getValue(key) == RiRi::RapidDataType("raresy"));
            }
            CHECK(response.end() - response.begin() == 0);  // ehhh... redundant, I know.
        }

        // 8
        SUBCASE ("SET multiple keys; duplicate keys (ErrorBatched)") {
            // (span, ErrorBatched)
            auto response = SET(duplicate, RiRi::enableErrorBatched{});
            CHECK(response.ok() == false);
            CHECK(response.code() == RiRi::StatusCode::ERR_MULTIPLE_OPERATIONS_FAILED);
            CHECK(RiRi::Internal::size() == 100);
            CHECK(response.totalErrorCount() == 100);
            CHECK(response.begin() != response.end());
            for (auto &[key, _] : duplicate_ref) {
                REQUIRE(RiRi::Internal::getValue(key) != nullptr);
                CHECK(*RiRi::Internal::getValue(key) != RiRi::RapidDataType("raresy"));
                CHECK(*RiRi::Internal::getValue(key) == RiRi::RapidDataType("RiRi"));
            }
            int i = 0;
            for (auto &[key, code] : response) {
                CHECK(key == duplicate_ref[i++].key);
                CHECK(code == RiRi::StatusCode::ERR_KEY_ALREADY_EXISTS);
            }
            CHECK(response.end() - response.begin() == 8);
            CHECK(i == 8);  // 8 entries + 1 (i++)
        }

        // 9
        SUBCASE ("SET multiple keys; mixed keys (ErrorBatched)") {
            // (span, ErrorBatched)
            auto response = SET(mixed, RiRi::enableErrorBatched{});
            CHECK(response.ok() == false);
            CHECK(response.code() == RiRi::StatusCode::ERR_MULTIPLE_OPERATIONS_FAILED);
            CHECK(RiRi::Internal::size() == 200);
            CHECK(response.totalErrorCount() == 100);
            CHECK(response.begin() != response.end());
            for (auto &[key, _] : unique_ref) {
                REQUIRE(RiRi::Internal::getValue(key) != nullptr);
                CHECK(*RiRi::Internal::getValue(key) == RiRi::RapidDataType(3.14));
            }
            for (auto &[key, _] : duplicate_ref) {
                REQUIRE(RiRi::Internal::getValue(key) != nullptr);
                CHECK(*RiRi::Internal::getValue(key) == RiRi::RapidDataType("RiRi"));
            }
            int i = 0;
            for (auto &[key, code] : response) {
                CHECK(key == mixed_ref[i++].key);   // initial 100 keys of mixed are same as duplicate
            }
            CHECK(response.end() - response.begin() == 8);
            CHECK(i == 8);  // 0...7 entries + 1 (i++)
        }

        // 10
        SUBCASE ("SET multiple keys; empty span (ErrorBatched)") {
            // (span, ErrorBatched)
            auto response = SET({}, RiRi::enableErrorBatched{});
            CHECK(response.ok() == false);
            CHECK(response.code() == RiRi::StatusCode::WARN_ZERO_NODES_PROVIDED);
            CHECK(RiRi::Internal::size() == 100);
            CHECK(response.totalErrorCount() == 0);
            CHECK(response.begin() == response.end());
        }

        // 11
        SUBCASE ("SET multiple keys; unique keys (Batched)") {
            // (span, Batched)
            auto response = SET(unique, RiRi::enableBatched{});
            CHECK(response.ok() == true);
            CHECK(RiRi::Internal::size() == 200);
            CHECK(response.totalEntryCount() == 0);
            CHECK(response.begin() == response.end());
            for (auto &[key, _] : unique_ref) {
                REQUIRE(RiRi::Internal::getValue(key) != nullptr);
                CHECK(*RiRi::Internal::getValue(key) == RiRi::RapidDataType("raresy"));
            }
            CHECK(response.end() - response.begin() == 0);
        }

        // 12
        SUBCASE ("SET multiple keys; duplicate keys (Batched)") {
            // (span, Batched)
            auto response = SET(duplicate, RiRi::enableBatched{});
            CHECK(response.ok() == false);
            CHECK(response.code() == RiRi::StatusCode::ERR_SOME_OPERATIONS_FAILED);
            CHECK(RiRi::Internal::size() == 100);
            CHECK(response.totalEntryCount() == 100);
            CHECK(response.begin() != response.end());
            for (auto &[key, _] : duplicate_ref) {
                REQUIRE(RiRi::Internal::getValue(key) != nullptr);
                CHECK(*RiRi::Internal::getValue(key) != RiRi::RapidDataType("raresy"));
                CHECK(*RiRi::Internal::getValue(key) == RiRi::RapidDataType("RiRi"));
            }
            int i = 0;
            for (auto &[key, code] : response) {
                CHECK(key == duplicate_ref[i++].key);
                REQUIRE(RiRi::Utils::unpack_field_code(&code) != nullptr);
                CHECK(*RiRi::Utils::unpack_field_code(&code) == RiRi::StatusCode::ERR_KEY_ALREADY_EXISTS);
            }
            CHECK(response.end() - response.begin() == 100);
            CHECK(i == 100); // 0...99 + 1

        }

        // 13
        SUBCASE ("SET multiple keys; mixed keys (Batched)") {
            // (span, Batched)
            auto response = SET(mixed, RiRi::enableBatched{});
            CHECK(response.ok() == false);
            CHECK(response.code() == RiRi::StatusCode::ERR_SOME_OPERATIONS_FAILED);
            CHECK(RiRi::Internal::size() == 200);
            CHECK(response.totalEntryCount() == 100);
            CHECK(response.begin() != response.end());
            for (auto &[key, _] : unique_ref) {
                REQUIRE(RiRi::Internal::getValue(key) != nullptr);
                CHECK(*RiRi::Internal::getValue(key) == RiRi::RapidDataType(3.14));
            }
            for (auto &[key, _] : duplicate_ref) {
                REQUIRE(RiRi::Internal::getValue(key) != nullptr);
                CHECK(*RiRi::Internal::getValue(key) == RiRi::RapidDataType("RiRi"));
            }
            int i = 0;
            for (auto &[key, code] : response) {
                CHECK(key == mixed_ref[i++].key);
                REQUIRE(RiRi::Utils::unpack_field_code(&code) != nullptr);
                CHECK(*RiRi::Utils::unpack_field_code(&code) == RiRi::StatusCode::ERR_KEY_ALREADY_EXISTS);
            }
            CHECK(response.end() - response.begin() == 100);
            CHECK(i == 100); // 0...99 + 1
        }

        // 14
        SUBCASE ("SET multiple keys; empty span (Batched)") {
            // (span, Batched)
            auto response = SET({}, RiRi::enableBatched{});
            CHECK(response.ok() == false);
            CHECK(response.code() == RiRi::StatusCode::WARN_ZERO_NODES_PROVIDED);
            CHECK(RiRi::Internal::size() == 100);
            CHECK(response.totalEntryCount() == 0);
            CHECK(response.begin() == response.end());
        }
    }

}