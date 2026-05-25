#pragma once

#include <span>
#include <RapidTypes.h>
#include <RapidResponse.hpp>    // That one HEINOUS header file


/**
 * @namespace RiRi
 * @brief The public API namespace, holding all RiRi functionalities.
 */
namespace RiRi {

        /**
         * @struct enableErrorBatched
         * @brief An empty struct used as a tag to dispatch to command function
         * overloads that return a `StatusErrorBatchWith<F>` response.
         */
        struct enableErrorBatched{ };

        /**
         * @struct enableBatched
         * @brief An empty struct used as a tag to dispatch to command function
         * overloads that return a `StatusBatchWith<F1, F2>` response.
         */
        struct enableBatched{ };

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
                 * @brief Stores a single key-value pair in the data store without the need for `RapidNode` creation.
                 *
                 * @note This is a user-friendly implementation requiring no creation of RapidNode prior.
                 * @note This overload of the SET function DOES NOT support key-value pairs.
                 *
                 * @param key a `string` value; can be either copied to or moved into
                 * @param value a `RapidDataType` value; can be either copied to or moved into
                 * @return A `Status` object containing `StatusCode`
                 */
                Response::Status SET(std::string key, RapidDataType value);

                /**
                 * @brief Stores key-value pairs in the data store (best effort approach).
                 * Supports bulk key-value pairs.
                 * @param nodes a `span` of `RapidNode`: `{ node1, node2, ... }`
                 * @return A `Status` object containing `StatusCode`
                 * @note To enable verbose response and get per-key diagnostics, pass the
                 * `enableBatched` tag and switch to handling StatusBatchWith<F1, F2> object.
                 * For error-only reporting, pass the `enableErrorBatched` tag and
                 * switch to handling `StatusErrorBatchWith<F>
                 */
                Response::Status SET (std::span<RapidNode> nodes);


                /**
                 * @brief Stores key-value pairs in the data store (best effort approach and limited diagnostics).
                 * Supports bulk key-value pairs and provides a capped diagnostic response.
                 *
                 * @param nodes a `span` of `RapidNode`: `{ node1, node2, ... }`
                 *
                 * @return A `StatusErrorBatchWith<std::string>` object containing `StatusCode` for overall status code,
                 * and a capped buffer containing ERROR responses as a key-error_code pair
                 * E.g. `{ {key1, ERR_}, {key2, ERR_}, ... }`
                 *
                 * The maximum number of errors tracked is defined at compile time by the TRACKING_CAPACITY.
                 *
                 * @note `StatusErrorBatchWith` essentially returns a list of keys that "failed" to get inserted into
                 * the map along with why the specific key failed to insert (per failed key diagnostics).
                 *
                 * If the overall code is ERR_SOME_OPERATIONS_FAILED, all errors are in the response. However, if the
                 * overall code is ERR_MULTIPLE_OPERATIONS_FAILED, there are more errors than what the response is showing.
                 *
                 * @warning There is a hard error-tracking limit (default 8). If this limit is exceeded, this function will still
                 * attempt to insert the remaining keys, but any further errors will be dropped from the response buffer.
                 */
                Response::StatusErrorBatchWith<std::string_view> SET (std::span<RapidNode> nodes, enableErrorBatched);


                /**
                 * @brief Stores key-value pairs in the data store (the best effort and full diagnostic)
                 * Supports bulk key-value pairs and provides a detailed diagnostic response.
                 *
                 * @param nodes a `span` of `RapidNodes`: `{ node1, node2, ... }`
                 *
                 * @return A `StatusBatchWith<F1, F2>` object containing `StatusCode` for overall status code,
                 * and a buffer containing ERROR responses as a key-error_code pair
                 * E.g. `{ {key1, ERR_}, {key2, ERR_}, ... }`.
                 *
                 * There is no limit to the number of errors tracked.
                 *
                 * @note This function explicitly returns an object containing ALL the ERROR responses, regardless
                 * of the size of the bulk input. It's basically an uncapped version of SET's `StatusErrorBatchWith<F>`
                 * overload. Error responses in the response represent which keys failed to get inserted and why.
                 *
                 * @warning This function is NOT RECOMMENDED for VERY LARGE BULK cases (unless you have unlimited RAM
                 * and time), or getting per key diagnostics is more important than speed. Prefer the overloaded
                 * function accessible by passing the `enableErrorBatched` tag (return type: `StatusErrorBatchWith<F>`)
                 * for a lighter response object.
                 */
                Response::StatusBatchWith <std::string_view, std::monostate> SET (std::span<RapidNode> nodes, enableBatched);
                // For SET, we really only need string_view and status code pairs, so there's no need of result_field,
                // so we set it to std::monostate, and we only call `addStatusEntry` and never `addResultEntry` (because
                // my API won't let you do so, the function is constrained).

