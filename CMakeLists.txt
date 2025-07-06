cmake_minimum_required(VERSION 3.20)
project(RiRiCore LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Force clang++ as the compiler
set(CMAKE_CXX_COMPILER "clang++")

set(CMAKE_CXX_FLAGS_RELEASE "-O3 -flto -march=native -fuse-ld=lld ")

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

set(PUBLIC_HEADERS
        include/riri/Commands.hpp    # Public API
        include/riri/RiRi.hpp        # Public API
)

set(PRIVATE_HEADERS
        src/include/DataManager.h    # Internal implementation
        src/include/MemoryMaps.h     # Internal implementation
        src/include/RapidTypes.h     # Internal implementation
        src/include/RiRiMacros.h     # Internal implementation
)

set(SEMI_PRIVATE_HEADERS
        src/utils/RapidResponse.hpp
        src/utils/helpers/status_to_string.hpp
)

# Install structure
install(FILES ${PUBLIC_HEADERS}
        DESTINATION include/riri
)

install(FILES ${SEMI_PRIVATE_HEADERS}
        DESTINATION include/riri/utils
)

install(FILES ${PRIVATE_HEADERS}
        DESTINATION include/riri/internal   # GO_AWAY zone
)

add_executable(riri
        src/cli/Main.cpp
        src/core/Commands.cpp
        src/core/MemoryMaps.cpp
        src/core/DataManager.cpp
        # Add more .cpp files here as needed
)

# Define preprocessor macros
target_compile_definitions(riri PRIVATE
        RIRI_DEV_MODE
        RIRI_BUILD_DLL
)

target_include_directories(riri PRIVATE
        ${CMAKE_SOURCE_DIR}/src/include
        ${CMAKE_SOURCE_DIR}/src/utils
        ${CMAKE_SOURCE_DIR}/include/riri/
)

target_compile_options(riri PRIVATE ${CMAKE_CXX_FLAGS_${CMAKE_BUILD_TYPE}})
