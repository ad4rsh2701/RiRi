#include <ostream>
#include <string_view>
#include <span>

#include "doctest.h"
#include "riri/Commands.hpp"
#include "riri/RapidTypes.hpp"
#include "riri/RapidResponse.hpp"


using namespace RiRi;

TEST_SUITE("Commands System") {

    // idk what the testcase naming convention is, so this is what you get
    TEST_CASE("SetCommand") {
        
        SUBCASE("Single Set command using key-value pair") {
            Response::Status response = Commands::SET("_key-str", "RiRi");

            CHECK(response.ok() == true);
            CHECK(response.code() == RiRi::StatusCode::OK);
        }
        
        SUBCASE("Single Set command using RapidNode") {
            RiRi::RapidNode node{"_key-str", "RiRi"};
            std::span<RapidNode> nodes(&node, 1);

            Response::Status response = Commands::SET(nodes);
            CHECK(response.ok() == true);
            CHECK(response.code() == RiRi::StatusCode::OK);
        }
        
        SUBCASE("Duplicate Key Set command") {
            RiRi::RapidNode node{"_key-str", "RiRi"};
            std::span<RapidNode> nodes(&node, 1);

            Response::Status response = Commands::SET(nodes);

            RiRi::RapidNode node{"_key-str", "RiRi...again?"};
            std::span<RapidNode> nodes(&node, 1);

            Response::Status response = Commands::SET(nodes);
            CHECK(response.ok() == false);
            CHECK(response.code() == RiRi::StatusCode::ERR_KEY_ALREADY_EXISTS);
        }

        SUBCASE("Multiple Set command using RapidNode") {
            std::array<RapidNode, 3> nodeArray = {
                RapidNode{"_key-str1", "RiRi_1"},
                RapidNode{"_key-str2", "RiRi_2"},
                RapidNode{"_key-str3", "RiRi_3"}};

            std::span<RapidNode> nodes(nodeArray);

            Response::Status response = Commands::SET(nodes);

            Response::Status response;
            CHECK(response.ok() == true);
            CHECK(response.code() == RiRi::StatusCode::OK);
        }

        SUBCASE("Duplicate key in a Multiple Set command")
        {
            std::array<RapidNode, 3> nodeArray = {
                RapidNode{"_key-str", "RiRi_1"},
                RapidNode{"_key-str", "RiRi_2"},
                RapidNode{"_key-str-different", "RiRi_3"}};
                
            std::span<RapidNode> nodes(nodeArray);

            Response::Status response = Commands::SET(nodes);

            CHECK(response.ok() == false);
            // Might be a different error code. probably the correct one tho.
            CHECK(response.code() == RiRi::StatusCode::ERR_SOME_OPERATIONS_FAILED);
        }

        // Maybe this is needed, maybe it is not. idk
        SUBCASE("Empty Span opject") {
            std::span<RapidNode> emptySpan;
            Response::Status response = Commands::SET(emptySpan);

            CHECK(response.ok() == false);
            // Maybe the error code should be something else.
            // I just went with what made most sense
            CHECK(response.code() == RiRi::StatusCode::ERR_INVALID_ARGUMENT);
        }
		
    }

}