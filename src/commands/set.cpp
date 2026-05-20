// SET

#include <Commands.hpp>
#include <DataManager.h>

namespace RiRi::Commands {

    Response::Status SET (std::string key, RapidDataType value) {
        return Response::Status(Internal::setValue(std::move(key), std::move(value))
            ? Response::StatusCode::OK
            : Response::StatusCode::ERR_KEY_ALREADY_EXISTS);
        // this is why I added an explicit constructor in RapidResponse class.
        // and no, I am not making it pretty with if-else
    }

    // I swear I don't normally code like the following normally
    // blame clang-tidy

    Response::Status SET (std::span<RapidNode> nodes) {
        Response::Status response;
        for (auto&[key, value]: nodes) {
            if (const bool success = Internal::setValue(std::move(key),std::move(value)); !success) {
                response.setCode(Response::StatusCode::ERR_SOME_OPERATIONS_FAILED);
            }
        }
        return response;
    }

    Response::StatusErrorBatchWith<std::string_view> SET ( std::span<RapidNode> nodes, enableFullResponse) {
        Response::StatusErrorBatchWith<std::string_view> response;
        for (auto& [key, value]: nodes) {
            if (const bool success = Internal::setValue(std::move(key),std::move(value)); !success) {
                // try_implace allows me to do this directly if things go wrong, heh
                response.addErrorEntry(key, Response::StatusCode::ERR_KEY_ALREADY_EXISTS);
            }
        }
        return response;
    }
}
