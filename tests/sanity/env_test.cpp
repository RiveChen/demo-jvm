#include <gtest/gtest.h>

#include <vector>

// 1. if macro TEST works
TEST(SanityCheck, MathWorks) {
  EXPECT_EQ(1 + 1, 2);
  EXPECT_TRUE(true);
}

// 2. check C++ standard version
TEST(SanityCheck, CppStandardVersion) {
  // __cplusplus value corresponds to version:
  // 201703L -> C++17
  // 202002L -> C++20
#if __cplusplus >= 201703L
  SUCCEED() << "C++17 or newer is active. Version: " << __cplusplus;
#else
  FAIL() << "Old C++ version detected! Version: " << __cplusplus;
#endif
}

// 3. check STL container support
TEST(SanityCheck, VectorUsage) {
  std::vector<int> v;
  v.push_back(10);
  v.push_back(20);
  EXPECT_EQ(v.size(), 2);
  EXPECT_EQ(v[0], 10);
}