## Header only library used to define tests and checks
add_library(felspar-test INTERFACE)
target_compile_features(felspar-test INTERFACE cxx_std_20)
target_include_directories(felspar-test INTERFACE ${PROJECT_SOURCE_DIR}/include)
install(DIRECTORY ../include/felspar TYPE INCLUDE)


## Library needed to store tests
add_library(felspar-test-suite STATIC suite.cpp)
target_link_libraries(felspar-test-suite felspar-test)


## Library used for the default test runner
add_library(felspar-test-runner STATIC runner.cpp)
target_link_libraries(felspar-test-runner felspar-test-suite)
if(TARGET felspar-check)
    add_test_run(felspar-check TESTS runner.tests.cpp)
endif()
