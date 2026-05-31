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
        for (auto& [key, _]: nodes) {
            if (const bool success = Internal::deleteKey(key); !success) {
                response.setCode(StatusCode::ERR_SOME_OPERATIONS_FAILED);
            }
        }
        return response;
    }

    Response::StatusErrorBatchWith<std::string_view> DELETE (std::span<RapidNode> nodes, enableErrorBatched) {
        Response::StatusErrorBatchWith<std::string_view> response;
        for (auto& [key, _]: nodes) {
            if (const bool success = Internal::deleteKey(key); !success) {
                response.addErrorEntry(key, StatusCode::ERR_KEY_NOT_FOUND);
            }
        }
        return response;
    }

    Response::StatusBatchWith<std::string_view, std::monostate> DELETE (std::span<RapidNode> nodes, enableBatched) {
        Response::StatusBatchWith<std::string_view, std::monostate> response;
        for (auto& [key, _]: nodes) {
            if (const bool success = Internal::deleteKey(key); !success) {
                response.addStatusEntry(key, StatusCode::ERR_KEY_NOT_FOUND);
            }
        }
        return response;
    }

} // namespace RiRi::Commands