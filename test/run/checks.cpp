#include <string>
#include <felspar/test.hpp>


static auto const unary = felspar::testsuite(
        "checks...unary",
        [](auto check) { check(true); },
        [](auto check) {
            auto const *ptr = "";
            check(ptr);
            check(ptr != nullptr);
        });

static auto const binary =
        felspar::testsuite("checks...binary")
                .test("==",
                      [](auto check) {
                          check(1) == 1;
                          check(std::string{"hello"}) == "hello";
                      })
                .test("!=", [](auto check) {
                    check(1) != 2;
                    check(std::string{"hello"}) != "world";
                });
