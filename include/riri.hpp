#ifndef RIRI_HPP_INCLUDED
#define RIRI_HPP_INCLUDED

#pragma once    // RIRI.HPP

// #################################################### C++23 ####################################################### //

#if __cplusplus < 202302L
#   error "RiRi requires C++23 or higher"
#endif

// ################################################### IMPORTS ###################################################### //

    // CORE
#include "riri/Commands.hpp"
#include "riri/RapidResponse.hpp"

    // UTILS
#include "riri/utils/Accessors.hpp"

    // VERSION
#include "version.hpp"

// ################################################################################################################## //

#endif // RIRI_HPP_INCLUDED