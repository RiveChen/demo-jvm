cmake_minimum_required(VERSION 3.20)

project(demo_jvm
  VERSION 1.0.0
  DESCRIPTION "A C++ implementation of JVM"
  LANGUAGES CXX
)

if("${CMAKE_BINARY_DIR}" STREQUAL "${CMAKE_SOURCE_DIR}")
    message(FATAL_ERROR "
    ###########################################################
    Error: In-source builds are forbidden!
    You must generate the build system in a separate directory.
    
    Step 1: Clean up the generated files (CMakeCache.txt, CMakeFiles/, etc.)
    Step 2: Create a build directory (e.g., mkdir build)
    Step 3: Run cmake from there (e.g., cd build && cmake ..)
    ###########################################################
    ")
endif()

# Modern CMake: Set C++ standard per target instead of globally
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF) # Turn off C++ extensions to avoid compatibility issues

# Generate compile_commands.json for clang-tidy and IDE support
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Global compiler options
if(MSVC)
    add_compile_options(/W4 /permissive-)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Address Sanitizer
option(ENABLE_ASAN "Enable Address Sanitizer" OFF)
if(ENABLE_ASAN AND NOT MSVC)
    message(STATUS "Address Sanitizer Enabled")
    add_compile_options(-fsanitize=address -g)
    add_link_options(-fsanitize=address)
endif()

# GoogleTest Integration
include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        v1.17.0
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Add subdirectories
add_subdirectory(src)

# Enable testing
enable_testing()
add_subdirectory(tests)

# Add a custom target to run all tests
add_custom_target(test_all
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure --progress # --parallel 8
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Running all tests (with output on failure)..."
    USES_TERMINAL
)