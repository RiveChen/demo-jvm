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

namespace class_loader {

class ClassFileParser {
 public:
  /**
   * @brief Construct a new ClassFileParser object
   * @param data Data to parse, not owned by the ClassFileParser
   */
  explicit ClassFileParser(std::span<common::U1> data) : reader_(data) {}

  /**
   * @brief A factory method to parse the class file
   * @return std::unique_ptr<ClassFile>
   */
  std::unique_ptr<ClassFile> parse();

  ByteReader& getReader() { return reader_; }

 private:
  void         parseMagic();
  Version      parseVersion();
  ConstantPool parseConstantPool();

  std::unique_ptr<ConstantInfo> createConstantInfo();

  // stateful parsing
  ByteReader reader_;
};

}  // namespace class_loader