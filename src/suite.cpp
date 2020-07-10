#include <felspar/test/runner.hpp>

#include <vector>


auto &cases() {
    static std::vector<felspar::test_case> c;
    return c;
}


void felspar::detail::register_test(
        std::string_view s,
        std::string_view n,
        test_function f,
        source_location l) {
    cases().push_back(test_case{s, n, f, l});
}


auto felspar::all_test_cases() -> std::span<test_case const> { return cases(); }