                ////////////////////////////////////////////////////////////////////////////////////////////////////////

                //GET

                /**
                 * @brief Retrieves the value associated with one key from the data store
                 * Doesn't require prior construction of `RapidNode`.
                 *
                 * @param key of the type: std::string
                 *
                 * @return A single value associated with the key and appropriate status code; inside a
                 * `StatusWith` response object
                 */
                Response::StatusWith<const RapidDataType*> GET(std::string_view key);

                /**
                 * @brief Retrieves the value associated with one key from the data store
                 *
                 * @param node of type: `span` of a single `RapidNode`: `{ {key, ''} }`
                 *
                 * @return A single value associated with the key and appropriate status code; inside a
                 * `StatusWith` response object.
                 *
                 * @warning Only works for spans containing single nodes!
                 * If multiple nodes are passed, no values are ever fetched; instead,
                 * a response object, containing a status code (`ERR_SINGLE_NODE_EXPECTED`)
                 * and an empty field value (`nullptr`), is returned.
                 */
                Response::StatusWith<const RapidDataType*> GET(std::span<RapidNode> node);

                /**
                 * @brief Retrieves values for multiple keys from the data stores; Best Effort approach
                 *
                 * @param nodes of tpe: `span` of `RapidNode`s: `{ {key, ''}, {key, ''}, ... }`
                 *
                 * @return A `StatusBatchWith<string_viw, const RapidDataType*>` object containing either values
                 * for each key or status code. Additionally, provides an overall status code.
                 */
                Response::StatusBatchWith<std::string_view, const RapidDataType*> GET(std::span<RapidNode> nodes, enableBatched);

                ////////////////////////////////////////////////////////////////////////////////////////////////////////

                //UPDATE

                /**
                 * @brief Updates/replaces existing value, associated with a key, with a new provided value without the
                 * need for `RapidNode` creation. Only for a single key update.
                 *
                 * @param key a `string` value; can be either copied to or moved into
                 * @param value a `RapidDataType` value; can be either copied to or moved into
                 *
                 * @return A `Status` object containing `StatusCode`
                 */
                Response::Status UPDATE(std::string_view key, RapidDataType value);

                /**
                 * @brief Updates/replaces existing values, each associated with a key, with new provided values
                 * Best effort approach and supports bulk updates.
                 *
                 * @param nodes a `span` of `RapidNode`: `{ node1, node2, ... }`
                 *
                 * @return A `Status` object containing `StatusCode`
                 */
                Response::Status UPDATE(std::span<RapidNode> nodes);

                /**
                 * @brief Updates/replaces existing values, each associated with a key, with new values
                 * Best effort approach and supports bulk updates. c
                 *
                 * @param nodes a `span` of `RapidNode`: `{ node1, node2, ... }`
                 *
                 * @return A `StatusErrorBatchWith<std::string>` object containing `StatusCode` for overall
                 * status code, and a capped buffer containing ERROR responses as a key-error_code pair
                 * E.g. `{ {key1, ERR_}, {key2, ERR_}, ... }`
                 *
                 * The maximum number of errors tracked is defined at compile time by the TRACKING_CAPACITY.
                 *
                 * @note `StatusErrorBatchWith` essentially returns a list of keys that "failed" to get updated in
                 * the map along with why the specific key failed to insert (per failed key diagnostics).
                 *
                 * If the overall code is ERR_SOME_OPERATIONS_FAILED, all errors are in the response. However, if the
                 * overall code is ERR_MULTIPLE_OPERATIONS_FAILED, there are more errors than what the response is showing.
                 *
                 * @warning There is a hard error-tracking limit (default 8). If this limit is exceeded, this function will still
                 * attempt to update the remaining keys, but any further errors will be dropped from the response buffer.
                 */
                Response::StatusErrorBatchWith<std::string_view> UPDATE (std::span<RapidNode> nodes, enableErrorBatched);

