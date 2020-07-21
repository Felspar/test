#pragma once


#include <span>
#include <string_view>
#include <utility>

#include <felspar/test/source.hpp>


namespace felspar {


    template<typename T>
    concept testable_value = requires(T t) {
        T{t};
    };


    namespace detail {


        /// Testing checks
        template<typename V>
        struct checks {
            V value;
            source_location source;

            template<typename X>
            checks(X &&v, source_location l)
            : value{std::forward<X>(v)}, source{std::move(l)} {}

            /// All supported comparisons
            template<typename C>
            void operator==(C &&c) const {
                bool const result = (value == std::forward<C>(c));
            }
            template<typename C>
            void operator!=(C &&c) const {
                bool const result = (value != std::forward<C>(c));
            }
        };


        /// The type that is injected into the test
        struct injected {
            template<testable_value V>
            auto operator()(
                    V &&v,
                    source_location loc = source_location::current()) const {
                return checks<V>{std::forward<V>(v), std::move(loc)};
            }
            auto operator()(
                    bool const result,
                    source_location loc = source_location::current()) const {}
        };


        using test_function = void (*)(injected);

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
        struct s {
            char const *suite;
            auto
                    test(char const *name,
                         test_function t,
                         source_location loc = source_location::current()) {
                register_test(suite, name, t, loc);
                return *this;
            }
            auto
                    test(test_function t,
                         source_location loc = source_location::current()) {
                register_test(suite, {}, t, loc);
                return *this;
            }
        };


    }


    template<typename F>
    concept test_function = requires(F f) {
        f(detail::injected{});
    };


    template<std::size_t N>
    inline auto testsuite(char const (&n)[N]) {
        return detail::s{n};
    }
    template<std::size_t N, test_function... Ts>
    inline auto testsuite(char const (&n)[N], Ts &&... tests) {
        detail::s suite{n};
        (suite.test(tests), ...);
        return suite;
    }


}
