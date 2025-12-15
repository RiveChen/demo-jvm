add_executable(test_interpreter_arithmetic interpreter_arithmetic_test.cpp)
target_link_libraries(test_interpreter_arithmetic PRIVATE jvm_engine jvm_runtime jvm_classloader GTest::gtest_main)

# compile testing .java files to .class files
add_dependencies(test_interpreter_arithmetic compile_test_classes)

# inject test class path
target_compile_definitions(test_interpreter_arithmetic PRIVATE
    TEST_CLASS_PATH="${CMAKE_BINARY_DIR}/test_classes"
)

gtest_discover_tests(test_interpreter_arithmetic)

