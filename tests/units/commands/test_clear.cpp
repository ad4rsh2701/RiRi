#include "DataManager.h"
#include "doctest.h"
#include "riri/Commands.hpp"
#include "riri/RapidTypes.hpp"
#include <ostream>

using namespace RiRi::Commands;


TEST_SUITE("COMMANDS") {

    TEST_CASE("CLEAR") {

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

        auto resp = CLEAR();
        REQUIRE(RiRi::Internal::size() == 0);
        CHECK(resp.ok() == true);
        CHECK(resp.errorCount() == 0);

        auto resp_again = CLEAR();    // on empty map, clear does nothing
        REQUIRE(RiRi::Internal::size() == 0);
        CHECK(resp_again.ok() == true);
        CHECK(resp_again.errorCount() == 0);
    }

}