#pragma once


#include <functional>
#include <span>

#include <felspar/test/check.hpp>


namespace felspar::test {


    using test_function_ptr =
            std::function<void(std::ostream &, test::injected)>;
    using test_check_log_function = void (*)(test::injected, std::ostream &);
    using test_check_function = void (*)(test::injected);
    using test_nullary_function = void (*)(void);


    void register_test(
            std::string_view suite,
            std::string test,
            test_function_ptr,
            source_location const &);
    inline void register_test(
            std::string_view suite,
            std::string test,
            test_check_log_function f,
            source_location const &loc) {
        register_test(
                suite, std::move(test),
                [f](std::ostream &l, test::injected c) { f(c, l); }, loc);
    }
    inline void register_test(
            std::string_view suite,
            std::string test,
            test_check_function f,
            source_location const &loc) {
        register_test(
                suite, std::move(test),
                [f](std::ostream &, test::injected c) { f(c); }, loc);
    }
    inline void register_test(
            std::string_view suite,
            std::string test,
            test_nullary_function f,
            source_location const &loc) {
        register_test(
                suite, std::move(test),
                [f](std::ostream &, test::injected) { f(); }, loc);
    }


    template<typename F>
    concept test_function = requires(
            F f, std::string_view sv, std::string s, source_location loc) {
                                register_test(sv, s, f, loc);
                            };


    template<typename F>
    struct part {
        template<typename V>
        part(V, source_location = source_location::current()) {}
    };
    struct registration {
        std::string_view const suite;

        template<test_function TF>
        auto test(
                char const *name,
                TF t,
                source_location const &loc = source_location::current()) const {
            register_test(suite, name, t, loc);
            return *this;
        }
        template<test_function TF>
        auto test(TF t, source_location const &loc = source_location::current())
                const {
            register_test(suite, {}, t, loc);
            return *this;
        }

        template<test_function... TF>
        auto test(char const *name, TF... ts) const {
            (register_test(suite, name, ts, source_location::current()), ...);
            return *this;
        }
        template<test_function... TF>
        auto test(TF... ts) const {
            (register_test(suite, {}, ts, source_location::current()), ...);
            return *this;
        }
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
