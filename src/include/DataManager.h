#pragma once

#include <string_view>

#include "RapidTypes.h"
#include "RiRiMacros.h"
#include "MemoryMaps.h"

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
 * Or you can use the public API, which is much safer and more stable.
 * 
 * Or you can create a fork of RiRi (here: https://github.com/ad4rsh2701/riri) and modify it as you wish.
 */
namespace RiRi::Internal {
    
    /**
     * @brief ### Insert the key-value pair in the internal memory map.
     * 
     * @param key Type: `std::string_view`
     * @param value Type: `const RapidDataType&`
     * @return `true` or `false`
     * 
     * @note Returns `true` if the key-value pair was successfully inserted, `false` if the key already exists or the insertion failed (very unlikely).
     */
    GO_AWAY bool setValue(std::string&& key, RapidDataType&& value) noexcept;


    /**
     * @brief ### Retrieve the value associated with the key from the internal memory map.
     * 
     * @param key Type: `std::string_view`
     * @return `RapidDataType*` or `nullptr`
     * 
     * @note Returns the value associated with the key if it exists, `nullptr` otherwise.
     */
    GO_AWAY const RapidDataType* getValue(std::string_view key) noexcept;
    
    
    /**
     * @brief ### Delete the key-value pair associated with the given key from the internal memory map.
     * 
     * @param key Type: `std::string_view`
     * @return `true` if the key was found and erased, `false` if the key did not exist.
     */
    GO_AWAY bool deleteKey(std::string_view key) noexcept;


    /**
     * @brief ### Update the value associated with the given key in the internal memory map.
     * 
     * @param key Type: `std::string_view`
     * @param newValue Type: `RapidDataType`
     * @return `true` if the key was found and updated, `false` if the key did not exist.
     */
    GO_AWAY bool updateValue(std::string_view key, RapidDataType&& newValue) noexcept;


    /**
     * @brief ### Retrieve the key associated with the given value from the internal memory map.
     * 
     * @param value Type: `const RapidDataType&`
     * @return `std::string*` containing the key if found, `nullptr` otherwise.
     *
     * @warning This is a `slow linear search`. Only used in rare or non-performance-critical cases.
     */
    GO_AWAY const std::string* getKeyByValue(RapidDataType&& value) noexcept;

    /**
     * @brief ### Clears all entries from the internal memory map.
     * 
     * This operation is guaranteed to succeed and does not throw.
     */
    GO_AWAY void clearMap() noexcept;


    /**
     * @brief ### Returns the size of the internal memory map.
     * 
     * This function is provided to avoid exposing the internal MemoryMap
     * directly in public headers. It serves purely as an access abstraction.
     * 
     * @return `size_t` representing the number of key-value pairs.
     */
    GO_AWAY size_t size() noexcept;
}
