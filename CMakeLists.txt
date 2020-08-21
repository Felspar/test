cmake_minimum_required(VERSION 3.12)
project(felspar-test)


set(FELSPAR_CHECK_TESTS YES
    CACHE STRING "Turn off to stop tests from being built and run")
if(${FELSPAR_CHECK_TESTS} AND NOT TARGET check)
    enable_testing()
    add_custom_target(check)
endif()


function(add_test_run test_target)
    set(seen-tests OFF)
    set(deps felspar-test-runner)
    foreach(arg ${ARGN})
        if(${seen-tests})
            set(name ${PROJECT_NAME}-${arg}-run)
            add_executable(${name} EXCLUDE_FROM_ALL ${arg})
            target_link_libraries(${name} ${deps})
            add_custom_command(TARGET ${name} POST_BUILD COMMAND ${name})
            add_dependencies(check ${name})
            add_test(NAME ${name}-ctest COMMAND ${name})
        elseif(arg STREQUAL "TESTS")
            set(seen-tests ON)
        else()
            set(deps ${deps} ";" ${arg})
        endif()
    endforeach(arg)
endfunction()


add_subdirectory(src)


if(TARGET check AND ${FELSPAR_CHECK_TESTS})
    set_property(TARGET check PROPERTY EXCLUDE_FROM_ALL TRUE)
    add_subdirectory(test)
endif()
