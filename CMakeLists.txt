cmake_minimum_required(VERSION 3.14...3.28)
project(felspar-test LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)


option(FELSPAR_CHECK_TESTS "Turn off to stop tests from being built and run" ON)
if(FELSPAR_CHECK_TESTS AND NOT TARGET felspar-check)
    enable_testing()
    add_custom_target(felspar-check)
    set_property(TARGET felspar-check PROPERTY EXCLUDE_FROM_ALL TRUE)
endif()


function(add_test_run test_target)
    set(seen-tests OFF)
    set(deps felspar-test-runner)
    set(nprefix ${PROJECT_NAME})
    foreach(arg IN LISTS ARGN)
        if(seen-tests)
            set(name ${nprefix}-${arg}-run)
            add_executable(${name} EXCLUDE_FROM_ALL ${arg})
            target_link_libraries(${name} PRIVATE ${deps})
            add_custom_command(TARGET ${name} POST_BUILD COMMAND ${name})
            add_dependencies(${test_target} ${name})
            add_test(NAME ${name}-ctest COMMAND ${name})
        elseif(arg STREQUAL "TESTS")
            set(seen-tests ON)
        else()
            list(APPEND deps ${arg})
            set(nprefix ${nprefix}-${arg})
        endif()
    endforeach(arg)
endfunction()


add_subdirectory(src)


if(TARGET felspar-check AND FELSPAR_CHECK_TESTS)
    add_subdirectory(test)
endif()
