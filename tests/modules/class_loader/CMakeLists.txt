add_executable(test_class_loader byte_reader_test.cpp)
target_link_libraries(test_class_loader PRIVATE jvm_classloader GTest::gtest_main)

# compile testing .java files to .class files
add_dependencies(test_class_loader compile_test_classes)

# inject test class path
target_compile_definitions(test_class_loader PRIVATE
    TEST_CLASS_PATH="${CMAKE_BINARY_DIR}/test_classes"
)

gtest_discover_tests(test_class_loader)