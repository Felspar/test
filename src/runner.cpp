#include <felspar/test/runner.hpp>

#include <iostream>


int main() {
    auto const tests = felspar::all_test_cases();
    for (auto const &test : felspar::all_test_cases()) {
        std::cout << test.suite << ":" << test.name << '\n';
        test();
    }
    return 0;
}
