/**
 * @file class_file.h
 * @author Rive Chen
 * @brief Class file for representing class files
 * @version 0.1
 * @date 2025-12-11
 *
 * @copyright Copyright (c) 2025
 *
 */
#pragma once

#include "common/types.h"

namespace class_loader {

constexpr common::U4 kClassFileMagic = 0xCAFEBABE;

class ClassFile {
 public:
  ClassFile() {}
  ClassFile(const ClassFile&)            = delete;
  ClassFile(ClassFile&&)                 = default;
  ClassFile& operator=(const ClassFile&) = delete;
  ClassFile& operator=(ClassFile&&)      = default;
  ~ClassFile()                           = default;

  friend class ClassFileParser;
};

}  // namespace class_loader