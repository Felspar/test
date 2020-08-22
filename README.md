# Felspar Test


**Testing library with cmake integration for C++20**

The test framework is currently at the pre-alpha "proof-of-concept" level and shows that we can now do testing properly without macros.

The framework is easy to use from your `CMakeLists.txt` and tests can be run as part of your normal build with no extra steps needed.

Works with Clang 10 and GCC 10 in C++20 mode with both libstdc++ and libc++.

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
        GIT_REPOSITORY git@github.com:Felspar/test.git
        GIT_TAG main
    )
FetchContent_GetProperties(felspar-test)
if(NOT felspar-test_POPULATED)
    FetchContent_Populate(felspar-test)
    add_subdirectory(${felspar-test_SOURCE_DIR} ${felspar-test_BINARY_DIR})
endif()
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
            check(ptr != nullptr).is_truthy();
        });
```

You can also use the `.test` member of the returned object, which also takes an optional test name:

```cpp
static auto const unary = felspar::testsuite("checks...unary")
        .test([](auto check) { check(true); })
        .test("truthy", [](auto check) {
            auto const *ptr = "";
            check(ptr).is_truthy();
            check(ptr != nullptr).is_truthy();
        });
```

Finally you can separate the tests. This is useful when creating macros to help with porting from other test suites:

```cpp
static auto const unary = felspar::testsuite("checks...unary");
static auto const unary_1 = unary.test([](auto check) { check(true); });
static auto const unary_truthy = unary.test("truthy", [](auto check) {
        auto const *ptr = "";
        check(ptr).is_truthy();
        check(ptr != nullptr).is_truthy();
    });
```


## Test operations

The object injected into the tests (conventionally called `check`) is used as the basis of the assertions. Values can be wrapped and then compared (only `==` and `!=` are currently supported):

```cpp
check(some_value) == other_value;
```

Values can also be checked to ensure they will work correctly in conditional contexts:

```cpp
check(non_empty_optional).is_truthy();
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

The first form, `throws`, checks that an exception of the appropriate type will be thrown and that the `what()` string of the caught exception and the passed exception are the same. The second form, `throws_type` only checks that an exception can be caught using a guard of the provided type.


## TODO

* Move implementation around and tidy up names.
* Implement the rest of the comparison operators, including spaceship.
* Print useful values in failure reports if the type supports `stream <<`.
* Report a failure if a `check` isn't done because `check(true);` does nothing.
* Add `skip` and `fails` alongside the `test` registration function.