                /**
                 * @brief Updates/replaces existing values, each associated with a key, with new values
                 * Best effort approach and supports bulk updates. Provides a detailed diagnostic response.
                 *
                 * @param nodes a `span` of `RapidNode`: `{ node1, node2, ... }`
                 *
                 * @return A `StatusBatchWith<F1, F2>` object containing `StatusCode` for overall status code,
                 * and a buffer containing ERROR responses as a key-error_code pair
                 * E.g. `{ {key1, ERR_}, {key2, ERR_}, ... }`.
                 *
                 * There is no limit to the number of errors tracked.
                 *
                 * @note This function explicitly returns an object containing ALL the ERROR responses, regardless
                 * of the size of the bulk input. It's an uncapped version of UPDATE's `StatusErrorBatchWith<F>`
                 * overload. Error responses in the response represent which keys failed to get updated and why.
                 *
                 * @warning This function is NOT RECOMMENDED for VERY LARGE BULK cases (unless you have unlimited RAM
                 * and time), or getting per key diagnostics is more important than speed. Prefer the overloaded
                 * function accessible by passing the `enableErrorBatched` tag (return type: `StatusErrorBatchWith<F>`)
                 * for a lighter response object.
                 */
                Response::StatusBatchWith<std::string_view, std::monostate> UPDATE (std::span<RapidNode> nodes, enableBatched);

                ////////////////////////////////////////////////////////////////////////////////////////////////////////

                //DELETE

                /**
                 * @brief Deletes/removes a key-value pair from the data store without the
                 * need for `RapidNode` creation. Only for single key-value pair deletion
                 *
                 * @param key a `string` value; can be either copied to or moved into
                 *
                 * @return A `Status` object containing `StatusCode`
                 */
                Response::Status DELETE(std::string_view key);

                /**
                 * @brief Deletes/removes key-value pairs from the data store
                 * Best effort approach and supports bulk deletion.
                 *
                 * @param nodes a `span` of `RapidNode`: `{ node1, node2, ... }`
                 *
                 * @return A `Status` object containing `StatusCode`
                 */
                Response::Status DELETE(std::span<RapidNode> nodes);

                /**
                 * @brief Deletes/removes key-value pairs from the data store
                 * Best effort approach and supports bulk deletion. Provides a capped diagnostic response.
                 *
                 * @param nodes a `span` of `RapidNode`: `{ node1, node2, ... }`
                 *
                 * @return  A `StatusErrorBatchWith<std::string>` object containing `StatusCode` for overall
                 * status code, and a capped buffer containing ERROR responses as a key-error_code pair
                 * E.g. `{ {key1, ERR_}, {key2, ERR_}, ... }`
                 *
                 * The maximum number of errors tracked is defined at compile time by the TRACKING_CAPACITY.
                 *
                 * @note `StatusErrorBatchWith` essentially returns a list of keys that "failed" to get deleted from
                 * the map along with why the specific key failed to get deleted (per failed key diagnostics).
                 *
                 * If the overall code is ERR_SOME_OPERATIONS_FAILED, all errors are in the response. However, if the
                 * overall code is ERR_MULTIPLE_OPERATIONS_FAILED, there are more errors than what the response is showing.
                 *
                 * @warning There is a hard error-tracking limit (default 8). If this limit is exceeded, this function will still
                 * attempt to delete the remaining keys, but any further errors will be dropped from the response buffer.
                 */
                Response::StatusErrorBatchWith<std::string_view> DELETE (std::span<RapidNode> nodes, enableErrorBatched);

