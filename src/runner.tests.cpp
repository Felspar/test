#include <felspar/test.hpp>


namespace {


    auto const format = felspar::testsuite("format")

                                .test("no extra",
                                      [](auto check) {
                                          check(felspar::test::format_failure_message("filename.cpp", 10, 4, "test")) ==
        "Failed at filename.cpp:10:4";
                                      })

                                .test("unary check",
                                      [](auto check) {
                                          check(felspar::test::format_failure_message("filename.cpp", 10, 4, "test", "value1")) ==
        "Failed at filename.cpp:10:4\ncheck(value1) test";
                                      })

                                .test("binary check", [](auto check) {
                                    check(felspar::test::format_failure_message("filename.cpp", 10, 4, "test", "value1", "value2" )) ==
        "Failed at filename.cpp:10:4\ncheck(value1) test value2";
                                });


}
