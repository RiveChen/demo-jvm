#include "common/access_flags.hpp"

#include <gtest/gtest.h>

TEST(AccessFlagsTest, ClassAccessFlags) {
  common::AccessFlags<common::flags::Class> access_flags(0x0001);
  EXPECT_TRUE(access_flags.has(common::flags::Class::PUBLIC));
  EXPECT_FALSE(access_flags.has(common::flags::Class::ABSTRACT));
}