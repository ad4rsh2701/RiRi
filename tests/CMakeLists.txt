# UNIT TESTS

add_executable(RiRi_tests
        test_main.cpp
        units/test_core.cpp
)

target_compile_definitions(RiRi_tests PRIVATE RIRI_INTERNAL)

# any headers in the current directory is our lib (doctest.h)
# and so are the private include dir inside riri too (we need to check internals so)
target_include_directories(RiRi_tests PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/include
)

target_link_libraries(RiRi_tests PRIVATE RiRi)

add_test(
        NAME RiRiTests COMMAND RiRi_tests
)