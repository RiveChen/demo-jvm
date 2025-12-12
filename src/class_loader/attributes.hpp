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

namespace jvm::class_loader {

class ByteReader;
class ClassFileParser;
class ConstantPool;

class AttributeInfo {
 public:
  virtual ~AttributeInfo()                       = default;
  virtual void readInfo(ClassFileParser& parser) = 0;

  U2 name_index;
  U4 length;

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
  void            readInfo(ClassFileParser& parser) override;
  std::vector<U1> info;
};

// The ConstantValueAttribute class
class ConstantValueAttribute : public AttributeInfo {
 public:
  void readInfo(ClassFileParser& parser) override;
  U2   value;
};

// The ExceptionsAttribute class
class ExceptionsAttribute : public AttributeInfo {
 public:
  void            readInfo(ClassFileParser& parser) override;
  U2              num_exceptions;
  std::vector<U2> exceptions;
};

// The BootstrapMethodsAttribute class
struct BootstrapMethod {
  U2              method_ref;
  U2              num_arguments;
  std::vector<U2> arguments;
};

class BootstrapMethodsAttribute : public AttributeInfo {
 public:
  void                         readInfo(ClassFileParser& parser) override;
  std::vector<BootstrapMethod> bootstrap_methods;

  U2 num_bootstrap_methods;
};

// The CodeAttribute class
struct ExceptionTableEntry {
  U2 start_pc;
  U2 end_pc;
  U2 handler_pc;
  U2 catch_type;
};

class CodeAttribute : public AttributeInfo {
 public:
  void readInfo(ClassFileParser& parser) override;

  // --- Member Variables ---
  U2 max_stack;   // Maximum depth of the operand stack
  U2 max_locals;  // Number of local variables

  // The actual bytecode
  U4              code_length;
  std::vector<U1> code;

  // The exception handler table
  U2                               num_exception_table;
  std::vector<ExceptionTableEntry> exception_table;

  // Nested attributes
  AttributeTable attributes;
};

}  // namespace jvm::class_loader