                /**
                 * @brief Deletes/removes key-value pairs from the data store
                 * Best effort approach and supports bulk deletion. Provides a detailed diagnostic response.
                 *
                 * @param nodes a `span` of `RapidNode`: `{ node1, node2, ... }`
                 *
                 * @return A `StatusBatchWith<F1, F2>` object containing `StatusCode` for overall status code,
                 * and a buffer containing ERROR responses as a key-error_code pair
                 * E.g. `{ {key1, ERR_}, {key2, ERR_}, ... }`.
                 *
                 * There is no limit to the number of errors tracked.
                 *
                 * @note This function explicitly returns an object containing ALL the ERROR responses, regardless
                 * of the size of the bulk input. It's an uncapped version of DELETE's `StatusErrorBatchWith<F>`
                 * overload. Error responses in the response represent which keys failed to get deleted and why.
                 *
                 * @warning This function is NOT RECOMMENDED for VERY LARGE BULK cases (unless you have unlimited RAM
                 * and time), or getting per key diagnostics is more important than speed. Prefer the overloaded
                 * function accessible by passing the `enableErrorBatched` tag (return type: `StatusErrorBatchWith<F>`)
                 * for a lighter response object.
                 */
                Response::StatusBatchWith<std::string_view, std::monostate> DELETE (std::span<RapidNode> nodes, enableBatched);

                ////////////////////////////////////////////////////////////////////////////////////////////////////////

                // CLEAR

                /**
                 * @brief Clears the entire data store (drops all stored key-values).
                 * @return A `Status` object containing `StatusCode`
                 */
                Response::Status CLEAR();

                ////////////////////////////////////////////////////////////////////////////////////////////////////////

                // //GET_ALL
                //
                // /**
                //  * @brief Returns the entire key-value datastore as a formatted string.
                //  *
                //  * @param args Should be empty and of type: `span` of `RapidNode`: `{{<empty>, ""}, ...}`.
                //  * @return A formatted list of key-value pairs, or "EMPTY" if the store is empty.
                //  *
                //  * @note - Any unexpected arguments will result in an error.
                //  * @note - The command name (`GET_ALL` or `DUMP`) is already removed before this function is called.
                //  *         The args vector should be empty.
                //  */
                // RIRI_API Response::RapidResponse GET_ALL(std::span<RapidNode> args);
                // RIRI_API Response::RapidResponseFull GET_ALL(std::span<RapidNode> args, enableFullResponse);

                ////////////////////////////////////////////////////////////////////////////////////////////////////////

                // // AUTOSET
                //
                // /**
                //  * @brief ## Note: Implementation Remaining
                //  * @brief Sets values to auto-generated keys using a hash-based scheme.
                //  *
                //  * @param args Parsed arguments of the type: `span` of `RapidNode`: `{ {<empty>, value}, {<empty>, value2}, ... }`
                //  * @return "OK (N auto-keys set)" where N is the number of values processed (expected).
                //  *
                //  * @note - Ideal for quick data dumps or logging without naming keys manually.
                //  * @note - The command name (`AUTOSET` or `HASH_SET`) is already removed before this function is called.
                //  *       The args vector only contains the remaining arguments (values).
                //  */
                // RIRI_API Response::RapidResponseFull AUTO_SET(std::span<RapidNode> args);
                // RIRI_API Response::RapidResponseFull AUTO_SET(std::span<RapidNode> args, enableFullResponse);

                ////////////////////////////////////////////////////////////////////////////////////////////////////////

                // // SEARCH
                //
                // /**
                //  * @brief ## Note: Optimization Remaining
                //  * @brief Searches the datastore for all keys associated with a specific value.
                //  *
                //  * @param args Parsed arguments of the type: `span` of `RapidNode`: `{ {<empty>, value}, {<empty>, value2}, ... }`
                //  * @return A list of matching keys, or "key: (not found)".
                //  *
                //  * @note - This operation performs a full scan of the datastore (O(N)).
                //  *         It may be slow with large datasets.
                //  * @note - The command name (`SEARCH` or `FIND_BY_VALUE`) is already removed before this function is called.
                //  *         The args vector only contains the remaining arguments (values).
                //  */
                // RIRI_API Response::RapidResponse SEARCH(std::span<RapidNode> args);
                // RIRI_API Response::RapidResponseFull SEARCH(std::span<RapidNode> args, enableFullResponse);

        } // namespace Commands

} // namespace RiRi