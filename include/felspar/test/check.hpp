#pragma once


#include <type_traits>

#include <felspar/test/report.hpp>
#include <felspar/test/source.hpp>


namespace felspar::test {


    template<typename T>
    concept testable_value = std::is_move_constructible_v<T>;


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
        auto report(bool passed, std::string_view op, C const &c) const {
            if (not passed) {
                throw_failure(source, op, value_string(value), value_string(c));
            }
        }

        /// All supported comparisons
        template<typename C>
        auto operator==(C const &c) const {
            return report(value == c, "==", c);
        }
        template<typename C>
        auto operator!=(C const &c) const {
            return report(value != c, "!=", c);
        }

        /// Other supported checks
        template<typename E>
        auto throws(E v) const;
        template<typename E>
        auto throws_type() const {
            bool passed{false};
            try {
                value();
            } catch (E const &) { passed = true; }
            if (not passed) { throw_failure(source, "throws"); }
        }

        auto is_truthy() const {
            if (not value) {
                throw_failure(source, "is_truthy", value_string(value));
            }
        }
        auto is_falsey() const {
            if (value) {
                throw_failure(source, "is_falsey", value_string(value));
            }
        }
    };


    /// The type that is injected into the test
    struct injected {
        template<testable_value V>
        auto operator()(
                V &&v, source_location loc = source_location::current()) const {
            return checks<V>{*this, std::forward<V>(v), std::move(loc)};
        }
        template<typename L>
        auto
                throws(L l,
                       source_location loc = source_location::current()) const {
            return checks<L>{*this, std::forward<L>(l), std::move(loc)};
        }
    };


    template<typename V>
    template<typename E>
    inline auto checks<V>::throws(E v) const {
        bool passed{false};
        try {
            value();
        } catch (E const &e) {
            check(e.what()) == std::string_view{v.what()};
            passed = true;
        }
        if (not passed) { throw_failure(source, "throws"); }
    }


}
