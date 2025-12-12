#pragma once

#include "attributes.hpp"
#include "common/access_flags.hpp"
#include "common/types.h"

namespace class_loader {

class ByteReader;

// Base class for fields and methods
class MemberInfo {
 public:
  virtual ~MemberInfo()                          = default;
  virtual void readInfo(ClassFileParser& parser) = 0;

  common::U2     name_index;
  common::U2     descriptor_index;
  AttributeTable attributes;

  std::string name;
};

class FieldInfo : public MemberInfo {
 public:
  // This holds flags specific to fields
  common::AccessFlags<common::flags::Field> access_flags;

  void readInfo(ClassFileParser& parser) override;
};

class MethodInfo : public MemberInfo {
 public:
  // This holds flags specific to methods
  common::AccessFlags<common::flags::Method> access_flags;

  void readInfo(ClassFileParser& parser) override;
};

class MemberTable {
 public:
  MemberTable(std::vector<std::unique_ptr<MemberInfo>>&& members) : members(std::move(members)) {}
  MemberTable(const MemberTable&)            = delete;
  MemberTable(MemberTable&&)                 = default;
  MemberTable& operator=(const MemberTable&) = delete;
  MemberTable& operator=(MemberTable&&)      = default;
  ~MemberTable()                             = default;
  std::vector<std::unique_ptr<MemberInfo>>& getMembers() { return members; }

 private:
  std::vector<std::unique_ptr<MemberInfo>> members;
};

}  // namespace class_loader