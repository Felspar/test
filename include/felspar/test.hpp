#pragma once


#include <span>
#include <string>
#include <string_view>
#include <sstream>
#include <utility>

#include <felspar/test/source.hpp>


namespace felspar {


    template<typename T>
    concept testable_value = std::is_move_constructible_v<T>;


    class test_failure : public std::exception {
        std::string message;

      public:
        test_failure(std::string m) : message{std::move(m)} {}

        char const *what() const noexcept { return message.c_str(); }
    };


    namespace detail {


        inline auto report(bool passed, std::string_view op, source_location s) {
            if (not passed) {
                std::stringstream m;
                m << op << " failed at " << s.file_name() << ":" << s.line()
                  << ":" << s.column();
                throw test_failure{m.str()};
            }
        }


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

            template<typename C>
            auto report(bool passed, std::string_view op, C &&) const {
                return detail::report(passed, op, source);
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

            /// Other supported checks
            template<typename E>
            auto throws(E v) const;
            template<typename E>
            auto throws_type() const {
                try {
                    value();
                    return detail::report(false, "throws", source);
                } catch (E const &) {
                    return detail::report(true, "throws", source);
                }
            }

            auto is_truthy() const {
                detail::report(value ? true : false, "is_truthy", source);
            }
            auto is_falsey() const {
                detail::report(value ? false : true, "is_falsey", source);
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
                return detail::report(false, "throws", source);
            } catch (E const &e) {
                check(e.what()) == std::string_view{v.what()};
                return detail::report(true, "throws", source);
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
