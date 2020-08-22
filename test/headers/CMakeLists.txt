add_library(felspar-test-headers
        check.cpp
        report.cpp
        runner.cpp
        test.cpp)
target_link_libraries(felspar-test-headers felspar-test)
