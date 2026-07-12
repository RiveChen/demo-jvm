add_executable(test_heap heap_test.cpp)
target_link_libraries(test_heap PRIVATE jvm_memory jvm_classfile GTest::gtest_main)

# compile testing .java files to .class files
add_dependencies(test_heap compile_test_classes)

# inject test class path
target_compile_definitions(
  test_heap PRIVATE TEST_CLASS_PATH="${CMAKE_BINARY_DIR}/test_classes")

gtest_discover_tests(test_heap)
