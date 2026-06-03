#include <ostream>   // Required by doctest to print values
#include <string_view>

#include "doctest.h"
#include "riri/RapidResponse.hpp"
#include "riri/RapidTypes.hpp"

using namespace RiRi::Response;

TEST_SUITE("Rapid Response System") {

    TEST_CASE("StatusWith") {

        SUBCASE("Default status is 'ORPHANED'") {
            StatusWith<const RiRi::RapidDataType*> response;
            CHECK(response.code() == RiRi::StatusCode::ORPHANED);
            CHECK(response.ok() == false);
        }

        SUBCASE("Explicit constructor sets '_field' and '_code'") {
            // <const RapidDataType*>
            RiRi::RapidDataType val = "riri";
            StatusWith<const RiRi::RapidDataType*> response{&val, RiRi::StatusCode::OK};

            REQUIRE(response.field() != nullptr);   // IMP
            CHECK(*response.field() == val);
            CHECK(response.code() == RiRi::StatusCode::OK);

            // <std::string_view>
            StatusWith<std::string_view> response_alt{"_key", RiRi::StatusCode::OK};

                // no UB possible here; string_view comparison does not dereference memory
            CHECK(response_alt.field() == "_key");
            CHECK(response_alt.code() == RiRi::StatusCode::OK);
        }

        SUBCASE("Setter 'fill' sets both '_field' and '_code'") {
            StatusWith<const RiRi::RapidDataType*> response;

            // fill()
            RiRi::RapidDataType val = "riri";
            response.fill(&val, RiRi::StatusCode::OK);
            REQUIRE(response.field() != nullptr);   // IMP
            CHECK(*response.field() == val);
            CHECK(response.code() == RiRi::StatusCode::OK);
        }

        SUBCASE("Setters 'setField' and 'setCode' work standalone") {
            StatusWith<const RiRi::RapidDataType*> response;

            // setCode()
            response.setCode(RiRi::StatusCode::OK);
            CHECK(response.code() == RiRi::StatusCode::OK);

            // setField()
            RiRi::RapidDataType val = "riri";
            response.setField(&val);
            REQUIRE(response.field() != nullptr);   // IMP
            CHECK(*response.field() == val);
        }

    }

}