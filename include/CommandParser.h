// RiRi Rapid Dev License (RRDL) v1.0
// © 2025 Adarsh Aryan (Shadow’s Lure)
// Licensed for open use and modification with credit required.
// Full license: /LICENSE or https://github.com/ad4rsh2701/RiRi

#pragma once
#include <string>
#include <vector>
#include "DataStore.h"
#include "ankerl/unordered_dense.h"


// Global pointer to the DataStore instance used by all command functions.
extern DataStore* g_dataStore;

/**
 * @brief Function pointer type used for RiRi command functions.
 * 
 * Each command function takes a list of `string_view` arguments,
 * and returns a result `string` (output or status).
 */
using RiRiCommandFn = std::string(*)(const std::vector<std::string_view>&);

/**
 * @class CommandParser
 * @brief Responsible for parsing and executing commands on the RiRi datastore.
 */
class CommandParser {
private:

    /**
     * @brief Mapping from command name to function pointer.
     * 
     * Supports aliases and function pointer-based fast dispatch.
     * /
     * @note `ankerl:unordered_dense` is used as a drop-in replacement for `std::unordered_map`.
     *        It provides much faster insertion/search and occupies far less memory.
     */
    ankerl::unordered_dense::map<std::string_view, RiRiCommandFn> ririCommandMap;

    /**
     * @brief Parses a raw command string into individual tokens.
     * 
     * @param command Full user command (e.g., "SET key value"), space-delimited.
     * @return Vector of parsed tokens (command + args) of the type `string_view`.
     * @note The returned `string_view`s are safe to use because they point into
     *       the original `command` input, which remains alive during usage in
     *       `executeCommand()`. Tokens are not stored long-term.
     */
    std::vector<std::string_view> parseCommand(std::string_view command) const; // Parse and validate commands

public:
    
    /**
     * @brief Constructs a CommandParser and initializes the command map.
     * 
     * @param ds Pointer to the main RiRi datastore.
     */
    explicit CommandParser(DataStore* ds); // Constructor

    /**
     * @brief Executes a full user command.
     * 
     * Parses and tokenizes the input command string, strips the command name
     * (i.e., the first token), and dispatches the remaining arguments to the
     * corresponding function mapped in `ririCommandMap`.
     * 
     * Example: "SET key7 RiRiIsAwesome" → calls `setCommand({"key7", "RiRiIsAwesome"})`
     * 
     * @param command Full user command (e.g., "SET key value"), space-delimited.
     * @return Result string to be printed or sent back to the client.
     */
    std::string executeCommand(std::string_view command) const; // Execute commands on the DataStore
};


// Respective Command Functions

//SET
/**
 * @brief Sets a key to a value. Also supports bulk key-value pairs.
 * @param args Parsed arguments. Accepts:
 *
 * - Single PAIR: `{ key, value }`
 * 
 * - Bulk PAIR: `{ key1, value1, key2, value2, ... }`
 * @return "OK" if successful, or error message for malformed input.
 * @note The command name (`SET`) is already removed before this function is called.
 *       The args vector only contains the remaining arguments.
 */
std::string setCommand(const std::vector<std::string_view>& args);

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
std::string getCommand(const std::vector<std::string_view>& args);

//UPDATE
/**
 * @brief Updates existing key(s) with new value(s).
 * @param args Parsed arguments. Accepts:
 *        
 * - Single: `{ key, newValue }`
 *        
 * - Bulk:   `{ key1, newVal1, key2, newVal2, ... }`
 * @return "OK" with a count of updated keys, or an error message.
 * @note - Keys must already exist; this does not create new ones.
 * @note - The command name (`UPDATE`) is already removed before this function is called.
 *       The args vector only contains the remaining arguments.
 */
std::string updateCommand(const std::vector<std::string_view>& args);

//DELETE
/**
 * @brief Deletes one or more key-value pairs from the datastore.
 * @param args Parsed arguments. Vector of keys to delete. Accepts:
 *         
 * - Single: `{ key }`
 *        
 * - Bulk: `{ key1, key2, key3, ... }`
 * @return "OK (N deleted)" where N is the number of keys successfully removed.
 * @note - Keys must already exist; there is no blackmagic which deletes non-existing keys.
 * @note - The command name (`DELETE` or `DEL`) is already removed before this function is called.
 *         The args vector only contains the remaining arguments.
 */
std::string deleteCommand(const std::vector<std::string_view>& args);

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
std::string getAllCommand(const std::vector<std::string_view>& args);

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
std::string clearCommand(const std::vector<std::string_view>& args);

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
std::string autoSetCommand(const std::vector<std::string_view>& args);

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
std::string searchCommand(const std::vector<std::string_view>& args);

