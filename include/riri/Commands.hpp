#pragma once

#include <string>
#include <string_view>
#include <span>

#include "../../src/include/RiRiMacros.h"
#include "../../src/include/RapidTypes.h"
#include "../../src/utils/RapidResponse.hpp"


/**
 * @namespace RiRi
 * @brief The public API namespace, holding all RiRi functionalities.
 */
namespace RiRi {

        /**
         * @struct enableFullResponse
         * @brief An empty struct used as a tag to dispatch to command function
         * overloads that return a full diagnostic response.
         */
        struct enableFullResponse{ };

        /**
         * @namespace RiRi::Commands
         * @brief Contains all command functions for the RiRi key-value store.
         *
         * Command functions should be of the type:
         * `RiRi::Error::RiRiResult<std::string_view>(*)(const std::span<RapidNode> args)`.
         *
         * This namespace includes functions for setting, getting, updating, deleting keys, retrieving all keys, clearing the store, auto-setting keys, and searching by value.
         * @note - Each command function is designed to handle a specific operation on the key-value store (map).
         * @note - The command name is expected to be stripped before calling these functions.
         */
        namespace Commands {
                // Respective Command Functions

                //SET
                /**
                 * @brief Sets a key to a value. Also supports bulk key-value pairs.
                 * @param args Parsed arguments of the type: `span` of `RapidNode`: `{ {key, value}, {key2, value2}, ... }`
                 * @return "OK (N keys set)" where N is the number of key-value pairs set, or an error message.
                 * @note The command name (`SET`) is already removed before this function is called.
                 *       The args vector only contains the remaining arguments.
                 */
                RIRI_API Response::RapidResponse setCommand(std::span<RapidNode> args);
                RIRI_API Response::RapidResponseFull setCommand(std::span<RapidNode> args, enableFullResponse);

                //GET
                /**
                 * @brief Retrieves the value(s) associated with one or more keys.
                 * @param args Parsed arguments of the type: `span` of `RapidNode`: `{ {key, ""}, {key2, ""}, ... }`
                 * @return The value(s) associated with the key(s), or error messages for missing keys.
                 * @note The command name (`GET`) is already removed before this function is called.
                 *       The args vector only contains the remaining arguments.
                 */
                RIRI_API Response::RapidResponse getCommand(std::span<RapidNode> args);
                RIRI_API Response::RapidResponseFull getCommand(std::span<RapidNode> args, enableFullResponse);

                //UPDATE
                /**
                 * @brief Updates existing key(s) with new value(s).
                 * @param args args Parsed arguments of the type: `span` of `RapidNode`: `{ {key, newValue}, {key2, newValue2}, ... }`
                 * @return "OK (N keys updated)" where N is the number of keys successfully updated, or an error message.
                 * @note - Keys must already exist; this does not create new ones.
                 * @note - The command name (`UPDATE`) is already removed before this function is called.
                 *       The args vector only contains the remaining arguments.
                 */
                RIRI_API Response::RapidResponse updateCommand(std::span<RapidNode> args);
                RIRI_API Response::RapidResponseFull updateCommand(std::span<RapidNode> args, enableFullResponse);

                //DELETE
                /**
                 * @brief Deletes one or more key-value pairs from the datastore.
                 * @param args Parsed arguments of the type: `span` of `RapidNode`: `{ {key, ""}, {key2, ""}, ... }`
                 * @return "OK (N keys deleted)" where N is the number of keys successfully removed.
                 * @note - Keys must already exist; there is no blackmagic that deletes non-existing keys.
                 * @note - The command name (`DELETE` or `DEL`) is already removed before this function is called.
                 *         The args vector only contains the remaining arguments.
                 */
                RIRI_API Response::RapidResponse deleteCommand(std::span<RapidNode> args);
                RIRI_API Response::RapidResponseFull deleteCommand(std::span<RapidNode> args, enableFullResponse);

                //GET_ALL
                /**
                 * @brief Returns the entire key-value datastore as a formatted string.
                 *
                 * @param args Should be empty and of type: `span` of `RapidNode`: `{{<empty>, ""}, ...}`.
                 * @return A formatted list of key-value pairs, or "EMPTY" if the store is empty.
                 *
                 * @note - Any unexpected arguments will result in an error.
                 * @note - The command name (`GET_ALL` or `DUMP`) is already removed before this function is called.
                 *         The args vector should be empty.
                 */
                RIRI_API Response::RapidResponse getAllCommand(std::span<RapidNode> args);
                RIRI_API Response::RapidResponseFull getAllCommand(std::span<RapidNode> args, enableFullResponse);

                // CLEAR
                /**
                 * @brief Clears all data from the datastore.
                 *
                 * @param args Should be empty and of the type: `span` of `RapidNode`: `{{<empty>, ""}, ...}`.
                 * @return "OK (cleared)" on success, or an error if arguments are passed.
                 * @note - Any unexpected arguments will result in an error.
                 * @note - The command name (`CLEAR` or `CLR`) is already removed before this function is called.
                 *         The args vector should be empty.
                 */
                RIRI_API Response::RapidResponse clearCommand(std::span<RapidNode> args);
                RIRI_API Response::RapidResponseFull clearCommand(std::span<RapidNode> args, enableFullResponse);

                // AUTOSET
                /**
                 * @brief ## Note: Implementation Remaining
                 * @brief Sets values to auto-generated keys using a hash-based scheme.
                 *
                 * @param args Parsed arguments of the type: `span` of `RapidNode`: `{ {<empty>, value}, {<empty>, value2}, ... }`
                 * @return "OK (N auto-keys set)" where N is the number of values processed (expected).
                 *
                 * @note - Ideal for quick data dumps or logging without naming keys manually.
                 * @note - The command name (`AUTOSET` or `HASH_SET`) is already removed before this function is called.
                 *       The args vector only contains the remaining arguments (values).
                 */
                RIRI_API Response::RapidResponseFull autoSetCommand(std::span<RapidNode> args);
                RIRI_API Response::RapidResponseFull autoSetCommand(std::span<RapidNode> args, enableFullResponse);

                // SEARCH
                /**
                 * @brief ## Note: Optimization Remaining
                 * @brief Searches the datastore for all keys associated with a specific value.
                 *
                 * @param args Parsed arguments of the type: `span` of `RapidNode`: `{ {<empty>, value}, {<empty>, value2}, ... }`
                 * @return A list of matching keys, or "key: (not found)".
                 *
                 *
                 * TREEEd
                 *
                 * @note - This operation performs a full scan of the datastore (O(N)).
                 *         It may be slow with large datasets.
                 * @note - The command name (`SEARCH` or `FIND_BY_VALUE`) is already removed before this function is called.
                 *         The args vector only contains the remaining arguments (values).
                 */
                RIRI_API Response::RapidResponse searchCommand(std::span<RapidNode> args);
                RIRI_API Response::RapidResponseFull searchCommand(std::span<RapidNode> args, enableFullResponse);
        }
} // namespace RiRi::Internal