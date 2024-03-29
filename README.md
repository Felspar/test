# Felspar Test


**Testing framework with cmake integration for C++**

[![Documentation](https://badgen.net/static/docs/felspar.com)](https://felspar.com/test/)
[![GitHub](https://badgen.net/badge/Github/felspar-test/green?icon=github)](https://github.com/Felspar/test)
[![License](https://badgen.net/github/license/Felspar/test)](https://github.com/Felspar/test/blob/main/LICENSE_1_0.txt)
[![Discord](https://badgen.net/badge/icon/discord?icon=discord&label)](https://discord.gg/tKSabUa52v)

The framework is easy to use from your `CMakeLists.txt` and tests can be run as part of your normal build with no extra steps needed. The tests are also easy to write using standard C++ syntax and operators with no macros to memorize.


## Basic usage

```cpp
#include <felspar/test.hpp>

namespace {
auto const vector = felspar::testsuite(
        "vector",
        [](auto check) { check(std::vector<int>{}.size()) == 0; },
        [](auto check) {
            std::vector items = {1, 2, 3, 4};
            check(items.size()) == 4;
            check(items.front()) == 1;
            check(items[1]) == 2;
            check(items[2]) == 3;
            check(items.back()) == 4;
            check(items.begin()) == items.cbegin();
        },
        [](auto check) {
            std::vector<std::string> ss;
            check([&]() {
                ss.at(1);
            }).template throws_type<std::out_of_range>();
        });
}
```


## CMake integration

To use, add it as a sub-directory in your project and then just `add_subdirectory`, or use:

```cmake
include(FetchContent)

FetchContent_Declare(
        felspar-test
        GIT_REPOSITORY https://github.com/Felspar/test.git
        GIT_TAG main
    )
FetchContent_MakeAvailable(felspar-test)
```

You will need to define a test target that you want to add the tests to:

```cmake
add_custom_target(check)
```

You may want different targets for unit and integration tests. Add as many as you need. To add tests to that target for a library called `your-library` use:

```cmake
add_test_run(check your-library TESTS checks1.cpp checks2.cpp)
```

Each file will be tested separately. If you need to have a library of test helpers you can do something along these lines:

```cmake
add_library(test-helpers helper1.cpp)
add_test_run(check your-library test-helpers TESTS checks1.cpp checks2.cpp)
```

Anything you add between the test target name and the `TESTS` word will be linked with the test executable.


## Writing tests

You can pass any number of test lambdas into the `testsuite` function and it will return a value that you store and acts as an anchor for the test runner to find and execute the tests.

```cpp
static auto const unary = felspar::testsuite(
        "checks...unary",
        [](auto check) { check(true); },
        [](auto check) {
            auto const *ptr = "";
            check(ptr).is_truthy();
            check(ptr) != nullptr);
        });
```

You can also use the `.test` member of the returned object, which also takes an optional test name:

```cpp
static auto const unary = felspar::testsuite("checks...unary")
        .test([](auto check) { check(true); })
        .test("truthy", [](auto check) {
            auto const *ptr = "";
            check(ptr).is_truthy();
            check(ptr) != nullptr;
        });
```

Finally you can separate the tests. This is useful when creating macros to help with porting from other test suites:

```cpp
static auto const unary = felspar::testsuite("checks...unary");
static auto const unary_1 = unary.test([](auto check) { check(true); });
static auto const unary_truthy = unary.test("truthy", [](auto check) {
        auto const *ptr = "";
        check(ptr).is_truthy();
        check(ptr) != nullptr;
    });
```

These approaches can be freely mixed.


### Logging in tests

The test runner can pass a `std::stringstream` to the the test which can be used for logging which is shown if the test fails.

```cpp
static auto const logger = felspar::testsuite("with logging",
    [](auto check, auto &os) {
        os << "Starting test\n";
        check(false).is_truthy();
    });
```

Might be reported as:

    with logging:1 ... FAIL :-(
    ---output---

    Starting test

    ^^^output^^^
    is_truthy failed at ../../test/test/run/checks.cpp:112:51
    check(0) is_truthy


### Tests with no checks

It is also possible to write tests that don't require checks at all. Simply pass a nullary test lambda for this situation:

```cpp
static auto const nocheck = felspar::testsuite("no check",
    []() {
        std::string{};
    });
```


## Test operations

The object injected into the tests (conventionally called `check`) is used as the basis of the assertions. Values can be wrapped and then compared (only `==`, `!=`, `<`, `<=`, `>`, and `>=` are currently supported):

```cpp
check(some_value) == other_value;
```

Values can also be checked to ensure they will work correctly in conditional contexts:

```cpp
check(non_empty_optional).is_truthy();
check(empty_optional).is_falsey();
```

Exceptions can be checked either by type or by value:

```cpp
check([]() {
    something_that_throws();
}).throws(std::runtime_error{"Argh!"});

check([]() {
    something_that_throws();
}).template throws_type<std::runtime_error>();
```

The first form, `throws`, checks that an exception of the appropriate type will be thrown and that the first lines of the `what()` string of the caught exception and the passed exception are the same. The thrown exception is returned (as a `std::exception_ptr`) so that further checks can be carried out on it if required.

The second form, `throws_type`, only checks that an exception can be caught using a guard of the provided type.


## Failure reporting

If the types used in an expression are printable to a `std::ostream<char>` then this is used to generate a failure message. For example `check(4) != 4` could be reported as:

    maths:7 ... FAIL :-(
    Failed at ../../test/tests.cpp:69:0
    check(4) != 4

If the type doesn't support printing then it will be shown as `?? unprintable ??` instead.


## Time outs

By default the test runner will time out after 30 seconds. To increase this number define the pre-processor symbol `FELSPAR_TEST_RUNNER_TIMEOUT_SECONDS` to a higher (or, if you want, a smaller) number in your build configuration for the tests.


## TODO

* Implement the spaceship operator.
* Report a failure if a `check` isn't done (because `check(true);` does nothing).
* Add `skip` and `fails` alongside the `test` registration function.
* Be more flexible in APIs that take multiple tests and test names.
