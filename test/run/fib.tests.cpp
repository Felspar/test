#include "fib.hpp"
#include <felspar/test.hpp>


static auto const tests =
        felspar::testsuite("fib")
                .test("0", [](auto check) { check(fib(0)) == 0u; })
                .test("1", [](auto check) { check(fib(1)) == 1u; })
                .test("2", [](auto check) { check(fib(2)) == 1u; })
                .test("really really long test name that goes on",
                      [](auto check) { check(fib(3)) == 2u; })
                .test("longish test name", [](auto check) {
                    check(fib(4)) == 3u;
                    check(fib(5)) == 5u;
                });
