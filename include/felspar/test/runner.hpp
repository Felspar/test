#pragma once


#include <felspar/test/suite.hpp>


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
        std::exception_ptr operator()() const {
            try {
                test(test::injected{});
                return nullptr;
            } catch (...) { return std::current_exception(); }
        }
    };
    std::span<test_case const> all_test_cases();


}
