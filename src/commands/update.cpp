#include "riri/Commands.hpp"
#include "DataManager.h"

namespace RiRi::Commands {

    // UPDATE

    Response::Status UPDATE (std::string_view key, RapidDataType value) {
        return Response::Status(Internal::updateValue(key, std::move(value))
            ? StatusCode::OK
            : StatusCode::ERR_KEY_NOT_FOUND);
    }

    Response::Status UPDATE (std::span<RapidNode> nodes) {
        Response::Status response;

        // I am so sorry.
        if (nodes.empty()) {
            response.setCode(StatusCode::WARN_ZERO_NODES_PROVIDED);
            return response;
        }
        if (nodes.size() == 1) {
            if (!Internal::updateValue(nodes[0].key, std::move(nodes[0].value))) {
                // if update failed
                response.setCode(StatusCode::ERR_KEY_NOT_FOUND);
                return response;
            }
            response.setCode(StatusCode::OK);
            return response;
        }

        response.setCode(StatusCode::OK);   // set default code
        for (auto& [key, value]: nodes) {
            if (!Internal::updateValue(key, std::move(value))) {
                // if update failed
                response.setCode(StatusCode::ERR_SOME_OPERATIONS_FAILED);
            }
        }
        return response;
    }

    Response::StatusErrorBatchWith<std::string_view> UPDATE (std::span<RapidNode> nodes, enableErrorBatched) {
        // the default code is OK (internal implementation)
        Response::StatusErrorBatchWith<std::string_view> response;
        if (nodes.empty()) {
            response.setCode(StatusCode::WARN_ZERO_NODES_PROVIDED);
            return response;
        } // exit early if empty
        for (auto& [key, value]: nodes) {
            if (!Internal::updateValue(key, std::move(value))) {
                // if update failed
                response.addErrorEntry(key, StatusCode::ERR_KEY_NOT_FOUND);
                // we never move or get rid of the provided key, so we just use it
            }
        }
        return response;
    }

    Response::StatusBatchWith<std::string_view, std::monostate> UPDATE (std::span<RapidNode> nodes, enableBatched) {
        Response::StatusBatchWith<std::string_view, std::monostate> response;
        if (nodes.empty()) {
            response.setCode(StatusCode::WARN_ZERO_NODES_PROVIDED);
            return response;
        }
        response.setCode(StatusCode::OK);   // set default overall code
        for (auto& [key, value]: nodes) {
            if (!Internal::updateValue(key, std::move(value))) {
                // if update failed
                response.addStatusEntry(key, StatusCode::ERR_KEY_NOT_FOUND);
            }
            // we don't need to track success cases, so no need to call `addResultEntry()`
            // though the compiler won't let me do that anyway; the function is constrained
            // for `addResultEntry()` none of the template params must be of type std::monostate
            // that isn't satisfied here
        }
        return response;
    }

} // namespace RiRi::Commands