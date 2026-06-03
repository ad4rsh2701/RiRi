# UNIT TESTS

add_executable(RiRi_tests
        test_main.cpp
        units/test_core.cpp
        units/commands/test_set.cpp
        units/commands/test_get.cpp
        units/commands/test_update.cpp
        units/commands/test_delete.cpp
        units/commands/test_clear.cpp
        units/response/test_status.cpp
        units/response/test_status_with.cpp
        units/response/test_status_batch_with.cpp
        units/response/test_status_error_batch_with.cpp
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