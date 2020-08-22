#pragma once


#include <sstream>
#include <string>
#include <string_view>

#include <felspar/test/source.hpp>


namespace felspar::test {


    template<typename T>
    concept stream_printable = requires(T const t, std::stringstream os) {
        {os << t};
    };

    class test_failure : public std::exception {
        std::string message;

      public:
        test_failure(std::string m) : message{std::move(m)} {}

        char const *what() const noexcept { return message.c_str(); }
    };


    inline auto report(bool passed, std::string_view op, source_location s) {
        if (not passed) {
            std::stringstream m;
            m << op << " failed at " << s.file_name() << ":" << s.line() << ":"
              << s.column();
            throw test_failure{m.str()};
        }
    }
    inline auto
            report(bool passed,
                   std::string_view op,
                   source_location s,
                   std::string_view vl,
                   std::string_view vr) {
        if (not passed) {
            std::stringstream m;
            m << op << " failed at " << s.file_name() << ":" << s.line() << ":"
              << s.column() << '\n'
              << vl << ' ' << op << ' ' << vr;
            throw test_failure{m.str()};
        }
    }

    template<typename V>
    inline std::string_view value_string(V const &) {
        return "?? unprintable ??";
    }
    template<stream_printable V>
    inline auto value_string(V const &v) {
        std::stringstream ss{};
        ss << v;
        return ss.str();
    }


}
