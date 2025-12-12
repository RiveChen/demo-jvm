#include "class_loader/class_file.h"

#include <gtest/gtest.h>

#include <fstream>
#include <memory>
#include <string>

#include "class_loader/class_file_parser.h"

using namespace jvm;

class ClassFileTest : public ::testing::Test {
 protected:
  std::unique_ptr<std::ifstream> getClassFileStream(const std::string& className) {
    std::string classPath = TEST_CLASS_PATH;
    std::string fullPath  = classPath + "/tests/data/java/" + className + ".class";
    auto        stream    = std::make_unique<std::ifstream>(fullPath, std::ios::binary);
    return stream;
  }

  void SetUp() override {
    class_file_stream_ = getClassFileStream(class_file_name_);
    ASSERT_TRUE(class_file_stream_->is_open()) << "Failed to open " << class_file_name_ << ".class";
    class_file_data_ = std::vector<U1>(std::istreambuf_iterator<char>(*class_file_stream_), {});
    parser_          = std::make_unique<class_loader::ClassFileParser>(class_file_data_);
  }

  std::string                                    class_file_name_ = "HelloWorld";
  std::unique_ptr<std::ifstream>                 class_file_stream_;
  std::vector<U1>                                class_file_data_;
  std::unique_ptr<class_loader::ClassFileParser> parser_;
};

TEST_F(ClassFileTest, ParseMagic) {
  EXPECT_EQ(parser_->getReader().read<U4>(), class_loader::kClassFileMagic);
}

TEST_F(ClassFileTest, InvalidMagic) {
  std::vector<U1>               invalid_data = {0x00, 0x00, 0x00, 0x00};
  class_loader::ClassFileParser invalid_parser(invalid_data);
  EXPECT_THROW(invalid_parser.parse(), std::runtime_error);
}

TEST_F(ClassFileTest, ParseVersion) {
  auto class_file = parser_->parse();
  EXPECT_EQ(class_file->version.toString(), "52.0");
}

TEST_F(ClassFileTest, ParseConstantPool) {
  auto  class_file    = parser_->parse();
  auto& constant_pool = class_file->constant_pool;
  EXPECT_EQ(constant_pool.size(), 26);
  EXPECT_EQ(constant_pool.getUtf8String(4), "java/lang/Object");
  EXPECT_EQ(constant_pool.getClassName(2), "java/lang/Object");
  EXPECT_THROW(constant_pool.getConstantInfo(0), std::invalid_argument);
}

TEST_F(ClassFileTest, ParseAccessFlags) {
  auto class_file = parser_->parse();
  EXPECT_EQ(class_file->access_flags.getFlags(), 0x0021);
  EXPECT_TRUE(class_file->access_flags.has(flags::Class::PUBLIC));
  EXPECT_TRUE(class_file->access_flags.has(flags::Class::SUPER));
  EXPECT_FALSE(class_file->access_flags.has(flags::Class::ABSTRACT));
}

TEST_F(ClassFileTest, ParseClassIndex) {
  auto class_file = parser_->parse();
  EXPECT_EQ(class_file->this_class_index, 21);
  EXPECT_EQ(class_file->constant_pool.getClassName(class_file->this_class_index),
            "tests/data/java/HelloWorld");
}

TEST_F(ClassFileTest, ParseSuperClassIndex) {
  auto class_file = parser_->parse();
  EXPECT_EQ(class_file->super_class_index, 2);
  EXPECT_EQ(class_file->constant_pool.getClassName(class_file->super_class_index),
            "java/lang/Object");
}

TEST_F(ClassFileTest, ParseInterfaces) {
  auto class_file = parser_->parse();
  EXPECT_EQ(class_file->interfaces_count, 0);
  EXPECT_EQ(class_file->interfaces, std::vector<U2>());
}

TEST_F(ClassFileTest, ParseFields) {
  auto class_file = parser_->parse();
  EXPECT_EQ(class_file->fields.getMembers().size(), 0);
}

TEST_F(ClassFileTest, ParseMethods) {
  auto class_file = parser_->parse();
  EXPECT_EQ(class_file->methods.getMembers().size(), 2);
  EXPECT_EQ(class_file->methods.getMembers()[0]->name, "<init>");
  EXPECT_EQ(
    class_file->constant_pool.getUtf8String(class_file->methods.getMembers()[0]->descriptor_index),
    "()V");
}

TEST_F(ClassFileTest, ParseAttributes) {
  auto class_file = parser_->parse();
  EXPECT_EQ(class_file->attributes.getAttributes().size(), 0);
}