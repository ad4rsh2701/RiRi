################################################# UNIT TESTS ###########################################################
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

add_test(NAME RiRiTests COMMAND RiRi_tests)
########################################################################################################################

option(RIRI_LTO_FOR_TESTS "Use LTO for building tests" ON)

##################################################### LTO ##############################################################
if (RIRI_LTO OR RIRI_LTO_FOR_TESTS)
    include(CheckIPOSupported)

    # check for IPO
    check_ipo_supported(RESULT lto_supported)
    if (lto_supported)
        set_property(TARGET RiRi_tests PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
        message(STATUS "RiRi: LTO Enabled for RiRi_tests")
    else()
        message(STATUS "RiRi: LTO not supported")
    endif ()
endif ()
########################################################################################################################
