add_executable(test_oops method_area_test.cpp klass_test.cpp
                         constant_pool_test.cpp string_pool_test.cpp
                         array_klass_test.cpp)
demo_jvm_configure_target(test_oops)
target_link_libraries(test_oops PRIVATE jvm_classfile GTest::gtest_main)
# shared fixture header (oops_test_base.hpp) lives in this dir
target_include_directories(test_oops PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# compile testing .java files to .class files
add_dependencies(test_oops compile_test_classes)

# inject test class path
target_compile_definitions(
  test_oops PRIVATE TEST_CLASS_PATH="${CMAKE_BINARY_DIR}/test_classes")

gtest_discover_tests(test_oops PROPERTIES LABELS "implementation")
