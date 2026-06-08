#include "riri/Commands.hpp"
#include "DataManager.h"

namespace RiRi::Commands {

    // SET

    Response::Status SET (std::string key, RapidDataType value) {
        return Response::Status(Internal::setValue(std::move(key), std::move(value))
            ? StatusCode::OK
            : StatusCode::ERR_KEY_ALREADY_EXISTS);
        // this is why I added an explicit constructor in RapidResponse class.
        // and no, I am not making it pretty with if-else
    }

    // I swear I don't normally code like the following normally
    // blame clang-tidy

    Response::Status SET (std::span<RapidNode> nodes) {
        Response::Status response;

        // I am so sorry.
        if (nodes.size() == 1) {
            if (!Internal::setValue(std::move(nodes[0].key),std::move(nodes[0].value))) {
                // if insertion failed
                response.setCode(StatusCode::ERR_KEY_ALREADY_EXISTS);
                return response;
            }
            response.setCode(StatusCode::OK);
            return response;
        }

        response.setCode(StatusCode::OK);   // set default code
        for (auto&[key, value]: nodes) {
            if (!Internal::setValue(std::move(key),std::move(value))) {
                // if insertion failed
                response.setCode(StatusCode::ERR_SOME_OPERATIONS_FAILED);
            }
        }
        return response;
    }

    Response::StatusErrorBatchWith<std::string_view> SET (std::span<RapidNode> nodes, enableErrorBatched) {
        // the default code is OK (internal implementation)
        Response::StatusErrorBatchWith<std::string_view> response;
        for (auto& [key, value]: nodes) {
            if (!Internal::setValue(std::move(key),std::move(value))) {
                // if insertion failed

                // try_emplace allows me to do this directly if things go wrong, heh
                response.addErrorEntry(key, StatusCode::ERR_KEY_ALREADY_EXISTS);
            }
        }
        return response;
    }

    Response::StatusBatchWith<std::string_view, std::monostate> SET (std::span<RapidNode> nodes, enableBatched) {
        Response::StatusBatchWith<std::string_view, std::monostate> response;
        response.setCode(StatusCode::OK);   // set default overall code
        for (auto& [key, value]: nodes) {
            if (!Internal::setValue(std::move(key),std::move(value))) {
                // if insertion failed
                response.addStatusEntry(key, StatusCode::ERR_KEY_ALREADY_EXISTS);
            }
            // we don't need to track success cases, so no need to call `addResultEntry()`
            // though the compiler won't let me do that anyway; the function is constrained
            // for `addResultEntry()` none of the template params must be of type std::monostate
            // that isn't satisfied here
        }
        return response;
    }

} // namespace RiRi::Commands
