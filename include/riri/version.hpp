#pragma once // VERSION.HPP

// Phase lifecycle (phase changes always coincide with a MINOR/MAJOR version bump):
//
// | Phase  |       Versions       | Scope                                                |
// |--------|----------------------|------------------------------------------------------|
// | miden  | v0.0.1 to v0.0.x     | Core architecture and API improvement                |
// | alpha  | v0.1.0 to v0.1.x     | Multi-threading and async                            |
// | beta   | v0.2.0 to v0.x.x     | Persistence, API freeze                              |
// | stable | v1.0.0               | First release                                        |

// RIRI VERSION
#define RIRI_VERSION_MAJOR 0
#define RIRI_VERSION_MINOR 0
#define RIRI_VERSION_PATCH 2
#define RIRI_VERSION_PHASE miden

#define RIRI_VERSION_CONCAT_INTERNAL(major, minor, patch, phase) v##major##_##minor##_##patch##_##phase
#define RIRI_VERSION_CONCAT(major, minor, patch, phase) RIRI_VERSION_CONCAT_INTERNAL(major, minor, patch, phase)
#define RIRI_VERSION RIRI_VERSION_CONCAT(RIRI_VERSION_MAJOR, RIRI_VERSION_MINOR, RIRI_VERSION_PATCH, RIRI_VERSION_PHASE)
