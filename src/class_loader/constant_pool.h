/**
 * @file constant_pool.h
 * @author Rive Chen
 * @brief Constant pool and its entries
 * @version 0.1
 * @date 2025-12-12
 *
 * @copyright Copyright (c) 2025
 *
 */
#pragma once

#include <memory>
#include <string>
#include <vector>

#include "common/types.h"

namespace jvm::class_loader {

class ByteReader;

// JVM Spec §4.4. The Constant Pool
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

// Abstract base class for all constant pool entries
class ConstantInfo {
 public:
  ConstantInfo()                               = default;
  ConstantInfo(const ConstantInfo&)            = default;
  ConstantInfo(ConstantInfo&&)                 = default;
  ConstantInfo& operator=(const ConstantInfo&) = default;
  ConstantInfo& operator=(ConstantInfo&&)      = default;
  virtual ~ConstantInfo()                      = default;
  // Each concrete type must know how to read its own data from the stream
  virtual void readInfo(ByteReader& reader) = 0;

  ConstantTag tag{};
};

// CONSTANT_Utf8_info
class Utf8Info : public ConstantInfo {
 public:
  void        readInfo(ByteReader& reader) override;
  std::string value;
};

// CONSTANT_Integer_info
class IntegerInfo : public ConstantInfo {
 public:
  void readInfo(ByteReader& reader) override;
  Jint value;
};

// CONSTANT_Float_info
class FloatInfo : public ConstantInfo {
 public:
  void   readInfo(ByteReader& reader) override;
  Jfloat value;
};

// CONSTANT_Long_info
class LongInfo : public ConstantInfo {
 public:
  void  readInfo(ByteReader& reader) override;
  Jlong value;
};

// CONSTANT_Double_info
class DoubleInfo : public ConstantInfo {
 public:
  void    readInfo(ByteReader& reader) override;
  Jdouble value;
};

// CONSTANT_Class_info
class ClassInfo : public ConstantInfo {
 public:
  void readInfo(ByteReader& reader) override;
  U2   name_index;
};

// CONSTANT_String_info
class StringInfo : public ConstantInfo {
 public:
  void readInfo(ByteReader& reader) override;
  U2   string_index;
};

// A common base for Fieldref, Methodref, and InterfaceMethodref
class MemberrefInfo : public ConstantInfo {
 public:
  void readInfo(ByteReader& reader) override;
  U2   class_index;
  U2   name_and_type_index;
};

class FieldrefInfo : public MemberrefInfo {};
class MethodrefInfo : public MemberrefInfo {};
class InterfaceMethodrefInfo : public MemberrefInfo {};

// CONSTANT_NameAndType_info
class NameAndTypeInfo : public ConstantInfo {
 public:
  void readInfo(ByteReader& reader) override;
  U2   name_index;
  U2   descriptor_index;
};

// CONSTANT_MethodHandle_info
class MethodHandleInfo : public ConstantInfo {
 public:
  void readInfo(ByteReader& reader) override;
  U1   reference_kind;
  U2   reference_index;
};

// CONSTANT_MethodType_info
class MethodTypeInfo : public ConstantInfo {
 public:
  void readInfo(ByteReader& reader) override;
  U2   descriptor_index;
};

// CONSTANT_InvokeDynamic_info
class InvokeDynamicInfo : public ConstantInfo {
 public:
  void readInfo(ByteReader& reader) override;
  U2   bootstrap_method_attr_index;
  U2   name_and_type_index;
};

class ConstantPool {
 public:
  // Takes ownership of the parsed constant info vector
  explicit ConstantPool(std::vector<std::unique_ptr<ConstantInfo>>&& pool)
    : pool_(std::move(pool)) {}
  ConstantPool(const ConstantPool&)            = delete;
  ConstantPool(ConstantPool&&)                 = default;
  ConstantPool& operator=(const ConstantPool&) = delete;
  ConstantPool& operator=(ConstantPool&&)      = default;
  ~ConstantPool()                              = default;

  /**
   * @brief Get a UTF-8 string from a utf8 info entry in the constant pool
   * @param index Index of the utf8 info entry in the constant pool
   * @return std::string
   */
  std::string getUtf8String(U2 index) const;

  /**
   * @brief Get a class name from a class info entry pointing to a utf8 info entry
   * @param class_index Index of the class info entry in the constant pool
   * @return std::string
   */
  std::string getClassName(U2 class_index) const;

  /**
   * @brief Get a name and descriptor from a name and type info entry pointing to two utf8 info
   * entries
   * @param index Index of the name and type info entry in the constant pool
   * @return std::pair<std::string, std::string>
   */
  std::pair<std::string, std::string> getNameAndType(U2 index) const;

  /**
   * @brief Get a constant info entry from the constant pool
   * @param index Index of the constant info entry in the constant pool
   * @return const ConstantInfo*
   */
  const ConstantInfo* getConstantInfo(U2 index) const;

  /**
   * @brief Get the size of the constant pool, including the placeholder at index 0
   * @return size_t
   */
  size_t size() const { return pool_.size(); }

 private:
  std::vector<std::unique_ptr<ConstantInfo>> pool_;
};

}  // namespace jvm::class_loader
