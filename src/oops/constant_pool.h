#pragma once

#include <string>
#include <vector>

#include "utilities/types.h"

namespace jvm::oops {

// 4 types of symbol references, not resolved yet
// using string, not U2 index, in order to remove the dependency for classfile
struct SymRef_Class {
  std::string class_name;
};
struct SymRef_Field {
  U2          class_cp_index;
  std::string member_name;
  std::string descriptor;
};
struct SymRef_Method {
  U2          class_cp_index;
  std::string member_name;
  std::string descriptor;
};
struct SymRef_InterfaceMethod {
  U2          class_cp_index;
  std::string member_name;
  std::string descriptor;
};

class Klass;
class Method;
class Field;

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

  void     setConstant(U2 index, RtCpInfo info) { infos_.at(index) = std::move(info); }
  RtCpInfo getConstant(U2 index) { return infos_.at(index); }

  Klass*  resolveClass(U2 index);
  Field*  resolveField(U2 index);
  Method* resolveMethod(U2 index);

 private:
  std::vector<RtCpInfo> infos_;
  Klass*                owner_klass_;

  friend class Klass;
};

}  // namespace jvm::oops