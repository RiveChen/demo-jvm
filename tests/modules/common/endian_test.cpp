#include "common/endian.hpp"

#include <gtest/gtest.h>

using namespace jvm;

TEST(EndianTest, ReadBigEndianU2) {
  U2 value = 0x1234;
  EXPECT_EQ(common::swapEndian(value), 0x3412);
}

TEST(EndianTest, ReadBigEndianU4) {
  U4 value = 0x12345678;
  EXPECT_EQ(common::swapEndian(value), 0x78563412);
}

TEST(EndianTest, ReadBigEndianU8) {
  U8 value = 0x1234567890ABCDEF;
  EXPECT_EQ(common::swapEndian(value), 0xEFCDAB9078563412);
}