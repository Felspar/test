# Cross-compiling

`felspar-test` runs each test as a post-build step, and also registers it with
CTest, so that a successful build means the tests passed. When you cross-compile,
the test executables are built for the *target* platform and can't be run
directly on the build host. You have two options: run them on the host through an
emulator, or leave the test targets out of the cross build and run them from a
native build instead (see
[Building without the tests](#building-without-the-tests)).

To run them on the host, the test runner invokes each test through CMake's
standard `CMAKE_CROSSCOMPILING_EMULATOR`:

```cmake
add_custom_command(TARGET ${name} POST_BUILD
    COMMAND ${CMAKE_CROSSCOMPILING_EMULATOR} $<TARGET_FILE:${name}>)
```

`CMAKE_CROSSCOMPILING_EMULATOR` is empty for a native build, so the test runs
directly. For a cross build you set it -- usually in your toolchain file -- to a
command that can run a target binary on the host. CMake applies the same variable
to the `add_test()` registration automatically, so `ctest` behaves the same way.


## Providing an emulator

The emulator is a property of *your* cross build, not of `felspar-test`. Set it in
the toolchain file you pass via `-DCMAKE_TOOLCHAIN_FILE=...`.

For a foreign-architecture Linux target a user-mode emulator is often enough on
its own:

```cmake
set(CMAKE_CROSSCOMPILING_EMULATOR qemu-aarch64)
```

When running a target binary needs more than a single command -- shared libraries
on a search path, environment variables, argument translation -- point it at a
small wrapper script instead:

```cmake
set(CMAKE_CROSSCOMPILING_EMULATOR "${CMAKE_CURRENT_LIST_DIR}/run-target")
```

The wrapper receives the target executable followed by its arguments, sets up
whatever the environment needs and execs the real emulator. For example, running
mingw-w64 Windows executables under wine from a Linux host:

```bash
#!/usr/bin/env bash
set -euo pipefail

# Let wine find the mingw runtime DLLs, as Windows-style ';'-separated Z: paths
# (resolved from the compiler so the threading variant matches what was linked).
target=x86_64-w64-mingw32
stdcxx_dir=$(dirname "$(clang --target=$target -print-file-name=libstdc++-6.dll)")
winpthread_dir=$(dirname "$(clang --target=$target -print-file-name=libwinpthread-1.dll)")
export WINEPATH="Z:${stdcxx_dir};Z:${winpthread_dir}"
export WINEDEBUG=-all

exe=$1
shift
# Rewrite absolute Unix path arguments to Z: paths (wine maps the root to Z:).
args=()
for a in "$@"; do
    if [[ $a == /* && -e $a ]]; then args+=("Z:$a"); else args+=("$a"); fi
done
exec wine "$exe" "${args[@]}"
```


## Building without the tests

Running the tests on the host is optional. The test executables are created
`EXCLUDE_FROM_ALL` and only attached to your test target, so they -- and the
emulator -- only come into play when you build that target.

If you would rather run the tests only from a native build, arrange for the cross
build not to create them at all. Turn off `FELSPAR_CHECK_TESTS` (which stops
`felspar-test` building and registering its own tests, and is a natural flag to
gate your own `add_test_run` calls on), or guard the test target and its tests
directly:

```cmake
if(NOT CMAKE_CROSSCOMPILING)
    add_custom_target(check)
    add_test_run(check your-library TESTS checks1.cpp checks2.cpp)
endif()
```

The cross build then produces only the shipping artifacts, with no emulator
involved.


## Why the wrapper is not part of this library

A wrapper like the one above is specific to a toolchain (here mingw + wine) and
often to a project -- it may also need to select a headless graphics or audio
backend, locate the program's data, and so on. `felspar-test` stays free of all
of that by depending only on the generic `CMAKE_CROSSCOMPILING_EMULATOR` hook.
Keep the wrapper next to your toolchain file, where those target- and
project-specific choices already live, so the test library remains
platform-agnostic.
