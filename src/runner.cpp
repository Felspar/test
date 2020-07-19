#include <felspar/test/runner.hpp>

#include <iostream>


int main() {
    auto const tests = felspar::all_test_cases();
    std::size_t number{0};
    for (auto const &test : felspar::all_test_cases()) {
        if (test.name.empty()) {
            std::cout << test.suite << ':' << std::to_string(++number) << '\n';
        } else {
            std::cout << test.suite << ':' << test.name << '\n';
        }
        test();
    }
    return 0;
}
