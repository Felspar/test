cmake_minimum_required(VERSION 3.12)
project(felspar-test)


set(FELSPAR_CHECK_TESTS YES
    CACHE STRING "Turn off to stop tests from being built and run")
if(${FELSPAR_CHECK_TESTS})
    add_custom_target(check)
endif()


function(add_test_run test_target)
    foreach(test_file ${ARGN})
        set(name felspar-test-${test_file}-run)
        message(STATUS "Test run for " ${CMAKE_CURRENT_BINARY_DIR}/${name})
        add_executable(${name} ${PROJECT_SOURCE_DIR}/src/runner.cpp ${test_file})
        target_link_libraries(${name} felspar-test)
        add_custom_command(TARGET ${name} POST_BUILD COMMAND ${name})
        add_dependencies(check ${name})
        add_test(NAME ${name}-ctest COMMAND ${name})
    endforeach(test_file)
endfunction()


add_subdirectory(src)


if(TARGET check AND ${FELSPAR_CHECK_TESTS})
    set_property(TARGET check PROPERTY EXCLUDE_FROM_ALL TRUE)
    add_subdirectory(test)
endif()
