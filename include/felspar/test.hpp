#pragma once


#include <span>
#include <string>
#include <string_view>
#include <sstream>
#include <utility>

#include <felspar/test/source.hpp>


namespace felspar {


    template<typename T>
    concept testable_value = requires(T t) {
        T{std::declval<T>()};
    };


    class test_failure : public std::exception {
        std::string message;

      public:
        test_failure(std::string m) : message{std::move(m)} {}

        char const *what() const noexcept { return message.c_str(); }
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

            template<typename C>
            auto report(bool passed, std::string_view op, C &&c) const {
                if (not passed) {
                    std::stringstream m;
                    m << op << " failed at " << source.file_name() << ":"
                      << source.line() << ":" << source.column();
                    throw test_failure{m.str()};
                }
            }

            /// All supported comparisons
            template<typename C>
            auto operator==(C &&c) const {
                return report(value == std::forward<C>(c), "==", std::move(c));
            }
            template<typename C>
            auto operator!=(C &&c) const {
                return report(value != std::forward<C>(c), "!=", std::move(c));
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
        f(detail::injected{});
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
