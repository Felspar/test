## Header only library used to define tests and checks
add_library(felspar-test INTERFACE)
target_include_directories(felspar-test INTERFACE ../include)
target_compile_features(felspar-test INTERFACE cxx_std_20)
install(DIRECTORY ../include/felspar DESTINATION include)


## Library needed to store tests
add_library(felspar-test-suite STATIC suite.cpp)
target_link_libraries(felspar-test-suite felspar-test)


## Library used for the default test runner
add_library(felspar-test-runner STATIC runner.cpp)
target_link_libraries(felspar-test-runner felspar-test-suite)
