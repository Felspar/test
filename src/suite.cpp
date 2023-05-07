#include <felspar/test/runner.hpp>

#include <vector>


auto &cases() {
    static std::vector<felspar::test::test_case> c;
    return c;
}


void felspar::test::register_test(
        std::string_view s,
        std::string n,
        test_function_ptr f,
        source_location const &l) {
    cases().push_back(test::test_case{s, std::move(n), f, l});
}


auto felspar::test::all_test_cases() -> std::span<test_case const> {
    auto const &c{cases()};
    return {c.data(), c.size()};
}
