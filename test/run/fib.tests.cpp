#include "fib.hpp"
#include <felspar/test.hpp>


static auto const tests =
        felspar::testsuite("fib")
                .test("0", [](auto check) { check(fib(0)) == 0; })
                .test("1", [](auto check) { check(fib(1)) == 1; })
                .test("2", [](auto check) { check(fib(2)) == 1; })
                .test("really really long test name that goes on",
                      [](auto check) { check(fib(3)) == 2; })
                .test("longish test name", [](auto check) {
                    check(fib(4)) == 3;
                    check(fib(5)) == 5;
                });
