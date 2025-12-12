#include "class_loader/byte_reader.hpp"

#include <gtest/gtest.h>

#include "common/types.h"

using namespace jvm;

class ByteReaderTest : public ::testing::Test {
 protected:
  void SetUp() override { reader_ = std::make_unique<class_loader::ByteReader>(dummy_bytes_); }

  void TearDown() override {}

  std::vector<U1> dummy_bytes_ = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
  std::unique_ptr<class_loader::ByteReader> reader_;
};

TEST_F(ByteReaderTest, ReadU2AndU1) {
  U2 val2 = reader_->read<U2>();
  U1 val1 = reader_->read<U1>();
  EXPECT_EQ(val2, 0x0102);
  EXPECT_EQ(val1, 0x03);
}

TEST_F(ByteReaderTest, ReadBytesVector) {
  std::vector<U1> out = reader_->readBytes(4);
  ASSERT_EQ(out.size(), 4);
  EXPECT_EQ(out[0], 0x01);
}

TEST_F(ByteReaderTest, ReadIntoBuffer) {
  char buffer[4];
  reader_->readBytes(buffer, 4);
  ASSERT_EQ(buffer[0], 0x01);
  EXPECT_EQ(buffer[1], 0x02);
  EXPECT_EQ(buffer[2], 0x03);
  EXPECT_EQ(buffer[3], 0x04);
}

TEST_F(ByteReaderTest, ReadOutOfBounds) {
  EXPECT_THROW(reader_->readBytes(100), std::out_of_range);
}

TEST_F(ByteReaderTest, ReadIntoNullBuffer) {
  EXPECT_THROW(reader_->readBytes(nullptr, 4), std::invalid_argument);
}