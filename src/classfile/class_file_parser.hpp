/**
 * @file class_file_parser.h
 * @brief Parser for Java class files (.class).
 *
 * Reads a binary .class file stream and produces a ClassFile object.
 * Implements the parsing logic specified in JVM Specification §4.
 *
 */
#pragma once

#include <memory>

#include "byte_reader.hpp"
#include "class_file.hpp"

namespace jvm::classfile {

/**
 * @brief Parser for binary class file data.
 *
 * Takes a byte span, sequentially reads the class file structures
 * (magic, version, constant pool, access flags, this/super class,
 * interfaces, fields, methods, attributes), and produces a
 * ClassFile value.
 */
class ClassFileParser {
 public:
  /**
   * @brief Construct a new ClassFileParser object.
   * @param data The raw class file bytes (not owned by the parser).
   */
  explicit ClassFileParser(std::span<U1> data) : reader_(data) {}

  /**
   * @brief Parse the class file data into a ClassFile structure.
   * @return std::unique_ptr<ClassFile> The parsed result.
   */
  std::unique_ptr<ClassFile> parse();

  /// @brief Access the underlying byte reader (e.g. for nested attribute parsing).
  ByteReader& getReader() { return reader_; }
  /// @brief Access the parsed constant pool (valid after parseConstantPool is called).
  const ConstantPool& getConstantPool() { return *constant_pool_ref_; }

 private:
  /// @name Parsing Steps (called in sequence by parse())
  ///@{
  void                      parseMagic();
  Version                   parseVersion();
  ConstantPool              parseConstantPool();
  AccessFlags<flags::Class> parseAccessFlags();
  MemberTable               parseFields();
  MemberTable               parseMethods();
  AttributeTable            parseAttributes();
  ///@}

  /// @name Factory Methods for Individual Entries
  ///@{
  std::unique_ptr<ConstantInfo>  createConstantInfo();
  std::unique_ptr<AttributeInfo> createAttributeInfo();
  std::unique_ptr<FieldInfo>     createFieldInfo();
  std::unique_ptr<MethodInfo>    createMethodInfo();
  ///@}

  ByteReader          reader_;  ///< Stateful byte reader (tracks position).
  const ConstantPool* constant_pool_ref_{
    nullptr};  ///< Pointer to parsed constant pool (for attribute parsing).

  friend class CodeAttribute;
  friend class FieldInfo;
  friend class MethodInfo;
};

}  // namespace jvm::classfile