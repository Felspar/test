#include <felspar/test/runner.hpp>

#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <thread>

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#endif


#ifndef FELSPAR_TEST_RUNNER_TIMEOUT_SECONDS
#define FELSPAR_TEST_RUNNER_TIMEOUT_SECONDS 30
#endif


std::string felspar::test::format_failure_message(
        std::string_view filename,
        std::size_t line,
        std::size_t column,
        std::string_view op,
        std::string_view first,
        std::string_view second) {
    std::stringstream m;
    m << filename << ":" << line << ":" << column << ": Error";
    if (not first.empty()) {
        m << "\n    check(" << first << ") " << op;
        if (not second.empty()) { m << ' ' << second; }
    } else if (not op.empty()) {
        m << "\n    " << op;
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
    std::thread{[]() {
        constexpr std::chrono::seconds timeout{
                FELSPAR_TEST_RUNNER_TIMEOUT_SECONDS};
        std::this_thread::sleep_for(timeout);
        std::cerr << "\n\nTiming out after " << timestr(timeout) << std::endl;
        /**
         * A watchdog must terminate without running `atexit` handlers,
         * static destructors, or stream flushes. That ordered shutdown is
         * unsafe to run from this thread while the main thread is wedged in a
         * blocking call, and on Windows it deadlocks during teardown so the
         * process never exits. Kill the process outright instead. The message
         * above is flushed with `std::endl` since nothing here will flush it
         * for us.
         */
#ifdef _WIN32
        ::TerminateProcess(::GetCurrentProcess(), 127);
#else
        std::_Exit(127);
#endif
    }}.detach();
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
    return std::clamp<std::size_t>(fail, 0, 126);
}
