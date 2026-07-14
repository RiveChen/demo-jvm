/**
 * @file class_file.h
 * @brief Data structure representing a parsed class file.
 *
 * Holds all data parsed from a .class file, including version info,
 * constant pool, access flags, interfaces, fields, methods, and attributes.
 *
 */
#pragma once

#include "attributes.hpp"
#include "constant_pool.hpp"
#include "members.hpp"
#include "utilities/access_flags.hpp"
#include "utilities/types.hpp"
#include "version.hpp"

namespace jvm::classfile {

/// Magic number identifying a valid class file (JVM Spec §4.1).
constexpr U4 kClassFileMagic = 0xCAFEBABE;

/**
 * @brief In-memory representation of a parsed .class file.
 *
 * All fields are public for direct access during class loading.
 * Instances are created by ClassFileParser::parse().
 */
class ClassFile {
 public:
  ClassFile(Version version, ConstantPool constant_pool, AccessFlags<flags::Class> access_flags,
            U2 this_class_index, U2 super_class_index, U2 interfaces_count,
            std::vector<U2> interfaces, MemberTable fields, MemberTable methods,
            AttributeTable attributes)
    : version(std::move(version)),
      constant_pool(std::move(constant_pool)),
      access_flags(access_flags),
      this_class_index(this_class_index),
      super_class_index(super_class_index),
      interfaces_count(interfaces_count),
      interfaces(std::move(interfaces)),
      fields(std::move(fields)),
      methods(std::move(methods)),
      attributes(std::move(attributes)) {}
  ClassFile(const ClassFile&)            = delete;
  ClassFile(ClassFile&&)                 = default;
  ClassFile& operator=(const ClassFile&) = delete;
  ClassFile& operator=(ClassFile&&)      = default;
  ~ClassFile()                           = default;

  Version                   version;            ///< Class file version (major.minor)
  ConstantPool              constant_pool;      ///< Constant pool entries (JVM §4.4)
  AccessFlags<flags::Class> access_flags;       ///< Class access and property modifiers (JVM §4.1)
  U2                        this_class_index;   ///< Index of this class name in the constant pool
  U2                        super_class_index;  ///< Index of super class name in the constant pool
  U2                        interfaces_count;   ///< Number of direct interfaces
  std::vector<U2>           interfaces;         ///< Indices of interface class entries
  MemberTable               fields;             ///< Field table (JVM §4.5)
  MemberTable               methods;            ///< Method table (JVM §4.6)
  AttributeTable            attributes;         ///< Class-level attributes (JVM §4.7)
};

}  // namespace jvm::classfile