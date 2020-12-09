#pragma once


#include <span>

#include <felspar/test/check.hpp>


namespace felspar::test {


    using test_function_ptr = void (*)(test::injected);

    void register_test(
            std::string_view suite,
            std::string_view test,
            test_function_ptr,
            source_location);


    template<typename F>
    struct part {
        template<typename V>
        part(V, source_location = source_location::current()) {}
    };
    struct registration {
        std::string_view const suite;
        auto
                test(char const *name,
                     test_function_ptr t,
                     source_location loc = source_location::current()) const {
            register_test(suite, name, t, loc);
            return *this;
        }
        auto
                test(test_function_ptr t,
                     source_location loc = source_location::current()) const {
            register_test(suite, {}, t, loc);
            return *this;
        }
    };


    template<typename F>
    concept test_function = requires(F f) {
        test_function_ptr{f};
        f(test::injected{});
    };


    template<std::size_t N>
    inline auto testsuite(char const (&n)[N]) {
        return registration{n};
    }
    template<std::size_t N, test_function... Ts>
    inline auto testsuite(char const (&n)[N], Ts &&...tests) {
        registration suite{n};
        (suite.test(tests), ...);
        return suite;
    }

}
