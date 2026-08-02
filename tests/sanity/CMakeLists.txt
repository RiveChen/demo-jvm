add_executable(test_sanity env_test.cpp)
demo_jvm_configure_target(test_sanity)
target_link_libraries(test_sanity PRIVATE GTest::gtest_main)
gtest_discover_tests(test_sanity PROPERTIES LABELS "sanity")
