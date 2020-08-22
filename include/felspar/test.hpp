#pragma once


#include <span>
#include <utility>

#include <felspar/test/report.hpp>
#include <felspar/test/source.hpp>


namespace felspar {


    template<typename T>
    concept testable_value = std::is_move_constructible_v<T>;


    namespace detail {


        struct injected;

        /// Testing checks
        template<typename V>
        struct checks {
            injected const &check;
            std::remove_cv_t<V> value;
            source_location source;

            template<typename X>
            checks(injected const &c, X &&v, source_location l)
            : check{c}, value{std::forward<X>(v)}, source{std::move(l)} {}

            auto report(bool passed, std::string_view op, std::string_view vr)
                    const {
                return felspar::test::report(
                        passed, op, source, felspar::test::value_string(value),
                        vr);
            }

            /// All supported comparisons
            template<typename C>
            auto operator==(C &&c) const {
                auto cs = felspar::test::value_string(c);
                return report(value == std::forward<C>(c), "==", cs);
            }
            template<typename C>
            auto operator!=(C &&c) const {
                auto cs = felspar::test::value_string(c);
                return report(value != std::forward<C>(c), "!=", cs);
            }

            /// Other supported checks
            template<typename E>
            auto throws(E v) const;
            template<typename E>
            auto throws_type() const {
                try {
                    value();
                    return felspar::test::report(false, "throws", source);
                } catch (E const &) {
                    return felspar::test::report(true, "throws", source);
                }
            }

            auto is_truthy() const {
                felspar::test::report(
                        value ? true : false, "is_truthy", source);
            }
            auto is_falsey() const {
                felspar::test::report(
                        value ? false : true, "is_falsey", source);
            }
        };


        /// The type that is injected into the test
        struct injected {
            template<testable_value V>
            auto operator()(
                    V &&v,
                    source_location loc = source_location::current()) const {
                return checks<V>{*this, std::forward<V>(v), std::move(loc)};
            }
            template<typename L>
            auto throws(L l, source_location loc = source_location::current())
                    const {
                return checks<L>{*this, std::forward<L>(l), std::move(loc)};
            }
        };


        template<typename V>
        template<typename E>
        inline auto checks<V>::throws(E v) const {
            try {
                value();
                return felspar::test::report(false, "throws", source);
            } catch (E const &e) {
                check(e.what()) == std::string_view{v.what()};
                return felspar::test::report(true, "throws", source);
            }
        }


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
