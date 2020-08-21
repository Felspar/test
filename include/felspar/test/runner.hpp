#pragma once


#include <felspar/test.hpp>


namespace felspar {


    /*
     * Used by the test runner to fetch tests and run them.
     */
    class test_case {
        friend void detail::register_test(
                std::string_view,
                std::string_view,
                detail::test_function,
                source_location);

        detail::test_function test;

        test_case(
                std::string_view s,
                std::string_view n,
                detail::test_function f,
                source_location)
        : test{f}, suite{s}, name{n} {}

      public:
        std::string_view suite, name;
        std::exception_ptr operator()() const {
            try {
                test(detail::injected{});
                return nullptr;
            } catch (...) { return std::current_exception(); }
        }
    };
    std::span<test_case const> all_test_cases();


}
