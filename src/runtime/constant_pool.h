#pragma once

#include <string>
#include <vector>

#include "common/types.h"

namespace jvm::runtime {

class Klass;
class Method;
class Field;

// 4 types of symbol references, not resolved yet
struct SymRef_Class {
  U2 name_index;
};
struct SymRef_Field {
  U2 class_index;
  U2 name_and_type_index;
};
struct SymRef_Method {
  U2 class_index;
  U2 name_and_type_index;
};
struct SymRef_InterfaceMethod {
  U2 class_index;
  U2 name_and_type_index;
};

using RtCpInfo = std::variant<std::monostate,                // empty
                              Jint, Jfloat, Jlong, Jdouble,  // literal
                              std::string,              // string literal, optimize: string intern
                              Klass*, Method*, Field*,  // resolved
                              SymRef_Class, SymRef_Field, SymRef_Method,
                              SymRef_InterfaceMethod  // symbol references, not resolved yet
                              >;

class RuntimeConstantPool {
 public:
  explicit RuntimeConstantPool(Klass* owner_klass) : owner_klass_(owner_klass) {}

  void     setConstant(U2 index, RtCpInfo info) { infos_[index] = std::move(info); }
  RtCpInfo getConstant(U2 index) { return infos_[index]; }

  Klass*                              resolveClass(U2 index);
  Field*                              resolveField(U2 index);
  Method*                             resolveMethod(U2 index);
  std::pair<std::string, std::string> resolveNameAndType(U2 index);

 private:
  std::vector<RtCpInfo> infos_;
  Klass*                owner_klass_;

  friend class Klass;
};

}  // namespace jvm::runtime