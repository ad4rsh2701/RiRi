#pragma once // VERSION.HPP


// RIRI VERSION
#define RIRI_VERSION_MAJOR 0
#define RIRI_VERSION_MINOR 0
#define RIRI_VERSION_PATCH 1
#define RIRI_VERSION_PHASE miden

#define RIRI_VERSION_CONCAT_INTERNAL(major, minor, patch, phase) v##major##_##minor##_##patch##_##phase
#define RIRI_VERSION_CONCAT(major, minor, patch, phase) RIRI_VERSION_CONCAT_INTERNAL(major, minor, patch, phase)
#define RIRI_VERSION RIRI_VERSION_CONCAT(RIRI_VERSION_MAJOR, RIRI_VERSION_MINOR, RIRI_VERSION_PATCH, RIRI_VERSION_PHASE)

// THE NAMESPACE
#define RIRI_NAMESPACE_BEGIN namespace RiRi::inline RIRI_VERSION {
#define RIRI_NAMESPACE_END }

