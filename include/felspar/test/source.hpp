#pragma once


#if __has_include(<experimental/source_location>)


#include <experimental/source_location>


namespace felspar {


    using source_location = std::experimental::source_location;


}


#else


namespace felspar {


    struct source_location {
        static source_location current() { return {}; }

        auto line() const { return l; }
        auto column() const { return c; }
        auto file_name() const { return f; }
        auto function_name() const { return fn; }

    private:
        char const *f = __builtin_FILE();
        char const *fn = __builtin_FUNCTION();
        unsigned l = __builtin_LINE();
        unsigned c = __builtin_COLUMN();
    };


}


#endif
