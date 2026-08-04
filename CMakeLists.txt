cmake_minimum_required(VERSION 3.14...3.28)
project(felspar-test LANGUAGES CXX)

include(CheckCXXSymbolExists)

check_cxx_symbol_exists(__cpp_lib_clamp "version" have_clamp)
if(NOT have_clamp)
  message(WARNING "felspar-test requires std::clamp")
endif()

option(FELSPAR_CHECK_TESTS "Turn off to stop tests from being built and run" ON)
if(FELSPAR_CHECK_TESTS AND NOT TARGET felspar-check)
    enable_testing()
    add_custom_target(felspar-check)
    set_property(TARGET felspar-check PROPERTY EXCLUDE_FROM_ALL TRUE)
endif()


# The thread sanitizer symbolizes its reports by forking llvm-symbolizer, which
# on some Linux hosts (seen with LLVM 21) never answers -- the reporting thread
# blocks on the pipe holding a sanitizer lock, so every other thread stops
# behind it and the run reaches the test runner's 30s timeout having printed
# nothing. A race then looks exactly like a deadlock in the code under test.
# The sanitizers speak addr2line's protocol too, and that path does not hang, so
# the report arrives complete with file and line. Set for every test run rather
# than the tsan trees alone: the trees built without it never read TSAN_OPTIONS.
if(CMAKE_HOST_SYSTEM_NAME STREQUAL "Linux")
    find_program(FELSPAR_ADDR2LINE addr2line)
    if(FELSPAR_ADDR2LINE)
        set(FELSPAR_TEST_TSAN_OPTIONS
            "TSAN_OPTIONS=external_symbolizer_path=${FELSPAR_ADDR2LINE}"
            CACHE INTERNAL "Sanitizer settings each test run is made under")
        set(FELSPAR_TEST_RUN_ENV
            ${CMAKE_COMMAND} -E env ${FELSPAR_TEST_TSAN_OPTIONS}
            CACHE INTERNAL "Command prefix putting a test run in that environment")
    endif()
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
            target_include_directories(${name} PRIVATE ${PROJECT_SOURCE_DIR}/include)
            # Run the test as a post-build step. CMAKE_CROSSCOMPILING_EMULATOR
            # is empty for native builds (the test runs directly) and the wine
            # wrapper for the mingw cross build, so cross-built test `.exe`s run
            # on the host. $<TARGET_FILE> resolves the right path (with `.exe` on
            # Windows) in every case. FELSPAR_TEST_RUN_ENV is empty unless the
            # sanitizer symbolizer above needed redirecting.
            add_custom_command(TARGET ${name} POST_BUILD
                COMMAND ${FELSPAR_TEST_RUN_ENV} ${CMAKE_CROSSCOMPILING_EMULATOR} $<TARGET_FILE:${name}>)
            add_dependencies(${test_target} ${name})
            # The same run, reached through ctest rather than the build, needs
            # the same environment.
            add_test(NAME ${name}-ctest COMMAND ${name})
            if(FELSPAR_TEST_TSAN_OPTIONS)
                set_tests_properties(${name}-ctest PROPERTIES
                    ENVIRONMENT "${FELSPAR_TEST_TSAN_OPTIONS}")
            endif()
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
