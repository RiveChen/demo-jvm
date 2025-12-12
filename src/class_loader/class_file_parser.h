/**
 * @file class_file_parser.h
 * @author Rive Chen
 * @brief Class file parser for parsing class files
 * @version 0.1
 * @date 2025-12-11
 *
 * @copyright Copyright (c) 2025
 *
 */
#pragma once

#include <memory>

#include "byte_reader.hpp"
#include "class_file.h"

namespace jvm::class_loader {

class ClassFileParser {
 public:
  /**
   * @brief Construct a new ClassFileParser object
   * @param data Data to parse, not owned by the ClassFileParser
   */
  explicit ClassFileParser(std::span<U1> data) : reader_(data) {}

  /**
   * @brief A factory method to parse the class file
   * @return std::unique_ptr<ClassFile>
   */
  std::unique_ptr<ClassFile> parse();

  ByteReader&         getReader() { return reader_; }
  const ConstantPool& getConstantPool() { return *constant_pool_ref_; }

 private:
  void                      parseMagic();
  Version                   parseVersion();
  ConstantPool              parseConstantPool();
  AccessFlags<flags::Class> parseAccessFlags();
  MemberTable               parseFields();
  MemberTable               parseMethods();
  AttributeTable            parseAttributes();

  std::unique_ptr<ConstantInfo>  createConstantInfo();
  std::unique_ptr<AttributeInfo> createAttributeInfo();
  std::unique_ptr<FieldInfo>     createFieldInfo();
  std::unique_ptr<MethodInfo>    createMethodInfo();

  // stateful parsing
  ByteReader reader_;
  // holding a ref to the constant pool to access the constant pool entries
  const ConstantPool* constant_pool_ref_{nullptr};

  // the following classes need parsing of nested attributes
  friend class CodeAttribute;
  friend class FieldInfo;
  friend class MethodInfo;
};

}  // namespace jvm::class_loader