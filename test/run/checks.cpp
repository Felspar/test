#include <felspar/test.hpp>

#include <string>
#include <vector>


namespace {


    static auto const unary = felspar::testsuite(
            "checks...unary",
            [](auto check) { check(true); },
            [](auto check) {
                auto const *ptr = "";
                check(ptr).is_truthy();
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


    auto const vector = felspar::testsuite(
            "vector",
            [](auto check) { check(std::vector<int>{}.size()) == 0; },
            [](auto check) {
                std::vector items = {1, 2, 3, 4};
                check(items.size()) == 4;
                check(items.front()) == 1;
                check(items[1]) == 2;
                check(items[2]) == 3;
                check(items.back()) == 4;
                check(items.begin()) == items.cbegin();
            });


}
