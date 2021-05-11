add_test_run(check TESTS checks.cpp)

add_library(felspar-test-fib fib.cpp)
target_compile_features(felspar-test-fib PUBLIC cxx_std_20)
add_test_run(check felspar-test-fib TESTS fib.tests.cpp)
