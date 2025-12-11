#include "class_loader/byte_reader.hpp"

#include <gtest/gtest.h>

class ByteReaderTest : public ::testing::Test {
 protected:
  void SetUp() override {
    std::vector<char> dummy_bytes = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
    input_  = new std::istringstream(std::string(dummy_bytes.begin(), dummy_bytes.end()));
    reader_ = new class_loader::ByteReader(*input_);
  }

  void TearDown() override {
    delete reader_;
    delete input_;
  }

  std::istringstream*       input_;
  class_loader::ByteReader* reader_;
};

TEST_F(ByteReaderTest, ReadU2AndU1) {
  common::U2 val2 = reader_->read<common::U2>();
  common::U1 val1 = reader_->read<common::U1>();
  EXPECT_EQ(val2, 0x0102);
  EXPECT_EQ(val1, 0x03);
}

TEST_F(ByteReaderTest, ReadBytesVector) {
  std::vector<common::U1> out = reader_->readBytes(4);
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