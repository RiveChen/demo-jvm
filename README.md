# A demo JVM in C++

This is a demo JVM of jvms-8 in modern C++ that I am building to learn more about the JVM.

## Features

- [ ] Class loading
- [ ] Method execution
- [ ] Garbage collection
- [ ] Threading
- [ ] Native methods
- [ ] Exception handling
- [ ] Reflection
- [ ] Annotations

## Requirements

- CMake 3.20 or higher
- C++20 compatible compiler
- Git (for fetching GoogleTest)

## Building

``` sh
# Create build directory
mkdir build && cd build
# Configure
cmake ..
# Build
cmake --build .
# Run tests (when available)
ctest
```
