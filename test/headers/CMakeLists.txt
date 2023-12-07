add_library(felspar-test-headers STATIC EXCLUDE_FROM_ALL
        check.cpp
        report.cpp
        runner.cpp
        suite.cpp
        source.cpp
        test.cpp)
target_link_libraries(felspar-test-headers PRIVATE felspar-test)
add_dependencies(felspar-check felspar-test-headers)
