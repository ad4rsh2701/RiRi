#pragma once

// Internal use-only warning macro
#ifndef RIRI_INTERNAL
    #define GO_AWAY [[deprecated("You're not supposed to be here, GO AWAY!")]]
#else
  #define GO_AWAY
#endif // RIRI_INTERNAL