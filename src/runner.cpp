#include <felspar/test/runner.hpp>

#include <iostream>


int main() {
    auto const tests = felspar::all_test_cases();
    std::size_t number{}, pass{}, fail{};
    for (auto const &test : felspar::all_test_cases()) {
        ++number;
        if (test.name.empty()) {
            std::cout << test.suite << ':' << std::to_string(++number);
        } else {
            std::cout << test.suite << ':' << test.name;
        }
        if (auto const eptr = test(); not eptr) {
            ++pass;
            std::cout << " ... OK";
        } else {
            ++fail;
            std::cout << " ... FAIL :-(";
            try {
                std::rethrow_exception(eptr);
            } catch (std::exception const &e) {
                std::cerr << '\n' << e.what();
            } catch (...) { std::cerr << "\nUnknown exception type"; }
        };
        std::cout << '\n';
    }
    std::cout << number << " tests run, " << pass << " passed and " << fail
              << " failed\n";
    return std::clamp<std::size_t>(fail, 0, 127);
}
