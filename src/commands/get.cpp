#include "riri/Commands.hpp"
#include "DataManager.h"

namespace RiRi::Commands {

    // GET

    Response::StatusWith<const RapidDataType*> GET (std::string_view key) {
        auto value = Internal::getValue(key);
        return Response::StatusWith (
            value,
            value ? StatusCode::OK : StatusCode::ERR_KEY_NOT_FOUND);
        // I refuse to use my own public helper functions for this case
    }

    Response::StatusWith<const RapidDataType*> GET (std::span<RapidNode> node) {
        Response::StatusWith<const RapidDataType*> response;
        if (node.empty()) {
            response.setCode(StatusCode::WARN_ZERO_NODES_PROVIDED);
            return response;
        }
        if (node.size() != 1) {
            response.setCode(StatusCode::ERR_SINGLE_NODE_EXPECTED);
            // the field stays nullptr (default init) -> wait! they might dereference it!
            // wait nvm, it's a const pointer that doesn't save them from dereferencing a nullptr tho!
            // let's actually test it later.
            // TODO
            return response;
        }
        auto value = Internal::getValue(node[0].key);
        response.fill(
            value,
            value ? StatusCode::OK : StatusCode::ERR_KEY_NOT_FOUND);
        return response;
    }

    Response::StatusBatchWith<std::string_view, const RapidDataType*> GET (std::span<RapidNode> nodes, enableBatched) {
        Response::StatusBatchWith<std::string_view, const RapidDataType *> response;
        if (nodes.empty()) {
            response.setCode(StatusCode::WARN_ZERO_NODES_PROVIDED);
            return response;
        }   // early exit on empty nodes
        for (auto &[key, _]: nodes) { // _ instead of "value" since we don't care about it
            auto val = Internal::getValue(key);
            val ? response.addResultEntry(key, val):
            response.addStatusEntry(key, StatusCode::ERR_KEY_NOT_FOUND);
        }
        return response;
    }

} // namespace RiRi::Commands
