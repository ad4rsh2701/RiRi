# UNIT TESTS

add_executable(RiRi_tests
        test_main.cpp
)

# any headers in the current directory is our lib (doctest.h)
target_include_directories(RiRi_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(RiRi_tests PRIVATE RiRi)

add_test(
        NAME RiRiTests COMMAND RiRi_tests
)