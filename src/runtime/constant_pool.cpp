#include "constant_pool.h"

#include <iostream>

#include "class_loader/class_file.h"
#include "class_loader/class_loader.h"
#include "klass.h"
namespace jvm::runtime {

Klass* RuntimeConstantPool::resolveClass(U2 index) {
  auto& slot = infos_[index];

  if (auto* klass = std::get_if<Klass*>(&slot)) {
    return *klass;
  }

  auto* sym_ref = std::get_if<SymRef_Class>(&slot);
  if (sym_ref == nullptr) {
    throw std::runtime_error("Invalid symbol reference");
  }

  const auto& class_cp   = owner_klass_->getClassFile()->constant_pool;
  auto        class_name = class_cp.getUtf8String(sym_ref->name_index);
  // replace '/' with '.'
  std::replace(class_name.begin(), class_name.end(), '/', '.');
  auto* resolved_klass = owner_klass_->getClassLoader()->loadClass(class_name);
  slot                 = resolved_klass;
  return resolved_klass;
}

Field* RuntimeConstantPool::resolveField(U2 index) {
  auto& slot = infos_[index];

  if (auto* field = std::get_if<Field*>(&slot)) {
    // if is already resolved, return
    return *field;
  }

  // otherwise, resolve it
  auto* sym_ref = std::get_if<SymRef_Field>(&slot);
  if (sym_ref == nullptr) {
    throw std::runtime_error("Invalid symbol reference");
  }

  Klass* target_klass = this->resolveClass(sym_ref->class_index);

  auto [name, descriptor] = this->resolveNameAndType(sym_ref->name_and_type_index);

  Field* resolved_field = target_klass->findField(name, descriptor);
  slot                  = resolved_field;
  return resolved_field;
}

Method* RuntimeConstantPool::resolveMethod(U2 index) {
  auto& slot = infos_[index];

  if (auto* method = std::get_if<Method*>(&slot)) {
    return *method;
  }

  auto* sym_ref = std::get_if<SymRef_Method>(&slot);
  if (sym_ref == nullptr) {
    throw std::runtime_error("Invalid symbol reference");
  }

  Klass* target_klass = this->resolveClass(sym_ref->class_index);

  auto [name, descriptor] = this->resolveNameAndType(sym_ref->name_and_type_index);

  Method* resolved_method = target_klass->findMethod(name, descriptor);
  slot                    = resolved_method;
  return resolved_method;
}

std::pair<std::string, std::string> RuntimeConstantPool::resolveNameAndType(U2 index) {
  const auto& cf_cp = owner_klass_->getClassFile()->constant_pool;
  const auto* nt_info =
    dynamic_cast<const class_loader::NameAndTypeInfo*>(cf_cp.getConstantInfo(index));
  if (nt_info == nullptr) {
    throw std::runtime_error("Invalid name and type info");
  }

  std::string name       = cf_cp.getUtf8String(nt_info->name_index);
  std::string descriptor = cf_cp.getUtf8String(nt_info->descriptor_index);

  return std::make_pair(name, descriptor);
}

}  // namespace jvm::runtime