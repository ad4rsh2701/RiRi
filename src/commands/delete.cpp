#include "riri/Commands.hpp"
#include "DataManager.h"

namespace RiRi::Commands {

    // DELETE

    Response::Status DELETE (std::string_view key) {
        return Response::Status(Internal::deleteKey(key)
            ? StatusCode::OK
            : StatusCode::ERR_KEY_NOT_FOUND);
    }

    Response::Status DELETE (std::span<RapidNode> nodes) {
        Response::Status response;

        // I am so sorry.
        if (nodes.empty()) {
            response.setCode(StatusCode::WARN_ZERO_NODES_PROVIDED);
            return response;
        }
        if (nodes.size() == 1) {
            if (!Internal::deleteKey(nodes[0].key)) {
                // if deletion failed
                response.setCode(StatusCode::ERR_KEY_NOT_FOUND);
                return response;
            }
            response.setCode(StatusCode::OK);
            return response;
        }

        response.setCode(StatusCode::OK);   // set default code
        for (auto& [key, _]: nodes) {
            if (!Internal::deleteKey(key)) {
                // if deletion failed
                response.setCode(StatusCode::ERR_SOME_OPERATIONS_FAILED);
            }
        }
        return response;
    }

    Response::StatusErrorBatchWith<std::string_view> DELETE (std::span<RapidNode> nodes, enableErrorBatched) {
        // the default code is OK (internal implementation)
        Response::StatusErrorBatchWith<std::string_view> response;
        if (nodes.empty()) {
            response.setCode(StatusCode::WARN_ZERO_NODES_PROVIDED);
            return response;
        }     // exit early if empty
        for (auto& [key, _]: nodes) {
            if (!Internal::deleteKey(key)) {
                // if deletion failed
                response.addErrorEntry(key, StatusCode::ERR_KEY_NOT_FOUND);
            }
        }
        return response;
    }

    Response::StatusBatchWith<std::string_view, std::monostate> DELETE (std::span<RapidNode> nodes, enableBatched) {
        Response::StatusBatchWith<std::string_view, std::monostate> response;
        if (nodes.empty()) {
            response.setCode(StatusCode::WARN_ZERO_NODES_PROVIDED);
            return response;
        }
        response.setCode(StatusCode::OK);   // set default overall code
        for (auto& [key, _]: nodes) {
            if (!Internal::deleteKey(key)) {
                // if deletion failed
                response.addStatusEntry(key, StatusCode::ERR_KEY_NOT_FOUND);
            }
        }
        return response;
    }

} // namespace RiRi::Commands