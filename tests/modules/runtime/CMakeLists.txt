add_executable(test_runtime local_variables_test.cpp operand_stack_test.cpp method_area_test.cpp klass_test.cpp constant_pool_test.cpp)
target_link_libraries(test_runtime PRIVATE jvm_runtime jvm_classloader GTest::gtest_main)

# compile testing .java files to .class files
add_dependencies(test_runtime compile_test_classes)

# inject test class path
target_compile_definitions(test_runtime PRIVATE
    TEST_CLASS_PATH="${CMAKE_BINARY_DIR}/test_classes"
)

gtest_discover_tests(test_runtime)