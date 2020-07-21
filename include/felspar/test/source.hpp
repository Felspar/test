#pragma once


#if __has_include(<experimental/source_location>)


#include <experimental/source_location>


namespace felspar {


    using source_location = std::experimental::source_location;


}


#else


namespace felspar {


    struct source_location {
        static source_location
                current(char const *f = __builtin_FILE(),
                        char const *fn = __builtin_FUNCTION(),
                        unsigned l = __builtin_LINE(),
                        unsigned c = __builtin_COLUMN()) {
            return {f, fn, l, c};
        }

        auto line() const { return l; }
        auto column() const { return c; }
        auto file_name() const { return f; }
        auto function_name() const { return fn; }

      private:
        source_location(char const *f, char const *fn, unsigned l, unsigned c)
        : f{f}, fn{fn}, l{l}, c{c} {}
        char const *f = {};
        char const *fn = {};
        unsigned l = {};
        unsigned c = {};
    };


}


#endif
