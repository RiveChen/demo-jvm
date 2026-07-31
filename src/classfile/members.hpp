/**
 * @file members.hpp
 * @brief Class file field and method member representations.
 *
 * Implements the field_info and method_info structures from
 * JVM Specification §4.5 and §4.6, including MemberTable
 * for holding collections of either type.
 */

#pragma once

#include "attributes.hpp"
#include "utilities/access_flags.hpp"
#include "utilities/types.hpp"

namespace jvm::classfile {

class ByteReader;

/**
 * @brief Base class for both field and method members in a class file.
 *
 * Each member has a name, descriptor (type signature), and attributes.
 * The `name` string is resolved from the constant pool after parsing.
 */
class MemberInfo {
 public:
  MemberInfo()                             = default;
  MemberInfo(const MemberInfo&)            = delete;
  MemberInfo(MemberInfo&&)                 = default;
  MemberInfo& operator=(const MemberInfo&) = delete;
  MemberInfo& operator=(MemberInfo&&)      = default;
  virtual ~MemberInfo()                    = default;

  /// @brief Read this member's data from the class file parser.
  virtual void readInfo(ClassFileParser& parser) = 0;

  U2             name_index{};        ///< Index into constant pool for the member name.
  U2             descriptor_index{};  ///< Index into constant pool for the descriptor.
  AttributeTable attributes;          ///< Member-level attributes (e.g. Code, ConstantValue).
  std::string    name;                ///< Resolved member name string.
};

/**
 * @brief Represents a field in the class file (field_info, JVM §4.5).
 */
class FieldInfo : public MemberInfo {
 public:
  AccessFlags<flags::Field> access_flags;  ///< Field-specific access flags.

  void readInfo(ClassFileParser& parser) override;
};

/**
 * @brief Represents a method in the class file (method_info, JVM §4.6).
 */
class MethodInfo : public MemberInfo {
 public:
  AccessFlags<flags::Method> access_flags;  ///< Method-specific access flags.

  void readInfo(ClassFileParser& parser) override;
};

/**
 * @brief Holds a collection of member (field or method) definitions.
 *
 * Owns the MemberInfo objects via unique_ptr.
 */
class MemberTable {
 public:
  explicit MemberTable(std::vector<std::unique_ptr<MemberInfo>>&& members)
    : members(std::move(members)) {}
  MemberTable(const MemberTable&)            = delete;
  MemberTable(MemberTable&&)                 = default;
  MemberTable& operator=(const MemberTable&) = delete;
  MemberTable& operator=(MemberTable&&)      = default;
  ~MemberTable()                             = default;
  std::vector<std::unique_ptr<MemberInfo>>& getMembers() { return members; }

 private:
  std::vector<std::unique_ptr<MemberInfo>> members;
};

}  // namespace jvm::classfile