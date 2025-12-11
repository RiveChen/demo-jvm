add_executable(test_sanity env_test.cpp)
target_link_libraries(test_sanity GTest::gtest_main)
gtest_discover_tests(test_sanity)