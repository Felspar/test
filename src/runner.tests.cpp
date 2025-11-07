#include <felspar/test.hpp>


namespace {


    auto const format =
            felspar::testsuite("format")
                    .test("no extra",
                          [](auto check) {
                              check(felspar::test::format_failure_message(
                                      "filename.cpp", 10, 4, ""))
                                      == "filename.cpp:10:4: Error";
                          })
                    .test("with extra message",
                          [](auto check) {
                              check(felspar::test::format_failure_message(
                                      "filename.cpp", 10, 4, "extra message"))
                                      == "filename.cpp:10:4: Error\n    extra message";
                          })
                    .test("unary check",
                          [](auto check) {
                              check(felspar::test::format_failure_message(
                                      "filename.cpp", 10, 4, "test", "value1"))
                                      == "filename.cpp:10:4: Error\n    check(value1) test";
                          })
                    .test("binary check", [](auto check) {
                        check(felspar::test::format_failure_message(
                                "filename.cpp", 10, 4, "test", "value1",
                                "value2"))
                                == "filename.cpp:10:4: Error\n    check(value1) test value2";
                    });


}
