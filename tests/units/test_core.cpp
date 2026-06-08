#include "doctest.h"
#include "DataManager.h"
#include "riri/utils/Accessors.hpp"
#include "riri/RapidTypes.hpp"
#include <cstdint>
#include <string>

using namespace RiRi::Internal;
using namespace RiRi::Utils;


TEST_CASE("(INTERNAL) Data Manager") {

    clearMap(); // this has to be correct
    REQUIRE(size() == 0); // still we will verify

    SUBCASE("setValue") {
        RiRi::RapidNode node{"_key-str", "RiRi"};
        CHECK(setValue(std::move(node.key), std::move(node.value)) == true);
        CHECK(size() == 1);
    }

    SUBCASE("setValue with duplicate key") {
        RiRi::RapidNode node{"_key-str", "RiRi"};
        CHECK(setValue(std::move(node.key), std::move(node.value)) == true);

        RiRi::RapidNode dup{"_key-str", "RiRi...again?"};
        CHECK(setValue(std::move(dup.key), std::move(dup.value)) == false);
    }

    SUBCASE("setValue with other types") {
        CHECK(setValue("_key-int", std::int64_t{67}) == true);
        CHECK(setValue("_key-double", 3.14) == true);
        CHECK(setValue("_key-bool", false) == true);
        CHECK(size() == 3);
    }

    SUBCASE("getValue") {
        std::string test_value = "RiRi";
        RiRi::RapidNode node{"_key", test_value};
        setValue(std::move(node.key), std::move(node.value));

        REQUIRE(getValue("_key") != nullptr);   // IMP

        CHECK(*unpack_as<std::string>(getValue("_key")) == test_value);
    }

    SUBCASE("getValue of non-existing key") {
        CHECK(getValue("_key") == nullptr);
    }

    SUBCASE("deleteKey") {

        RiRi::RapidNode node{"_key", "RiRi"};
        setValue(std::move(node.key), std::move(node.value));

        CHECK(deleteKey("_key") == true);
        CHECK(getValue("_key") == nullptr);
    }

    SUBCASE("deleteKey of non-existing key") {
        CHECK(deleteKey("_key") == false);
    }

    SUBCASE("updateValue") {
        RiRi::RapidNode node{"_key", "RiRi"};
        setValue(std::move(node.key), std::move(node.value));

        std::string updated_value = "shadowslure";
        REQUIRE(updateValue("_key", updated_value) == true);  // IMP

        CHECK(*unpack_as<std::string>(getValue("_key")) == updated_value);
        CHECK(size() == 1); // replacement, not addition
    }

    SUBCASE("updateValue of non-existing key") {
        CHECK(updateValue("_key", "shadowslure") == false);
    }

    SUBCASE("updateValue of of existing key with same value") {
        RiRi::RapidNode node{"_key", "RiRi"};
        setValue(std::move(node.key), std::move(node.value));

        REQUIRE(updateValue("_key", "RiRi") == true);

        CHECK(*unpack_as<std::string>(getValue("_key")) == "RiRi");
        CHECK(size() == 1);

    }

    SUBCASE("clearMap") {
        setValue("_main_key", RiRi::RapidDataType{"ad4rsh2701"});
        setValue("_alt_key", RiRi::RapidDataType{"ShadowsLure"});

        clearMap();
        CHECK(size() == 0);
    }

    SUBCASE("size") {
        CHECK(size() == 0);

        setValue("_main_key", RiRi::RapidDataType{"ad4rsh2701"});
        CHECK(size() == 1);

        setValue("_alt_key", RiRi::RapidDataType{"ShadowsLure"});
        CHECK(size() == 2);

        deleteKey("_alt_key");
        CHECK(size() == 1);
    }

}