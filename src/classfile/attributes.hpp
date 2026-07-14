/**
 * @file attributes.hpp
 * @brief Class file attribute structures.
 *
 * Implements the attribute structures defined in JVM Specification §4.7.
 * This includes Code, ConstantValue, Exceptions, BootstrapMethods,
 * and generic fallback attributes.
 *
 */
#pragma once

#include <vector>

#include "utilities/types.hpp"

namespace jvm::classfile {

class ByteReader;
class ClassFileParser;
class ConstantPool;

/**
 * @brief Abstract base class for all attribute types (JVM §4.7).
 *
 * Each attribute has a name (resolved from constant pool), length,
 * and a readInfo() method that parses the attribute-specific data.
 */
class AttributeInfo {
 public:
  AttributeInfo()                                = default;
  AttributeInfo(const AttributeInfo&)            = default;
  AttributeInfo(AttributeInfo&&)                 = default;
  AttributeInfo& operator=(const AttributeInfo&) = default;
  AttributeInfo& operator=(AttributeInfo&&)      = default;
  virtual ~AttributeInfo()                       = default;

  /// @brief Parse the attribute-specific data from the class file.
  virtual void readInfo(ClassFileParser& parser) = 0;

  U2          name_index{};  ///< Index into constant pool for the attribute name.
  U4          length{};      ///< Length of attribute data (excluding the header).
  std::string name;          ///< Resolved attribute name string.
};

/**
 * @brief Holds an ordered collection of attributes.
 *
 * Owns the AttributeInfo objects via unique_ptr. Provides
 * typed lookup via getAttribute<T>().
 */
class AttributeTable {
 public:
  AttributeTable() = default;
  explicit AttributeTable(std::vector<std::unique_ptr<AttributeInfo>>&& attributes)
    : attributes(std::move(attributes)) {}
  AttributeTable(const AttributeTable&)            = delete;
  AttributeTable(AttributeTable&&)                 = default;
  AttributeTable& operator=(const AttributeTable&) = delete;
  AttributeTable& operator=(AttributeTable&&)      = default;
  ~AttributeTable()                                = default;

  /// @brief Find an attribute of the specified type.
  /// @tparam T The attribute subclass to look for.
  /// @return Pointer to the attribute, or nullptr if not found.
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

/**
 * @brief Fallback for unrecognized attribute types.
 *
 * Stores the raw bytes of the attribute as a vector of U1.
 */
class GenericAttribute : public AttributeInfo {
 public:
  void            readInfo(ClassFileParser& parser) override;
  std::vector<U1> info;  ///< Raw attribute bytes.
};

/**
 * @brief ConstantValue attribute (JVM §4.7.2).
 *
 * Indicates a constant value for a static final field.
 */
class ConstantValueAttribute : public AttributeInfo {
 public:
  void readInfo(ClassFileParser& parser) override;
  U2   value;  ///< Index into constant pool for the constant value.
};

/**
 * @brief Exceptions attribute (JVM §4.7.5).
 *
 * Lists the checked exceptions a method may throw.
 */
class ExceptionsAttribute : public AttributeInfo {
 public:
  void            readInfo(ClassFileParser& parser) override;
  U2              num_exceptions;  ///< Number of exception class indices.
  std::vector<U2> exceptions;      ///< Indices into constant pool for exception classes.
};

/// A single entry in the BootstrapMethods attribute (JVM §4.7.23).
struct BootstrapMethod {
  U2              method_ref;     ///< Index of the method handle.
  U2              num_arguments;  ///< Number of bootstrap arguments.
  std::vector<U2> arguments;      ///< Indices of bootstrap argument constants.
};

/**
 * @brief BootstrapMethods attribute (JVM §4.7.23).
 *
 * Used by invokedynamic to store bootstrap method references.
 */
class BootstrapMethodsAttribute : public AttributeInfo {
 public:
  void                         readInfo(ClassFileParser& parser) override;
  std::vector<BootstrapMethod> bootstrap_methods;
  U2                           num_bootstrap_methods;
};

/// A single entry in the Code attribute's exception table (JVM §4.7.3).
struct ExceptionTableEntry {
  U2 start_pc;    ///< Inclusive start of try block.
  U2 end_pc;      ///< Exclusive end of try block.
  U2 handler_pc;  ///< Start of handler code.
  U2 catch_type;  ///< Index of catch class (0 = finally/any).
};

/**
 * @brief Code attribute (JVM §4.7.3).
 *
 * Contains the bytecode, exception table, and nested attributes
 * for a single method.
 */
class CodeAttribute : public AttributeInfo {
 public:
  void readInfo(ClassFileParser& parser) override;

  U2 max_stack;   ///< Maximum depth of the operand stack.
  U2 max_locals;  ///< Number of local variable slots.

  U4              code_length;  ///< Length of the bytecode array.
  std::vector<U1> code;         ///< The bytecode instructions.

  U2                               num_exception_table;  ///< Number of exception table entries.
  std::vector<ExceptionTableEntry> exception_table;      ///< Exception handler table.

  AttributeTable attributes;  ///< Nested attributes (LineNumberTable, etc.).
};

}  // namespace jvm::classfile