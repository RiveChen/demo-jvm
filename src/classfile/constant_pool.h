/**
 * @file constant_pool.h
 * @brief Constant pool entries and their container for parsed class files.
 *
 * Implements the constant pool structures defined in JVM Specification §4.4.
 * Each constant pool entry type (CONSTANT_Utf8, CONSTANT_Class, etc.) is
 * represented by a subclass of ConstantInfo.
 *
 */
#pragma once

#include <memory>
#include <string>
#include <vector>

#include "utilities/types.h"

namespace jvm::classfile {

class ByteReader;

/// @name JVM Constant Pool Tags (JVM Spec §4.4, Table 4.4-A)
///@{
enum class ConstantTag : U1 {
  kUtf8               = 1,
  kInteger            = 3,
  kFloat              = 4,
  kLong               = 5,
  kDouble             = 6,
  kClass              = 7,
  kString             = 8,
  kFieldref           = 9,
  kMethodref          = 10,
  kInterfaceMethodref = 11,
  kNameAndType        = 12,
  kMethodHandle       = 15,
  kMethodType         = 16,
  kInvokeDynamic      = 18,
};
///@}

/**
 * @brief Abstract base class for all constant pool entries.
 *
 * Each concrete subclass knows how to read its own data from a ByteReader.
 */
class ConstantInfo {
 public:
  ConstantInfo()                               = default;
  ConstantInfo(const ConstantInfo&)            = default;
  ConstantInfo(ConstantInfo&&)                 = default;
  ConstantInfo& operator=(const ConstantInfo&) = default;
  ConstantInfo& operator=(ConstantInfo&&)      = default;
  virtual ~ConstantInfo()                      = default;

  /// @brief Read this entry's data from the byte stream.
  virtual void readInfo(ByteReader& reader) = 0;

  ConstantTag tag{};  ///< Constant pool tag identifying the entry type.
};

/// CONSTANT_Utf8_info (JVM Spec §4.4.7)
class Utf8Info : public ConstantInfo {
 public:
  void        readInfo(ByteReader& reader) override;
  std::string value;  ///< Decoded UTF-8 string value.
};

/// CONSTANT_Integer_info (JVM Spec §4.4.4)
class IntegerInfo : public ConstantInfo {
 public:
  void readInfo(ByteReader& reader) override;
  Jint value;
};

/// CONSTANT_Float_info (JVM Spec §4.4.4)
class FloatInfo : public ConstantInfo {
 public:
  void   readInfo(ByteReader& reader) override;
  Jfloat value;
};

/// CONSTANT_Long_info (JVM Spec §4.4.5)
class LongInfo : public ConstantInfo {
 public:
  void  readInfo(ByteReader& reader) override;
  Jlong value;
};

/// CONSTANT_Double_info (JVM Spec §4.4.5)
class DoubleInfo : public ConstantInfo {
 public:
  void    readInfo(ByteReader& reader) override;
  Jdouble value;
};

/// CONSTANT_Class_info (JVM Spec §4.4.1)
class ClassInfo : public ConstantInfo {
 public:
  void readInfo(ByteReader& reader) override;
  U2   name_index;  ///< Index into constant pool for the class name (Utf8).
};

/// CONSTANT_String_info (JVM Spec §4.4.3)
class StringInfo : public ConstantInfo {
 public:
  void readInfo(ByteReader& reader) override;
  U2   string_index;  ///< Index into constant pool for the string value (Utf8).
};

/**
 * @brief Common base for Fieldref, Methodref, and InterfaceMethodref.
 *  (JVM Spec §4.4.2)
 */
class MemberrefInfo : public ConstantInfo {
 public:
  void readInfo(ByteReader& reader) override;
  U2   class_index;          ///< Index of the class entry.
  U2   name_and_type_index;  ///< Index of the name-and-type entry.
};

/// CONSTANT_Fieldref_info
class FieldrefInfo : public MemberrefInfo {};
/// CONSTANT_Methodref_info
class MethodrefInfo : public MemberrefInfo {};
/// CONSTANT_InterfaceMethodref_info
class InterfaceMethodrefInfo : public MemberrefInfo {};

/// CONSTANT_NameAndType_info (JVM Spec §4.4.6)
class NameAndTypeInfo : public ConstantInfo {
 public:
  void readInfo(ByteReader& reader) override;
  U2   name_index;        ///< Index for the name (Utf8).
  U2   descriptor_index;  ///< Index for the descriptor (Utf8).
};

/// CONSTANT_MethodHandle_info (JVM Spec §4.4.8)
class MethodHandleInfo : public ConstantInfo {
 public:
  void readInfo(ByteReader& reader) override;
  U1   reference_kind;   ///< Kind of method handle (1-9).
  U2   reference_index;  ///< Index into constant pool.
};

/// CONSTANT_MethodType_info (JVM Spec §4.4.9)
class MethodTypeInfo : public ConstantInfo {
 public:
  void readInfo(ByteReader& reader) override;
  U2   descriptor_index;  ///< Index for the method descriptor (Utf8).
};

/// CONSTANT_InvokeDynamic_info (JVM Spec §4.4.10)
class InvokeDynamicInfo : public ConstantInfo {
 public:
  void readInfo(ByteReader& reader) override;
  U2   bootstrap_method_attr_index;  ///< Index into bootstrap methods table.
  U2   name_and_type_index;          ///< Index of the name-and-type entry.
};

/**
 * @brief Container for all constant pool entries in a class file.
 *
 * Owns the ConstantInfo objects via unique_ptr. Index 0 is reserved
 * and invalid (JVM convention). Provides lookup helpers for common
 * access patterns (getUtf8String, getClassName, getNameAndType).
 */
class ConstantPool {
 public:
  explicit ConstantPool(std::vector<std::unique_ptr<ConstantInfo>>&& pool)
    : pool_(std::move(pool)) {}
  ConstantPool(const ConstantPool&)            = delete;
  ConstantPool(ConstantPool&&)                 = default;
  ConstantPool& operator=(const ConstantPool&) = delete;
  ConstantPool& operator=(ConstantPool&&)      = default;
  ~ConstantPool()                              = default;

  /// @brief Get a UTF-8 string from a Utf8Info entry.
  /// @param index 1-based constant pool index.
  std::string getUtf8String(U2 index) const;

  /// @brief Get a class name from a ClassInfo entry.
  /// @param class_index Index of the ClassInfo entry.
  std::string getClassName(U2 class_index) const;

  /// @brief Get (name, descriptor) from a NameAndTypeInfo entry.
  std::pair<std::string, std::string> getNameAndType(U2 index) const;

  /// @brief Get a raw ConstantInfo pointer at the given index.
  const ConstantInfo* getConstantInfo(U2 index) const;

  /// @brief Return the pool size (including the placeholder at index 0).
  size_t size() const { return pool_.size(); }

 private:
  std::vector<std::unique_ptr<ConstantInfo>> pool_;
};

}  // namespace jvm::classfile