// Compiler: The project is built with Clang C++ ONLY.

// Feel free to try compiling with other compilers.

// This header defines macros used throughout RiRi.

#pragma once

// =======================
// Internal-use-only warning
// =======================
#ifndef RIRI_INTERNAL
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
// Force Inlining
// =======================
#define GET_INLINE_PLEASE inline __attribute__((always_inline))