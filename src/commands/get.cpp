// GET

#include <Commands.hpp>
#include <DataManager.h>

namespace RiRi::Commands {

    // GET

    Response::StatusWith<const RapidDataType*> GET (std::string key) {
        auto value = Internal::getValue(key);
        return Response::StatusWith (
            value,
            value ? Response::StatusCode::OK : Response::StatusCode::ERR_KEY_NOT_FOUND);
        // I refuse to use my own public helper functions for this case
    }

    Response::StatusWith<const RapidDataType*> GET (std::span<RapidNode> node) {
        Response::StatusWith<const RapidDataType*> response;
        if (node.size() != 1) {
            response.setCode(Response::StatusCode::ERR_SINGLE_NODE_EXPECTED);
            // field stays nullptr (default init)
            return response;
        }
        auto value = Internal::getValue(node[0].key);
        response.fill(
            value,
            value ? Response::StatusCode::OK : Response::StatusCode::ERR_KEY_NOT_FOUND);
        return response;
    }

    Response::StatusBatchWith<std::string_view, const RapidDataType*> GET (std::span<RapidNode> nodes, enableBatched) {
        Response::StatusBatchWith<std::string_view, const RapidDataType *> response;
        for (auto &[key, _]: nodes) { // _ instead of "value" since we don't care about it
            auto val = Internal::getValue(key);
            val ? response.addResultEntry(key, val):
            response.addStatusEntry(key, Response::StatusCode::ERR_KEY_NOT_FOUND);
        }
        return response;
    }
}
