#include "riri/Commands.hpp"
#include "DataManager.h"

namespace RiRi::Commands {

    // UPDATE

    Response::Status UPDATE (std::string_view key, RapidDataType value) {
        return Response::Status(Internal::updateValue(key, std::move(value))
            ? Response::StatusCode::OK
            : Response::StatusCode::ERR_KEY_NOT_FOUND);
    }

    Response::Status UPDATE (std::span<RapidNode> nodes) {
        Response::Status response;
        for (auto& [key, value]: nodes) {
            if (const bool success = Internal::updateValue(key, std::move(value)); !success) {
                response.setCode(Response::StatusCode::ERR_SOME_OPERATIONS_FAILED);
            }
        }
        return response;
    }

    Response::StatusErrorBatchWith<std::string_view> UPDATE (std::span<RapidNode> nodes, enableErrorBatched) {
        Response::StatusErrorBatchWith<std::string_view> response;
        for (auto& [key, value]: nodes) {
            if (const bool success = Internal::updateValue(key, std::move(value)); !success) {
                // we never move or get rid of the provided key, so we just use it
                response.addErrorEntry(key, Response::StatusCode::ERR_KEY_NOT_FOUND);
            }
        }
        return response;
    }

    Response::StatusBatchWith<std::string_view, std::monostate> UPDATE (std::span<RapidNode> nodes, enableBatched) {
        Response::StatusBatchWith<std::string_view, std::monostate> response;
        for (auto& [key, value]: nodes) {
            if (const bool success = Internal::updateValue(key, std::move(value)); !success) {
                response.addStatusEntry(key, Response::StatusCode::ERR_KEY_NOT_FOUND);
            }
            // we don't need to track success cases, so no need to call `addResultEntry()`
            // though the compiler won't let me do that anyway; the function is constrained
            // for `addResultEntry()` none of the template params must be of type std::monostate
            // that isn't satisfied here
        }
        return response;
    }

} // namespace RiRi::Commands