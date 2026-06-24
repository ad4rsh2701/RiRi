#include "DataManager.h"
#include "doctest.h"
#include "riri/Commands.hpp"
#include "riri/RapidTypes.hpp"
#include "riri/utils/Accessors.hpp"
#include <ostream>

using namespace RiRi::Commands;

// =============================================== LISTS OF SUBCASES ===================================================
// +-------------------------------------------------------------+-----------------------------------------------------+
// |                             SUBCASE                         |                    Overload                         |
// +-------------------------------------------------------------+-----------------------------------------------------+
// | 1.  GET single key; key exists                              | (key) :: (span) :: (span, enableBatched)            |
// | 2.  GET single key; key does not exist                      | (key) :: (span) :: (span, enableBatched)
// | 3.  GET multiple keys; any (span)                           | GET(span)
// | 4.  GET multiple keys; all keys exist                       | GET(span, enableBatched)                            |
// | 5.  GET multiple keys; no keys exist                        | GET(span, enableBatched)                            |
// | 6.  GET multiple keys; some exist                           | GET(span, enableBatched)                            |
// | 7.  GET multiple keys; empty span                           | GET(span, enableBatched)                            |
// +-------------------------------------------------------------+-----------------------------------------------------+


TEST_SUITE("Commands") {
    TEST_CASE("GET") {

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
        SUBCASE("GET single key; key exists") {
            // (key)
            auto response_f = GET("key0");
            CHECK(response_f.ok() == true);
            CHECK(response_f.code() == RiRi::StatusCode::OK);
            REQUIRE(response_f.field() != nullptr);
            CHECK(*response_f.field() == RiRi::RapidDataType("RiRi"));

            // (span)
            RiRi::RapidNode node[] {{"key2"}};
            auto response_n = GET(node);
            CHECK(response_n.ok() == true);
            CHECK(response_n.code() == RiRi::StatusCode::OK);

            // (span, enableBatched)
            auto response_b = GET(node, RiRi::enableBatched{});
            CHECK(response_b.ok() == true);
            CHECK(response_b.code() == RiRi::StatusCode::OK);
            CHECK(response_b.begin() != response_b.end());
            CHECK(response_b.totalEntryCount() == 1);
            for (auto [key, val]: response_b) {
                CHECK(key == node[0].key);
                REQUIRE(RiRi::Utils::unpack_field(&val) != nullptr);
                CHECK(*RiRi::Utils::unpack_field(&val) == RiRi::RapidDataType("RiRi"));
            }
        }

        // 2
        SUBCASE("GET single key; key does not exist") {
            // (key)
            auto response_f = GET("key101");
            CHECK(response_f.ok() == false);
            CHECK(response_f.code() == RiRi::StatusCode::ERR_KEY_NOT_FOUND);
            REQUIRE(response_f.field() == nullptr);

            // (span)
            RiRi::RapidNode node[] {{"key101"}};
            auto response_n = GET(node);
            CHECK(response_n.ok() == false);
            CHECK(response_n.code() == RiRi::StatusCode::ERR_KEY_NOT_FOUND);

            // (span, enableBatched)
            auto response_b = GET(node, RiRi::enableBatched{});
            CHECK(response_b.ok() == false);
            CHECK(response_b.code() == RiRi::StatusCode::ERR_SOME_OPERATIONS_FAILED);
            CHECK(response_b.begin() != response_b.end());
            CHECK(response_b.totalEntryCount() == 1);
            for (auto [key, code]: response_b) {
                CHECK(key == node[0].key);
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

            new_nodes[i].key = std::move("key"+std::to_string(200+i));
            mixed_nodes[i+100].key = std::move("key"+std::to_string(200+i));
        }

        // 3
        SUBCASE("GET multiple keys; any (span)") {
            auto response = GET(mixed_nodes);
            CHECK(response.ok() == false);
            CHECK(response.code() == RiRi::StatusCode::ERR_SINGLE_NODE_EXPECTED);
            REQUIRE(response.field() == nullptr);
                // this is a special case, GET with span will only accept single node spans
                // to get batched, one must explicitly pass those tags, consistent API (not really).
        }

        // 4
        SUBCASE("GET multiple keys; all keys exist") {
            auto response = GET(existing_nodes, RiRi::enableBatched{});
            CHECK(response.ok() == true);
            CHECK(response.code() == RiRi::StatusCode::OK);
            CHECK(response.begin() != response.end());
            CHECK(response.totalEntryCount() == 100);
            int i = 0;
            for (auto [key, val]: response) {
                CHECK(key == existing_nodes[i++].key);
                REQUIRE(RiRi::Utils::unpack_field(&val) != nullptr);
                CHECK(*RiRi::Utils::unpack_field(&val) == RiRi::RapidDataType("RiRi"));
            }
        }

        // 5
        SUBCASE("GET multiple keys; no keys exist") {
            auto response = GET(new_nodes, RiRi::enableBatched{});
            CHECK(response.ok() == false);
            CHECK(response.code() == RiRi::StatusCode::ERR_SOME_OPERATIONS_FAILED);
            CHECK(response.begin() != response.end());
            CHECK(response.totalEntryCount() == 100);
            int i = 0;
            for (auto [key, code]: response) {
                CHECK(key == new_nodes[i++].key);
                REQUIRE(RiRi::Utils::unpack_field_code(&code) != nullptr);
                CHECK(*RiRi::Utils::unpack_field_code(&code) == RiRi::StatusCode::ERR_KEY_NOT_FOUND);
            }
        }

        // 6
        SUBCASE("GET multiple keys; some exist") {
            auto response = GET(mixed_nodes, RiRi::enableBatched{});
            CHECK(response.ok() == false);
            CHECK(response.code() == RiRi::StatusCode::ERR_SOME_OPERATIONS_FAILED);
            CHECK(response.begin() != response.end());
            CHECK(response.totalEntryCount() == 200);
            auto it = response.begin();
            for (int i = 0 ; i < 100 ; i++ ) {
                auto key = it->target;
                auto val = it->result;
                CHECK(key == mixed_nodes[i].key);
                REQUIRE(RiRi::Utils::unpack_field(&val) != nullptr);
                CHECK(*RiRi::Utils::unpack_field(&val) == RiRi::RapidDataType("RiRi"));
                it++;
            }
            for (int i = 100 ; i < 200 ; i++ ) {
                auto key = it->target;
                auto code = it->result;
                CHECK(key == mixed_nodes[i].key);
                REQUIRE(RiRi::Utils::unpack_field_code(&code) != nullptr);
                CHECK(*RiRi::Utils::unpack_field_code(&code) == RiRi::StatusCode::ERR_KEY_NOT_FOUND);
                it++;
            }
            CHECK(it == response.end());
        }

        // 7
        SUBCASE("GET multiple keys; empty span") {
            auto response = GET({}, RiRi::enableBatched{});
            CHECK(response.ok() == false);
            CHECK(response.code() == RiRi::StatusCode::WARN_ZERO_NODES_PROVIDED);
            CHECK(response.totalEntryCount() == 0);
            REQUIRE(response.begin() == response.end());
        }
    }
}
