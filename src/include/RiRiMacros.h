// Compiler: The project is built with Clang C++.

// This header defines macros used throughout RiRi.
// Must be included before any use of RIRI_API or GO_AWAY.
// Safe to include multiple times (uses #pragma once).

#pragma once

// =======================
// Symbol export for shared libs
// =======================
#ifdef RIRI_BUILD_DLL
  #define RIRI_API __attribute__((visibility("default")))
#else
  #define RIRI_API
#endif

// =======================
// Internal-use-only warning
// =======================
#ifndef RIRI_DEV_MODE
  #define GO_AWAY [[deprecated("You're not supposed to be here. GO AWAY.")]]
#else
  #define GO_AWAY
#endif

// =======================
// Dev-mode assert
// =======================
#ifdef RIRI_DEV_MODE
  #include <cassert>
  #define RIRI_ASSERT(x) assert(x)
#else
  #define RIRI_ASSERT(x)
#endif

// =======================
// Dev-only logging
// =======================
#ifdef RIRI_DEV_MODE
  #include <iostream>
  #define RIRI_DEV_LOG(...) std::cout << "[RiRi Dev] " << __VA_ARGS__ << '\n'
#else
  #define RIRI_DEV_LOG(...)
#endif
