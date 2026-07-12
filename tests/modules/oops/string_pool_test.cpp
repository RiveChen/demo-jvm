#include "oops/string_pool.h"

#include <gtest/gtest.h>

#include <string>

using namespace jvm;

namespace {

class StringPoolTest : public ::testing::Test {
 protected:
  void SetUp() override { oops::StringPool::getSingleton().reset(); }
};

}  // namespace

TEST_F(StringPoolTest, InternReturnsEqualContent) {
  const std::string* p = oops::StringPool::getSingleton().intern("Hello, World!");
  ASSERT_NE(p, nullptr);
  EXPECT_EQ(*p, "Hello, World!");
}

TEST_F(StringPoolTest, SameContentInternsToSamePointer) {
  auto& sp = oops::StringPool::getSingleton();
  const std::string* a = sp.intern("Hello, World!");
  // distinct source object, equal content -> must dedup to the same pointer
  const std::string* b = sp.intern(std::string("Hello, ") + "World!");
  EXPECT_EQ(a, b);
}

TEST_F(StringPoolTest, DifferentContentInternsToDifferentPointers) {
  auto& sp = oops::StringPool::getSingleton();
  EXPECT_NE(sp.intern("foo"), sp.intern("bar"));
}

TEST_F(StringPoolTest, PointerStaysValidAfterManyInserts) {
  auto&              sp    = oops::StringPool::getSingleton();
  const std::string* first = sp.intern("first");
  for (int i = 0; i < 1000; ++i) {
    sp.intern("filler-" + std::to_string(i));
  }
  // node-based container: earlier pointer must remain valid, unchanged, and stable
  EXPECT_EQ(sp.intern("first"), first);
  EXPECT_EQ(*first, "first");
}
