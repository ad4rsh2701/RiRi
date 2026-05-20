#pragma once

#include <span>
#include <RiRiMacros.h>
#include <RapidTypes.h>
#include <RapidResponse.hpp>    // That one HEINOUS header file


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
         * @brief Contains all command functions of RiRi
         *
         * This namespace includes functions for setting, getting, updating, deleting keys,
         * retrieving all keys,clearing the store, auto-setting keys, and searching by value.
         */
        namespace Commands {

                ////////////////////////////////////////////////////////////////////////////////////////////////////////

                //SET

                /**
                 * @brief Stores a single key-value pair in the data store without the need for RapidNode creation.
                 *
                 * @note This is a user-friendly implementation requiring no creation of RapidNode prior.
                 * @note This overload of the SET function DOES NOT support key-value pairs.
                 *
                 * @param key a `string` value; can be either copied to or moved into
                 * @param value a `RapidDataType` value; can be either copied to or moved into
                 * @return A `RapidResponse` object containing `StatusCode`
                 */
                Response::Status SET(std::string key, RapidDataType value);

                /**
                 * @brief Stores key-value pairs in the data store (best effort approach).
                 * Supports bulk key-value pairs.
                 * @param nodes a `span` of `RapidNode`: `{ node1, node2, ... }`
                 * @return A `RapidResponse` object containing `StatusCode`
                 * @note To enable verbose response and get per-key diagnostics, pass the
                 * enableFullResponse tag and switch to handling RapidResponseFull object.
                 */
                Response::Status SET (std::span<RapidNode> nodes);

                /**
                 * @brief Stores key-value pairs in the data store (best effort approach).
                 * Supports bulk key-value pairs and provides a verbose response.
                 * @param nodes a `span` of `RapidNode`: `{ node1, node2, ... }`
                 * @return A `RapidResponseFull` object containing `StatusCode` for overall status code, and a buffer
                 * containing ERROR responses as a key-error_code pair: `{ {key1, ERR_}, {key2, ERR_} }`.
                 *
                 * @note RapidResponseFull essentially returns a list of keys that "failed" to get inserted into the map
                 * along with why the specific key failed to insert (per key diagnostics).
                 *
                 * If the overall code is ERR_SOME_OPERATIONS_FAILED, all errors are in the response. However, if the
                 * overall code is ERR_MULTIPLE_OPERATIONS_FAILED, there are more errors than what the response is showing.
                 *
                 * @warning There is a hard error-tracking limit (default 8). If this limit is exceeded, this function will still
                 * attempt to insert the remaining keys, but any further errors will be dropped from the response buffer.
                 */
                Response::StatusErrorBatchWith<std::string_view> SET (std::span<RapidNode> nodes, enableFullResponse);

                ////////////////////////////////////////////////////////////////////////////////////////////////////////

                //GET

                /**
                 * @brief Retrieves the value(s) associated with one or more keys.
                 * @param args Parsed arguments of the type: `span` of `RapidNode`: `{ {key, ""}, {key2, ""}, ... }`
                 * @return The value(s) associated with the key(s), or error messages for missing keys.
                 * @note The command name (`GET`) is already removed before this function is called.
                 *       The args vector only contains the remaining arguments.
                 */
                RIRI_API Response::RapidResponse GET(std::span<RapidNode> args);
                RIRI_API Response::RapidResponseFull GET(std::span<RapidNode> args, enableFullResponse);

                ////////////////////////////////////////////////////////////////////////////////////////////////////////

                //UPDATE

                /**
                 * @brief Updates existing key(s) with new value(s).
                 * @param args args Parsed arguments of the type: `span` of `RapidNode`: `{ {key, newValue}, {key2, newValue2}, ... }`
                 * @return "OK (N keys updated)" where N is the number of keys successfully updated, or an error message.
                 * @note - Keys must already exist; this does not create new ones.
                 * @note - The command name (`UPDATE`) is already removed before this function is called.
                 *       The args vector only contains the remaining arguments.
                 */
                RIRI_API Response::RapidResponse UPDATE(std::span<RapidNode> args);
                RIRI_API Response::RapidResponseFull UPDATE(std::span<RapidNode> args, enableFullResponse);

                ////////////////////////////////////////////////////////////////////////////////////////////////////////

                //DELETE

                /**
                 * @brief Deletes one or more key-value pairs from the datastore.
                 * @param args Parsed arguments of the type: `span` of `RapidNode`: `{ {key, ""}, {key2, ""}, ... }`
                 * @return "OK (N keys deleted)" where N is the number of keys successfully removed.
                 * @note - Keys must already exist; there is no blackmagic that deletes non-existing keys.
                 * @note - The command name (`DELETE` or `DEL`) is already removed before this function is called.
                 *         The args vector only contains the remaining arguments.
                 */
                RIRI_API Response::RapidResponse DELETE(std::span<RapidNode> args);
                RIRI_API Response::RapidResponseFull DELETE(std::span<RapidNode> args, enableFullResponse);

                ////////////////////////////////////////////////////////////////////////////////////////////////////////

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
                RIRI_API Response::RapidResponse GET_ALL(std::span<RapidNode> args);
                RIRI_API Response::RapidResponseFull GET_ALL(std::span<RapidNode> args, enableFullResponse);

                ////////////////////////////////////////////////////////////////////////////////////////////////////////

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
                RIRI_API Response::RapidResponse CLEAR(std::span<RapidNode> args);
                RIRI_API Response::RapidResponseFull CLEAR(std::span<RapidNode> args, enableFullResponse);

                ////////////////////////////////////////////////////////////////////////////////////////////////////////

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
                RIRI_API Response::RapidResponseFull AUTO_SET(std::span<RapidNode> args);
                RIRI_API Response::RapidResponseFull AUTO_SET(std::span<RapidNode> args, enableFullResponse);

                ////////////////////////////////////////////////////////////////////////////////////////////////////////

                // SEARCH

                /**
                 * @brief ## Note: Optimization Remaining
                 * @brief Searches the datastore for all keys associated with a specific value.
                 *
                 * @param args Parsed arguments of the type: `span` of `RapidNode`: `{ {<empty>, value}, {<empty>, value2}, ... }`
                 * @return A list of matching keys, or "key: (not found)".
                 *
                 * @note - This operation performs a full scan of the datastore (O(N)).
                 *         It may be slow with large datasets.
                 * @note - The command name (`SEARCH` or `FIND_BY_VALUE`) is already removed before this function is called.
                 *         The args vector only contains the remaining arguments (values).
                 */
                RIRI_API Response::RapidResponse SEARCH(std::span<RapidNode> args);
                RIRI_API Response::RapidResponseFull SEARCH(std::span<RapidNode> args, enableFullResponse);
        }
} // namespace RiRi::Internal