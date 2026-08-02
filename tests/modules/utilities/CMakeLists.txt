add_executable(test_endian endian_test.cpp access_flags_test.cpp)
demo_jvm_enable_warnings(test_endian)
target_link_libraries(test_endian PRIVATE jvm_utilities GTest::gtest_main)
gtest_discover_tests(test_endian PROPERTIES LABELS "implementation")

add_executable(test_descriptor descriptor_test.cpp)
demo_jvm_enable_warnings(test_descriptor)
target_link_libraries(test_descriptor PRIVATE jvm_utilities GTest::gtest_main)
gtest_discover_tests(test_descriptor PROPERTIES LABELS "implementation")
