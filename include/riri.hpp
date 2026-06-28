#pragma once    // RIRI.HPP

// #################################################### C++23 ####################################################### //

#if __cplusplus < 202002L
#   error "RiRi requires C++23 or higher"
#elif __cplusplus < 202302L
#   warning "C++20 is being used. RiRi currently supports C++20, but future versions will require C++23."
#endif

// ################################################### IMPORTS ###################################################### //

    // CORE
#include "riri/Commands.hpp"
#include "riri/RapidResponse.hpp"

    // UTILS
#include "riri/utils/Accessors.hpp"

    // VERSION
#include "riri/version.hpp"

// ################################################################################################################## //