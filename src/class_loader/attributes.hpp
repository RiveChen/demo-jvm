/**
 * @file attributes.hpp
 * @author Rive Chen
 * @brief Attributes in class files, methods and fields, such as generic, constant value,
 * exceptions, bootstrap methods, and code attributes
 * @version 0.1
 * @date 2025-12-12
 *
 * @copyright Copyright (c) 2025
 *
 */
#pragma once

#include <vector>

#include "common/types.h"

namespace class_loader {

class ByteReader;
class ClassFileParser;
class ConstantPool;

class AttributeInfo {
 public:
  virtual ~AttributeInfo()                       = default;
  virtual void readInfo(ClassFileParser& parser) = 0;

  common::U2 name_index;
  common::U4 length;

  std::string name;
};

class AttributeTable {
 public:
  AttributeTable() = default;
  AttributeTable(std::vector<std::unique_ptr<AttributeInfo>>&& attributes)
    : attributes(std::move(attributes)) {}
  AttributeTable(const AttributeTable&)            = delete;
  AttributeTable(AttributeTable&&)                 = default;
  AttributeTable& operator=(const AttributeTable&) = delete;
  AttributeTable& operator=(AttributeTable&&)      = default;
  ~AttributeTable()                                = default;

  template <typename T>
  T* getAttribute() {
    for (auto& attribute : attributes) {
      if (dynamic_cast<T*>(attribute.get())) {
        return dynamic_cast<T*>(attribute.get());
      }
    }
    return nullptr;
  };

  std::vector<std::unique_ptr<AttributeInfo>>& getAttributes() { return attributes; }

 private:
  std::vector<std::unique_ptr<AttributeInfo>> attributes;
};

class GenericAttribute : public AttributeInfo {
 public:
  void                    readInfo(ClassFileParser& parser) override;
  std::vector<common::U1> info;
};

// The ConstantValueAttribute class
class ConstantValueAttribute : public AttributeInfo {
 public:
  void       readInfo(ClassFileParser& parser) override;
  common::U2 value;
};

// The ExceptionsAttribute class
class ExceptionsAttribute : public AttributeInfo {
 public:
  void                    readInfo(ClassFileParser& parser) override;
  common::U2              num_exceptions;
  std::vector<common::U2> exceptions;
};

// The BootstrapMethodsAttribute class
struct BootstrapMethod {
  common::U2              method_ref;
  common::U2              num_arguments;
  std::vector<common::U2> arguments;
};

class BootstrapMethodsAttribute : public AttributeInfo {
 public:
  void                         readInfo(ClassFileParser& parser) override;
  std::vector<BootstrapMethod> bootstrap_methods;

  common::U2 num_bootstrap_methods;
};

// The CodeAttribute class
struct ExceptionTableEntry {
  common::U2 start_pc;
  common::U2 end_pc;
  common::U2 handler_pc;
  common::U2 catch_type;
};

class CodeAttribute : public AttributeInfo {
 public:
  void readInfo(ClassFileParser& parser) override;

  // --- Member Variables ---
  common::U2 max_stack;   // Maximum depth of the operand stack
  common::U2 max_locals;  // Number of local variables

  // The actual bytecode
  common::U4              code_length;
  std::vector<common::U1> code;

  // The exception handler table
  common::U2                       num_exception_table;
  std::vector<ExceptionTableEntry> exception_table;

  // Nested attributes
  AttributeTable attributes;
};

}  // namespace class_loader
