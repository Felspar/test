# Felspar Test

**Testing library with cmake integration for C++20**

The test framework is currently at the pre-alpha "proof-of-concept" level and shows that we can now do testing properly without macros.


```cpp
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
```

To use add it as a sub-directory in your project and then just use `add_subdirectory`, or use:

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
