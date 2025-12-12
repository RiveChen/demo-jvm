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

#include "attributes.hpp"
#include "common/access_flags.hpp"
#include "common/types.h"
#include "constant_pool.h"
#include "members.h"
#include "version.h"

namespace class_loader {

constexpr common::U4 kClassFileMagic = 0xCAFEBABE;

class ClassFile {
 public:
  ClassFile(Version version, ConstantPool constant_pool,
            common::AccessFlags<common::flags::Class> access_flags, common::U2 this_class_index,
            common::U2 super_class_index, common::U2 interfaces_count,
            std::vector<common::U2> interfaces, MemberTable fields, MemberTable methods,
            AttributeTable attributes)
    : version(std::move(version)),
      constant_pool(std::move(constant_pool)),
      access_flags(std::move(access_flags)),
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

  Version                                   version;
  ConstantPool                              constant_pool;
  common::AccessFlags<common::flags::Class> access_flags;
  common::U2                                this_class_index;
  common::U2                                super_class_index;
  common::U2                                interfaces_count;
  std::vector<common::U2>                   interfaces;
  MemberTable                               fields;
  MemberTable                               methods;
  AttributeTable                            attributes;
};

}  // namespace class_loader