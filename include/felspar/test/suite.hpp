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
            std::string_view suite, std::string test, test_function_ptr);
    inline void register_test(
            std::string_view suite,
            std::string test,
            test_check_log_function f) {
        register_test(
                suite, std::move(test),
                [f](std::ostream &l, test::injected c) { f(c, l); });
    }
    inline void register_test(
            std::string_view suite, std::string test, test_check_function f) {
        register_test(
                suite, std::move(test),
                [f](std::ostream &, test::injected c) { f(c); });
    }
    inline void register_test(
            std::string_view suite, std::string test, test_nullary_function f) {
        register_test(
                suite, std::move(test),
                [f](std::ostream &, test::injected) { f(); });
    }


    template<typename F>
    concept test_function = requires(F f, std::string_view sv, std::string s) {
                                register_test(sv, s, f);
                            };


    struct registration {
        std::string_view const suite;

        template<test_function TF>
        auto test(char const *name, TF t) const {
            register_test(suite, name, t);
            return *this;
        }
        template<test_function TF>
        auto test(TF t) const {
            register_test(suite, {}, t);
            return *this;
        }
        template<test_function TF, test_function... TFs>
        auto test(char const *name, TF t, TFs... ts) const {
            rt(name, std::tuple{t, ts...},
               std::make_index_sequence<sizeof...(TFs) + 1>{});
            return *this;
        }
        template<test_function TF, test_function... TFs>
        auto test(TF t, TFs... ts) const {
            register_test(suite, {}, t);
            (register_test(suite, {}, ts), ...);
            return *this;
        }

      private:
        template<test_function... TFs, std::size_t... I>
        void rt(const char *n,
                std::tuple<TFs...> ts,
                std::index_sequence<I...>) const {
            (register_test(
                     suite, std::string{n} + "/" + std::to_string(I + 1),
                     std::get<I>(ts)),
             ...);
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
