# demo-jvm

A teaching JVM 8 implementation written in modern C++20, built to learn the JVM specification (jvms-8).

## Feature Status

| Feature                  |     Status      | Notes                                                                                                        |
| ------------------------ | :-------------: | ------------------------------------------------------------------------------------------------------------ |
| Class loading            |      Done       | Class file parsing, linking, class initialization state machine                                              |
| Method execution         |      Done       | Interpreter (while + switch), most common bytecodes                                                          |
| String constant pool     |     Partial     | Constant pool string dedup (stable pointer from LDC), true interning (`String.intern()`) not yet implemented |
| Native methods           |     Partial     | NativeRegistry + symbol interception (stubs)                                                                 |
| Arrays                   |     Partial     | Klass/type-array infrastructure complete; access instructions pending                                        |
| Exception handling       | Not implemented | VM throws `std::runtime_error`                                                                               |
| Garbage collection       | Not implemented | Bump-pointer arena                                                                                           |
| Multithreading           | Not implemented | Single-threaded runtime                                                                                      |
| Reflection / annotations | Not implemented | -                                                                                                            |

## Requirements

- CMake 3.20+ (minimum for manual configuration via `-B build -S .`)
- CMake 3.28+ (required to use `CMakePresets.json`, schema version 8)
- C++20 compiler (Clang recommended)
- Ninja (build generator)
- Git (fetches GoogleTest)
- JDK (compiles test Java classes with javac `--release 8`)

## Quick Start

```sh
# Configure + build (default Debug, no sanitizers)
cmake --preset default
cmake --build build

# Run all tests
ctest --test-dir build --output-on-failure

# Run HelloWorld
./build/bin/demo-jvm build/test_classes tests.data.java.HelloWorld
```

## Sanitizers (UBSan / ASan)

Enable via [CMakePresets.json](CMakePresets.json):

```sh
# UndefinedBehaviorSanitizer
cmake --preset ubsan && cmake --build build
ctest --test-dir build --output-on-failure

# AddressSanitizer
cmake --preset asan && cmake --build build
ctest --test-dir build --output-on-failure

# Both
cmake --preset asan-ubsan && cmake --build build
```

> All presets share the `build/` directory; switching presets only reconfigures (seconds) and incrementally rebuilds affected files.

## Developer Toolchain

| Tool                           | Purpose                                  | Config                                                  |
| ------------------------------ | ---------------------------------------- | ------------------------------------------------------- |
| CMake Tools (VSCode extension) | Configure/build/test                     | `.vscode/settings.json`, `CMakePresets.json`            |
| clangd                         | Completion, navigation, live diagnostics | `.clangd`, `.clang-tidy`                                |
| clang-tidy                     | Static analysis                          | `.clang-tidy`                                           |
| clang-format                   | Code formatting                          | `.clang-format` (`cmake --build build --target format`) |
| C++ TestMate                   | GoogleTest discovery (Testing panel)     | `.vscode/settings.json`                                 |

`compile_commands.json` is generated in `build/` for clangd / clang-tidy.

## Quality Gates

```sh
cmake --build build                # compile with -Wall -Wextra -Wpedantic -Wshadow -Wconversion
cmake --build build --target check-format   # format check
cmake --preset ubsan               # UBSan full test suite
cmake --preset asan                # ASan full test suite
```
