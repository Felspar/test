#include <felspar/test.hpp>

#include <string>
#include <vector>


namespace {


    static auto const unary = felspar::testsuite("checks").test(
            "unary",
            [](auto check) { check(true); },
            [](auto check) {
                auto const *ptr = "";
                check(ptr).is_truthy();
                ptr = nullptr;
                check(ptr).is_falsey();
            });

    static auto const binary =
            felspar::testsuite("checks:binary")
                    .test("==",
                          [](auto check) {
                              check(1) == 1;
                              check(std::string{"hello"}) == "hello";
                          })
                    .test("!=",
                          [](auto check) {
                              check(1) != 2;
                              check(std::string{"hello"}) != "world";
                          })
                    .test("<",
                          [](auto check) {
                              check(1) < 2;
                              check(std::string{"hello"}) < "world";
                          })
                    .test("<=",
                          [](auto check) {
                              check(1) <= 1;
                              check(1) <= 2;
                              check(std::string{"hello"}) <= "hello";
                              check(std::string{"hello"}) <= "world";
                          })
                    .test(">",
                          [](auto check) {
                              check(2) > 1;
                              check(std::string{"world"}) > "hello";
                          })
                    .test(">=", [](auto check) {
                        check(1) >= 1;
                        check(2) >= 2;
                        check(std::string{"world"}) >= "world";
                        check(std::string{"world"}) >= "hello";
                    });


    struct moveonly {
        moveonly() {}
        moveonly(moveonly const &) = delete;
        moveonly(moveonly &&) {}
        bool operator==(moveonly const &) const { return true; }
    };
    static auto const for_movable =
            felspar::testsuite("move only type").test([](auto check) {
                check(moveonly{}) == moveonly{};
            });


    [[noreturn]] void thrower() { throw std::runtime_error{"An exception"}; }
    auto const throws = felspar::testsuite("throws").test(
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


    auto const vector = felspar::testsuite(
            "vector",
            [](auto check) { check(std::vector<int>{}.size()) == 0u; },
            [](auto check) {
                std::vector items = {1, 2, 3, 4};
                check(items.size()) == 4u;
                check(items.front()) == 1;
                check(items[1]) == 2;
                check(items[2]) == 3;
                check(items.back()) == 4;
                check(items.begin()) == items.cbegin();
            },
            [](auto check) {
                std::vector<std::string> ss;
                check([&]() {
                    ss.at(1);
                }).template throws_type<std::out_of_range>();
            });


    auto const overloads = felspar::testsuite(
            "overloads",
            []() { std::string{}; },
            [](auto check) { check(true).is_truthy(); },
            [](auto &os, auto check) {
                os << "Starting test\n";
                check(true).is_truthy();
            });


}
