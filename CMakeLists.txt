# Specify the C++ version you want to use
cmake_minimum_required(VERSION 3.10)

# Project name
project(RiRi)

# Specify the C++ standard (you can change this version if needed)
set(CMAKE_CXX_STANDARD 17)

# Add include directories for header files
include_directories(${PROJECT_SOURCE_DIR}/include)

# Add all your source files in src folder
set(SOURCES
    src/main.cpp
    src/RiRiServer.cpp
    src/NetworkManager.cpp
    src/CommandParser.cpp
    src/DataStore.cpp
    src/PersistenceEngine.cpp
    # add any other files here
)

# Create the executable
add_executable(RiRi ${SOURCES})

# Link any necessary libraries (if required)
# For example, if you need to link against the pthread library, you can add:
# target_link_libraries(RiRi pthread)
