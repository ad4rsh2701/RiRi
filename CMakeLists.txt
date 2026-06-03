cmake_minimum_required(VERSION 3.20)

project(RiRi VERSION 0.0.1 LANGUAGES CXX)

add_library(RiRi STATIC
        src/commands/clear.cpp
        src/commands/delete.cpp
        src/commands/get.cpp
        src/commands/set.cpp
        src/commands/update.cpp
        src/core/DataManager.cpp
        src/core/MemoryMaps.cpp
)

target_compile_features(RiRi PUBLIC cxx_std_23)

# Define RIRI_INTERNAL, because we will be using the internal files to build obv
target_compile_definitions(RiRi PRIVATE RIRI_INTERNAL)

target_include_directories(RiRi
        # PUBLIC: Visible to RiRi and anyone who links against it (FetchContent users)
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>

        # PRIVATE: Visible ONLY internally to RiRi's own source files
        PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/include>
)


if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_compile_options(RiRi PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(RiRi PRIVATE $<$<CONFIG:Release>:-O3>)
endif()

# Apply LTO if supported
include(CheckIPOSupported)
check_ipo_supported(RESULT lto_supported)
if(lto_supported)
    set_property(TARGET RiRi PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()


# UNIT TESTS
option(RIRI_BUILD_TESTS "Build tests for RiRi" OFF)
if(RIRI_BUILD_TESTS)
    message(STATUS "RiRi: Building tests...")
    enable_testing()
    add_subdirectory(tests)
else()
    message(STATUS "RiRi: Skipped building tests!")
endif()