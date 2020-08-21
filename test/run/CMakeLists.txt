add_test_run(check TESTS checks.cpp)

add_library(felspar-test-fib fib.cpp)
add_test_run(check felspar-test-fib TESTS fib.tests.cpp)
