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


struct movable {
    movable() {}
    movable(movable const &) = delete;
    movable(movable &&) {}
    bool operator==(movable const &) const { return true; }
};
static auto const for_movable =
        felspar::testsuite("movable type").test([](auto check) {
            check(movable{}) == movable{};
        });


void thrower() { throw std::runtime_error{"An exception"}; }
auto const throws = felspar::testsuite(
        "throws",
        [](auto check) {
            check([]() {
                thrower();
            }).throws(std::runtime_error{"An exception"});
        },
        [](auto check) {
            check([]() {
                thrower();
            }).template throws_type<std::runtime_error>();
        });
