#pragma once


#if __has_include(<experimental/source_location>)


#include <experimental/source_location>


namespace felspar {


    using source_location = std::experimental::source_location;


}


#else


namespace felspar {


    struct source_location {
        /// Can't be `consteval` due to clang bug
        static /*consteval*/ constexpr source_location
                current(char const *f = __builtin_FILE(),
                        char const *fn = __builtin_FUNCTION(),
                        unsigned l = __builtin_LINE(),
                        unsigned c = __builtin_COLUMN()) noexcept {
            return {f, fn, l, c};
        }

        constexpr auto line() const noexcept { return l; }
        constexpr auto column() const noexcept { return c; }
        constexpr auto file_name() const noexcept { return f; }
        constexpr auto function_name() const noexcept { return fn; }

      private:
        constexpr source_location(
                char const *f, char const *fn, unsigned l, unsigned c)
        : f{f}, fn{fn}, l{l}, c{c} {}
        char const *f = {};
        char const *fn = {};
        unsigned l = {};
        unsigned c = {};
    };


}


#endif
