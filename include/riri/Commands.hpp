#pragma once

#include <string>
#include <string_view>
#include <span>

#include "src/include/RiRiMacros.h"
#include "src/utils/RapidError.h"


/**
 * @namespace RiRi::Commands
 * @brief Contains all command functions for the RiRi key-value store.
 * 
 * Command functions should be of the type:
 * `RiRi::Error::RiRiResult<std::string_view>(*)(const std::span<std::string_view>)`
 *
 * This namespace includes functions for setting, getting, updating, deleting keys, retrieving all keys, clearing the store, auto-setting keys, and searching by value.
 * @note - Each command function is designed to handle a specific operation on the key-value store (map).
 * @note - The command name is expected to be stripped before calling these functions.
 */
namespace RiRi::Commands {
// Respective Command Functions

    //SET
    /**
     * @brief Sets a key to a value. Also supports bulk key-value pairs.
     * @param args Parsed arguments. Accepts:
     *
     * - Single PAIR: `{ key, value }`
     * 
     * - Bulk PAIR: `{ key1, value1, key2, value2, ... }`
     * @return "OK (N keys set)" where N is the number of key-value pairs set, or an error message.
     * @note The command name (`SET`) is already removed before this function is called.
     *       The args vector only contains the remaining arguments.
     */
    RIRI_API RiRi::Error::RiRiResult<std::string_view> setCommand(const std::span<std::string_view> args);

    //GET
    /**
     * @brief Retrieves the value(s) associated with one or more keys.
     * @param args Parsed arguments (command name removed). Accepts:
     * 
     * - Single KEY: `{ key }`
     * 
     * - Bulk KEYS:   `{ key1, key2, key3, ... }`
     * @return The value(s) associated with the key(s), or error messages for missing keys.
     * @note The command name (`GET`) is already removed before this function is called.
     *       The args vector only contains the remaining arguments.
     */
    RIRI_API RiRi::Error::RiRiResult<std::string_view> getCommand(const std::span<std::string_view> args);

    //UPDATE
    /**
     * @brief Updates existing key(s) with new value(s).
     * @param args Parsed arguments. Accepts:
     *        
     * - Single: `{ key, newValue }`
     *        
     * - Bulk:   `{ key1, newVal1, key2, newVal2, ... }`
     * @return "OK (N keys updated)" where N is the number of keys successfully updated, or an error message.
     * @note - Keys must already exist; this does not create new ones.
     * @note - The command name (`UPDATE`) is already removed before this function is called.
     *       The args vector only contains the remaining arguments.
     */
    RIRI_API RiRi::Error::RiRiResult<std::string_view> updateCommand(const std::span<std::string_view> args);

    //DELETE
    /**
     * @brief Deletes one or more key-value pairs from the datastore.
     * @param args Parsed arguments. Vector of keys to delete. Accepts:
     *         
     * - Single: `{ key }`
     *        
     * - Bulk: `{ key1, key2, key3, ... }`
     * @return "OK (N keys deleted)" where N is the number of keys successfully removed.
     * @note - Keys must already exist; there is no blackmagic which deletes non-existing keys.
     * @note - The command name (`DELETE` or `DEL`) is already removed before this function is called.
     *         The args vector only contains the remaining arguments.
     */
    RIRI_API RiRi::Error::RiRiResult<std::string_view> deleteCommand(const std::span<std::string_view> args);

    //GET_ALL
    /**
     * @brief Returns the entire key-value datastore as a formatted string.
     *
     * @param args Should be empty.
     * @return A formatted list of key-value pairs, or "EMPTY" if the store is empty.
     *
     * @note - Any unexpected arguments will result in an error.
     * @note - The command name (`GET_ALL` or `DUMP`) is already removed before this function is called.
     *         The args vector should be empty.
     */
    RIRI_API RiRi::Error::RiRiResult<std::string_view> getAllCommand(const std::span<std::string_view> args);

    // CLEAR
    /**
     * @brief Clears all data from the datastore.
     *
     * @param args Should be empty.
     * @return "OK (cleared)" on success, or an error if arguments are passed.
     * @note - Any unexpected arguments will result in an error.
     * @note - The command name (`CLEAR` or `CLR`) is already removed before this function is called.
     *         The args vector should be empty.
     */
    RIRI_API RiRi::Error::RiRiResult<std::string_view> clearCommand(const std::span<std::string_view> args);

    // AUTOSET
    /**
     * @brief ## Note: Implementation Remaining
     * @brief Sets values to auto-generated keys using a hash-based scheme.
     *
     * @param args One or more `values`. Each value will be stored under a unique auto-generated key.
     * @return "OK (N auto-keys set)" where N is the number of values processed (expected).
     *
     * @note - Ideal for quick data dumps or logging without naming keys manually.
     * @note - The command name (`AUTOSET` or `HASH_SET`) is already removed before this function is called.
     *       The args vector only contains the remaining arguments (values).
     */
    RIRI_API RiRi::Error::RiRiResult<std::string_view> autoSetCommand(const std::span<std::string_view> args);

    // SEARCH
    /**
     * @brief ## Note: Optimization Remaining
     * @brief Searches the datastore for all keys associated with a specific value.
     *
     * @param args Should contain at least one value to search for. Accepts:
     *         
     * - Single: `{ value }`
     *        
     * - Bulk: `{ val1, val2, val3, ... }`
     * @return A list of matching keys, or "key: (not found)".
     *
     * @note - This operation performs a full scan of the datastore (O(N)).
     *         It may be slow with large datasets.
     * @note - The command name (`SEARCH` or `FIND_BY_VALUE`) is already removed before this function is called.
     *         The args vector only contains the remaining arguments (values).
     */
    RIRI_API RiRi::Error::RiRiResult<std::string_view> searchCommand(const std::span<std::string_view> args);

} // namespace RiRi::Internal