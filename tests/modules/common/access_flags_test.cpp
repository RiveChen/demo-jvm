#include "common/access_flags.hpp"

#include <gtest/gtest.h>

using namespace jvm;

TEST(AccessFlagsTest, ClassAccessFlags) {
  AccessFlags<flags::Class> access_flags(0x0001);
  EXPECT_TRUE(access_flags.has(flags::Class::PUBLIC));
  EXPECT_FALSE(access_flags.has(flags::Class::ABSTRACT));
}