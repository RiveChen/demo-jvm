# Test base header path
set(TEST_BASE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

# Arithmetic tests (existing)
add_executable(test_interpreter_arithmetic interpreter_arithmetic_test.cpp)
target_link_libraries(test_interpreter_arithmetic PRIVATE jvm_engine jvm_runtime jvm_classloader GTest::gtest_main)
target_include_directories(test_interpreter_arithmetic PRIVATE ${TEST_BASE_DIR})
add_dependencies(test_interpreter_arithmetic compile_test_classes)
target_compile_definitions(test_interpreter_arithmetic PRIVATE
    TEST_CLASS_PATH="${CMAKE_BINARY_DIR}/test_classes"
)
gtest_discover_tests(test_interpreter_arithmetic)

# Constants tests
add_executable(test_interpreter_constants interpreter_constants_test.cpp)
target_link_libraries(test_interpreter_constants PRIVATE jvm_engine jvm_runtime jvm_classloader GTest::gtest_main)
target_include_directories(test_interpreter_constants PRIVATE ${TEST_BASE_DIR})
add_dependencies(test_interpreter_constants compile_test_classes)
target_compile_definitions(test_interpreter_constants PRIVATE
    TEST_CLASS_PATH="${CMAKE_BINARY_DIR}/test_classes"
)
gtest_discover_tests(test_interpreter_constants)

# Load/Store tests
add_executable(test_interpreter_load_store interpreter_load_store_test.cpp)
target_link_libraries(test_interpreter_load_store PRIVATE jvm_engine jvm_runtime jvm_classloader GTest::gtest_main)
target_include_directories(test_interpreter_load_store PRIVATE ${TEST_BASE_DIR})
add_dependencies(test_interpreter_load_store compile_test_classes)
target_compile_definitions(test_interpreter_load_store PRIVATE
    TEST_CLASS_PATH="${CMAKE_BINARY_DIR}/test_classes"
)
gtest_discover_tests(test_interpreter_load_store)

# Stack operation tests
add_executable(test_interpreter_stack interpreter_stack_test.cpp)
target_link_libraries(test_interpreter_stack PRIVATE jvm_engine jvm_runtime jvm_classloader GTest::gtest_main)
target_include_directories(test_interpreter_stack PRIVATE ${TEST_BASE_DIR})
add_dependencies(test_interpreter_stack compile_test_classes)
target_compile_definitions(test_interpreter_stack PRIVATE
    TEST_CLASS_PATH="${CMAKE_BINARY_DIR}/test_classes"
)
gtest_discover_tests(test_interpreter_stack)

# Conversion tests
add_executable(test_interpreter_conversion interpreter_conversion_test.cpp)
target_link_libraries(test_interpreter_conversion PRIVATE jvm_engine jvm_runtime jvm_classloader GTest::gtest_main)
target_include_directories(test_interpreter_conversion PRIVATE ${TEST_BASE_DIR})
add_dependencies(test_interpreter_conversion compile_test_classes)
target_compile_definitions(test_interpreter_conversion PRIVATE
    TEST_CLASS_PATH="${CMAKE_BINARY_DIR}/test_classes"
)
gtest_discover_tests(test_interpreter_conversion)

