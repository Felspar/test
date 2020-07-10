#include <string>
#include <felspar/test.hpp>


static auto const binary =
        felspar::testsuite("checks...binary")
            .test("==", [](auto check) {
                check(1) == 1;
                check(std::string{"hello"}) == "hello";
            })
            .test("!=", [](auto check) {
                check(1) != 2;
                check(std::string{"hello"}) != "world";
            });
