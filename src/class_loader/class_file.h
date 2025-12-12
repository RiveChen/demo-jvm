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
#include "constant_pool.h"
#include "version.h"

namespace class_loader {

constexpr common::U4 kClassFileMagic = 0xCAFEBABE;

class ClassFile {
 public:
  explicit ClassFile(Version version, ConstantPool constant_pool)
    : version(std::move(version)), constant_pool(std::move(constant_pool)) {}
  ClassFile(const ClassFile&)            = delete;
  ClassFile(ClassFile&&)                 = default;
  ClassFile& operator=(const ClassFile&) = delete;
  ClassFile& operator=(ClassFile&&)      = default;
  ~ClassFile()                           = default;

  Version      version;
  ConstantPool constant_pool;
};

}  // namespace class_loader