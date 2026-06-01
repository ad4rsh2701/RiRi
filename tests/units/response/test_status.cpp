#include "doctest.h"
#include "riri/RapidResponse.hpp"

using namespace RiRi::Response;

TEST_SUITE("Rapid Response System") {

    TEST_CASE("Status") {

        SUBCASE("Default status is 'ORPHANED'") {
            Status response;
            CHECK(response.code() == RiRi::StatusCode::ORPHANED);
            CHECK(response.ok() == false);
        }

        SUBCASE("Explicit constructor sets '_code'") {
            Status response{RiRi::StatusCode::OK};
            CHECK(response.code() == RiRi::StatusCode::OK);
            CHECK(response.ok() == true);
        }

        SUBCASE("Setter 'setCode' updates '_code'") {
            Status response;

            // setCode()
            response.setCode(RiRi::StatusCode::OK);
            CHECK(response.code() == RiRi::StatusCode::OK);
        }

    }

}