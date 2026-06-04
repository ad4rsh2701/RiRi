#include <ostream>  // Required by doctest to print values
#include <string_view>

#include "doctest.h"
#include "riri/RapidResponse.hpp"
#include "riri/RapidTypes.hpp"


using namespace RiRi::Response;

TEST_SUITE("Rapid Response System") {

    TEST_CASE("StatusErrorBatchWith") {

        SUBCASE("Default status is OK") {
            StatusErrorBatchWith<const RiRi::RapidDataType*> response;
            CHECK(response.ok() == true);
            CHECK(response.code() == RiRi::StatusCode::OK);
            CHECK(response.totalErrorCount() == 0);
        }

        SUBCASE("Single error entry") {
            // <const RapidDataType*>
            StatusErrorBatchWith<const RiRi::RapidDataType*> response;
            RiRi::RapidDataType val = "riri";
            response.addErrorEntry(&val, RiRi::StatusCode::ERR_UNKNOWN);
            CHECK(response.ok() == false);
            CHECK(response.code() == RiRi::StatusCode::ERR_SOME_OPERATIONS_FAILED);
            CHECK(response.totalErrorCount() == 1);

            // <std::string_view>
            StatusErrorBatchWith<std::string_view> response_alt;
            response_alt.addErrorEntry("_key", RiRi::StatusCode::ERR_UNKNOWN);
            CHECK(response_alt.ok() == false);
            CHECK(response_alt.code() == RiRi::StatusCode::ERR_SOME_OPERATIONS_FAILED);
            CHECK(response_alt.totalErrorCount() == 1);
        }

        SUBCASE("Multiple error entry upto capacity") {
            // <const RapidDataType*>
            StatusErrorBatchWith<const RiRi::RapidDataType*> response;
            RiRi::RapidDataType val = "riri";
            for (int i=0; i < TRACKING_CAPACITY; i++) {
                // idc if all the pointers point to the same obj rn,
                // the entry requires a const pointer to RapidDataType,
                // and I gave that (be it the same), end of discussion.
                response.addErrorEntry(&val, RiRi::StatusCode::ERR_UNKNOWN);
            }
            CHECK(response.ok() == false);
            CHECK(response.code() == RiRi::StatusCode::ERR_SOME_OPERATIONS_FAILED);
            CHECK(response.totalErrorCount() == TRACKING_CAPACITY);

            // <std::string_view>
            StatusErrorBatchWith<std::string_view> response_alt;
            for (int i = 0; i < TRACKING_CAPACITY; i++) {
                // this gets the job done. zip it.
                response_alt.addErrorEntry("_key", RiRi::StatusCode::ERR_UNKNOWN);
            }
            CHECK(response_alt.ok() == false);
            CHECK(response_alt.code() == RiRi::StatusCode::ERR_SOME_OPERATIONS_FAILED);
            CHECK(response_alt.totalErrorCount() == TRACKING_CAPACITY);
        }

        SUBCASE("Multiple error entry beyond capacity") {
            // <const RapidDataType*>
            StatusErrorBatchWith<const RiRi::RapidDataType*> response;
            RiRi::RapidDataType val = "riri";
            for (int i=0; i < TRACKING_CAPACITY + 1; i++) {
                response.addErrorEntry(&val, RiRi::StatusCode::ERR_UNKNOWN);
            }
            CHECK(response.ok() == false);
            CHECK(response.code() == RiRi::StatusCode::ERR_MULTIPLE_OPERATIONS_FAILED);
            CHECK(response.totalErrorCount() == TRACKING_CAPACITY + 1);

            // <std::string_view>
            StatusErrorBatchWith<std::string_view> response_alt;
            for (int i = 0; i < TRACKING_CAPACITY + 1; i++) {
                response_alt.addErrorEntry("_key", RiRi::StatusCode::ERR_UNKNOWN);
            }
            CHECK(response_alt.ok() == false);
            CHECK(response_alt.code() == RiRi::StatusCode::ERR_MULTIPLE_OPERATIONS_FAILED);
            CHECK(response_alt.totalErrorCount() == TRACKING_CAPACITY + 1);
        }

        SUBCASE("Iterators cover only up to capacity") {
            StatusErrorBatchWith<const RiRi::RapidDataType*> response;
            RiRi::RapidDataType val = "riri";
            for (int i=0; i < TRACKING_CAPACITY + 1; i++) {
                response.addErrorEntry(&val, RiRi::StatusCode::ERR_UNKNOWN);
            }
            int count = 0;
            for (const auto& _ : response) count++;
            CHECK(count == TRACKING_CAPACITY);
        }

        SUBCASE("Iterator entries contain correct values") {
            // <const RapidDataType*>
            StatusErrorBatchWith<const RiRi::RapidDataType*> response;
            RiRi::RapidDataType val_one= "riri", val_two = "raresy";

            response.addErrorEntry(&val_one, RiRi::StatusCode::ERR_UNKNOWN);
            response.addErrorEntry(&val_two, RiRi::StatusCode::ERR_KEY_ALREADY_EXISTS);

            auto it = response.begin(); // this can't be nullptr, unless the
                // object is somehow corrupted; then you DO deserve getting seg-faulted

            REQUIRE(it->target != nullptr);  // IMP
            CHECK(*it->target == val_one);
            CHECK(it->code == RiRi::StatusCode::ERR_UNKNOWN);
            ++it;
            REQUIRE(it->target != nullptr);  // IMP
            CHECK(*it->target == val_two);
            CHECK(it->code == RiRi::StatusCode::ERR_KEY_ALREADY_EXISTS);

            // <std::string_view>
            StatusErrorBatchWith<std::string_view> response_alt;
            response_alt.addErrorEntry("_key1", RiRi::StatusCode::ERR_UNKNOWN);
            response_alt.addErrorEntry("_key2", RiRi::StatusCode::ERR_KEY_ALREADY_EXISTS);

            auto it_alt = response_alt.begin();
                // Again, string_view comparison does not dereference memory
            CHECK(it_alt->target == "_key1");
            CHECK(it_alt->code == RiRi::StatusCode::ERR_UNKNOWN);
            ++it_alt;
                // same idea here
            CHECK(it_alt->target == "_key2");
            CHECK(it_alt->code == RiRi::StatusCode::ERR_KEY_ALREADY_EXISTS);
        }


        SUBCASE("Reset executes cleanly") {
            StatusErrorBatchWith<const RiRi::RapidDataType*> response;
            RiRi::RapidDataType val = "riri";
            for (int i=0; i < TRACKING_CAPACITY + 1; i++) {
                response.addErrorEntry(&val, RiRi::StatusCode::ERR_UNKNOWN);
            }
            response.reset();
            CHECK(response.ok() == true);
            CHECK(response.code() == RiRi::StatusCode::OK);
            CHECK(response.totalErrorCount() == 0);
            CHECK(response.begin() == response.end());
        }

    }

}