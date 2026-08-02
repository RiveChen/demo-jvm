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
# Configure + build the development preset
cmake --preset dev
cmake --build --preset dev

# Run all tests
ctest --preset dev

# Run HelloWorld
./build/dev/bin/demo-jvm build/dev/test_classes tests.data.java.HelloWorld

# Optimized build
cmake --preset release
cmake --build --preset release
ctest --preset release
```

## Sanitizers (UBSan / ASan)

Enable via [CMakePresets.json](CMakePresets.json):

```sh
# UndefinedBehaviorSanitizer
cmake --preset ubsan
cmake --build --preset ubsan
ctest --preset ubsan

# AddressSanitizer
cmake --preset asan
cmake --build --preset asan
ctest --preset asan
```

Each preset has an isolated directory under `build/<preset>`.

## Developer Toolchain

| Tool                           | Purpose                                  | Config                                                  |
| ------------------------------ | ---------------------------------------- | ------------------------------------------------------- |
| CMake Tools (VSCode extension) | Configure/build/test                     | `.vscode/settings.json`, `CMakePresets.json`            |
| clangd                         | Completion, navigation, live diagnostics | `.clangd`, `.clang-tidy`                                |
| clang-tidy                     | Static analysis                          | `.clang-tidy`                                           |
| clang-format                   | Code formatting                          | `.clang-format` (`cmake --build build/dev --target format`) |
| C++ TestMate                   | GoogleTest discovery (Testing panel)     | `.vscode/settings.json`                                 |

`compile_commands.json` is generated in each `build/<preset>/` directory for clangd / clang-tidy.

## Quality Gates

```sh
cmake --build --preset dev
cmake --build build/dev --target check-format
cmake --build --preset release
cmake --build --preset asan && ctest --preset asan
cmake --build --preset ubsan && ctest --preset ubsan
```
