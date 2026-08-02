cmake_minimum_required(VERSION 3.20)

project(
  demo_jvm
  VERSION 1.0.0
  DESCRIPTION "A C++ implementation of JVM"
  LANGUAGES CXX)

include(CTest)

if("${CMAKE_BINARY_DIR}" STREQUAL "${CMAKE_SOURCE_DIR}")
  message(
    FATAL_ERROR
      "
    ###########################################################
    Error: In-source builds are forbidden!
    You must generate the build system in a separate directory.

    Step 1: Clean up the generated files (CMakeCache.txt, CMakeFiles/, etc.)
    Step 2: Create a build directory (e.g., mkdir build)
    Step 3: Run cmake from there (e.g., cd build && cmake ..)
    ###########################################################
    ")
endif()

# Print the active C++ compiler (useful when switching presets/toolchains)
message(STATUS "C++ Compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION} (${CMAKE_CXX_COMPILER})")
message(STATUS "C++ Standard: C++20 (target compile feature)")

# Generate compile_commands.json for clang-tidy and IDE support
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

option(ENABLE_ASAN "Enable Address Sanitizer" OFF)
option(ENABLE_UBSAN "Enable Undefined Behavior Sanitizer" OFF)

# First-party targets opt into one shared target-level build strategy. Third-party
# dependencies added through FetchContent do not inherit these requirements.
add_library(demo_jvm_build_options INTERFACE)
target_compile_features(demo_jvm_build_options INTERFACE cxx_std_20)

if(MSVC)
  target_compile_options(
    demo_jvm_build_options
    INTERFACE $<$<COMPILE_LANGUAGE:CXX>:/W4>
              $<$<COMPILE_LANGUAGE:CXX>:/permissive->)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "^(GNU|Clang|AppleClang)$")
  target_compile_options(
    demo_jvm_build_options
    INTERFACE $<$<COMPILE_LANGUAGE:CXX>:-Wall>
              $<$<COMPILE_LANGUAGE:CXX>:-Wextra>
              $<$<COMPILE_LANGUAGE:CXX>:-Wpedantic>
              $<$<COMPILE_LANGUAGE:CXX>:-Wconversion>
              $<$<COMPILE_LANGUAGE:CXX>:-Wextra-semi>)
else()
  message(WARNING "No warning policy is defined for ${CMAKE_CXX_COMPILER_ID}")
endif()

if(ENABLE_ASAN OR ENABLE_UBSAN)
  if(NOT CMAKE_CXX_COMPILER_ID MATCHES "^(GNU|Clang|AppleClang)$")
    message(FATAL_ERROR "The configured compiler does not support this project's sanitizer flags")
  endif()

  target_compile_options(
    demo_jvm_build_options INTERFACE -fno-omit-frame-pointer)
endif()

if(ENABLE_ASAN)
  message(STATUS "Address Sanitizer Enabled")
  target_compile_options(demo_jvm_build_options INTERFACE -fsanitize=address)
  target_link_options(demo_jvm_build_options INTERFACE -fsanitize=address)
endif()

if(ENABLE_UBSAN)
  message(STATUS "Undefined Behavior Sanitizer Enabled")
  # NOTE: No -fno-sanitize-recover here on purpose.
  # With recovery enabled, UBSan reports violations to stderr and lets the
  # process continue, so all tests within one executable still run to
  # completion instead of aborting at the first violation (SIGABRT).
  target_compile_options(demo_jvm_build_options INTERFACE -fsanitize=undefined)
  target_link_options(demo_jvm_build_options INTERFACE -fsanitize=undefined)
endif()

function(demo_jvm_configure_target target)
  if(NOT TARGET ${target})
    message(FATAL_ERROR "Cannot configure unknown target: ${target}")
  endif()

  set_target_properties(${target} PROPERTIES CXX_EXTENSIONS OFF)
  target_link_libraries(${target} PRIVATE demo_jvm_build_options)
endfunction()

# Production targets are always available.
add_subdirectory(src)

if(BUILD_TESTING)
  # Test-only dependencies and targets stay out of production-only builds.
  include(FetchContent)
  FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.17.0)

  # For Windows: Prevent overriding the parent project's compiler/linker settings.
  if(MSVC)
    set(gtest_force_shared_crt
        ON
        CACHE BOOL "" FORCE)
  endif()

  FetchContent_MakeAvailable(googletest)
  add_subdirectory(tests)

  add_custom_target(
    test_all
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure --progress
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Running all tests (with output on failure)..."
    USES_TERMINAL)
endif()

# Format all C++ source files with clang-format
file(
  GLOB_RECURSE ALL_SOURCE_FILES
  ${CMAKE_SOURCE_DIR}/src/*.cpp ${CMAKE_SOURCE_DIR}/src/*.hpp
  ${CMAKE_SOURCE_DIR}/include/*.hpp ${CMAKE_SOURCE_DIR}/include/*.h
  ${CMAKE_SOURCE_DIR}/tests/*.cpp ${CMAKE_SOURCE_DIR}/tests/*.hpp)

add_custom_target(
  format
  COMMAND clang-format -i ${ALL_SOURCE_FILES}
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  COMMENT "Formatting all source files with clang-format..."
  USES_TERMINAL)

add_custom_target(
  check-format
  COMMAND clang-format --dry-run --Werror ${ALL_SOURCE_FILES}
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  COMMENT "Checking format of all source files..."
  USES_TERMINAL)
