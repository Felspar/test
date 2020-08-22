#pragma once


#include <span>
#include <utility>

#include <felspar/test/check.hpp>


namespace felspar {


    namespace detail {


        using test_function = void (*)(test::injected);

        void register_test(
                std::string_view suite,
                std::string_view test,
                test_function,
                source_location);


        template<typename F>
        struct part {
            template<typename V>
            part(V v, source_location loc = source_location::current()) {}
        };
        struct registration {
            std::string_view const suite;
            auto test(
                    char const *name,
                    test_function t,
                    source_location loc = source_location::current()) const {
                register_test(suite, name, t, loc);
                return *this;
            }
            auto test(
                    test_function t,
                    source_location loc = source_location::current()) const {
                register_test(suite, {}, t, loc);
                return *this;
            }
        };


    }


    template<typename F>
    concept test_function = requires(F f) {
        f(test::injected{});
    };


    template<std::size_t N>
    inline auto testsuite(char const (&n)[N]) {
        return detail::registration{n};
    }
    template<std::size_t N, test_function... Ts>
    inline auto testsuite(char const (&n)[N], Ts &&... tests) {
        detail::registration suite{n};
        (suite.test(tests), ...);
        return suite;
    }


}
