#include <felspar/test/runner.hpp>

#include <iostream>


std::string felspar::test::format_failure_message(
        std::string_view filename,
        std::size_t line,
        std::size_t column,
        std::string_view op,
        std::string_view first,
        std::string_view second) {
    std::stringstream m;
    m << op << " failed at " << filename << ":" << line << ":" << column;
    if (not first.empty()) {
        m << "\ncheck(" << first << ") " << op;
        if (not second.empty()) { m << ' ' << second; }
    }
    return m.str();
}


namespace {
    inline std::string timestr(std::chrono::nanoseconds ns) {
        using namespace std::literals;
        if (ns < 1us) {
            return std::to_string(ns.count()) + "ns";
        } else if (ns < 1ms) {
            return std::to_string(ns.count() / 1000) + "µs";
        } else if (ns < 1s) {
            return std::to_string(ns.count() / 1000'000) + "ms";
        } else {
            return std::to_string(ns.count() / 1000'000'000) + "s";
        }
    }
}


int main() {
    std::size_t number{}, pass{}, fail{};
    for (auto const &test : felspar::test::all_test_cases()) {
        if (test.name.empty()) {
            std::cout << test.suite << ':' << std::to_string(++number);
        } else {
            std::cout << test.suite << ':' << test.name;
        }
        std::cout << std::flush;
        std::stringstream ss;
        if (auto const [eptr, time] = test(ss); not eptr) {
            ++pass;
            std::cout << " ... OK -- " << timestr(time);
        } else {
            ++fail;
            std::cout << " ... FAIL :-( -- " << timestr(time);
            if (auto const str = ss.str(); not str.empty()) {
                std::cout << "\n---output---\n\n" << str << "\n^^^output^^^";
            }
            try {
                std::rethrow_exception(eptr);
            } catch (std::exception const &e) {
                std::cerr << '\n' << e.what();
            } catch (...) { std::cerr << "\nUnknown exception type"; }
        };
        std::cout << '\n';
    }
    std::cout << (pass + fail) << " tests run, " << pass << " passed";
    if (fail) {
        std::cout << " and " << fail << " failed\n";
    } else {
        std::cout << '\n';
    }
    return std::clamp<std::size_t>(fail, 0, 127);
}
