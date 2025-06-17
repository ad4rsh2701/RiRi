#pragma once

#include <string_view>
#include <optional>
#include "RapidTypes.h"
#include "RiRiMacros.h"

/**
 * @brief ### WARNING: INTERNAL ZONE
 * 
 * If you are reading this, then you are in the internal zone of RiRi.
 * And you are 100% going to get an error or break something.
 * 
 * PLEASE DO NOT use internal functions, files, classes, or structs — they're NOT part of the public API.
 * 
 * If you really want to break something, go ahead.
 * But don't say I didn't warn you.
 * 
 * Or you can just use the public API, which is much safer and more stable.
 * 
 * Or you can create a fork of RiRi (here: https://github.com/ad4rsh2701/riri) and modify it as you wish.
 */
namespace RiRi::Internal {
    
    /**
     * @brief Set the Value object
     * 
     * @param key 
     * @param value 
     * @return bool
     */
    GO_AWAY bool setValue(std::string_view key, RapidDataType value);

    GO_AWAY std::optional<RapidDataType> getValue(std::string_view key);

    GO_AWAY bool deleteKey(std::string_view key);

    GO_AWAY bool updateValue(std::string_view key, RapidDataType newValue);

    GO_AWAY std::optional<std::string_view> getKeyByValue(const RapidDataType& value);

    GO_AWAY void clearMap();

    GO_AWAY size_t size();
}
