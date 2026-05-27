#include <riri/Commands.hpp>
#include <DataManager.h>

namespace RiRi::Commands {

    // CLEAR

    Response::Status CLEAR () {
        Internal::clearMap();
        return Response::Status(Response::StatusCode::OK);
    }

} // namespace RiRi::Commands