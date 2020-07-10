add_library(felspar-test STATIC suite.cpp)
target_include_directories(felspar-test PUBLIC ../include)
target_compile_features(felspar-test PUBLIC cxx_std_20)
install(DIRECTORY ../include/felspar DESTINATION include)
