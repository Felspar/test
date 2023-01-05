#pragma once


#include <sstream>
#include <string>
#include <string_view>

#include <felspar/test/source.hpp>


namespace felspar::test {


    /**
     * Generate a test failure report which will be used as the constructor
     * message for the thrown exception. This function needs to be implemented
     * by the test runner.
     */
    std::string format_failure_message(
            std::string_view filename,
            std::size_t line,
            std::size_t column,
            std::string_view op,
            std::string_view first = std::string_view{},
            std::string_view second = std::string_view{});


    /**
     * Concepts used for deciding how to print values. Right now only support
     * `ostream`.
     */
    template<typename T>
    concept stream_printable = requires(T const t, std::stringstream os) {
                                   { os << t };
                               };

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


    /// The exception that is thrown in response to a test failure.
    class test_failure : public std::exception {
        std::string message;

      public:
        test_failure(std::string m) : message{std::move(m)} {}

        char const *what() const noexcept { return message.c_str(); }
    };


    /// Default reporting which also throws the exception.
    [[noreturn]] inline auto throw_failure(
            source_location s,
            std::string_view op,
            std::string_view vl = std::string_view{},
            std::string_view vr = std::string_view{}) {
        throw test_failure{format_failure_message(
                s.file_name(), s.line(), s.column(), op, vl, vr)};
    }


}
