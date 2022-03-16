#pragma once


#include <felspar/test/suite.hpp>

#include <chrono>


namespace felspar::test {


    /**
     * Used by the test runner to fetch tests and run them.
     */
    class test_case {
        friend void register_test(
                std::string_view,
                std::string_view,
                test_function_ptr,
                source_location);

        test_function_ptr test;

        test_case(
                std::string_view s,
                std::string_view n,
                test_function_ptr f,
                source_location)
        : test{f}, suite{s}, name{n} {}

      public:
        std::string_view suite, name;
        std::pair<std::exception_ptr, std::chrono::steady_clock::duration>
                operator()(std::ostream &os) const {
            auto const started = std::chrono::steady_clock::now();
            try {
                test(os, test::injected{});
                return {nullptr, std::chrono::steady_clock::now() - started};
            } catch (...) {
                return {std::current_exception(),
                        std::chrono::steady_clock::now() - started};
            }
        }
    };
    std::span<test_case const> all_test_cases();


}
