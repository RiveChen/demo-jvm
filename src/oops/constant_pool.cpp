#include "constant_pool.h"

#include <stdexcept>
#include <variant>

#include "classfile/class_loader.h"
#include "klass.h"

namespace jvm::oops {

Klass* RuntimeConstantPool::resolveClass(U2 index) {
  auto& slot = infos_.at(index);
  if (auto* klass = std::get_if<Klass*>(&slot)) {
    return *klass;
  }

  auto* sym_ref = std::get_if<SymRef_Class>(&slot);
  if (sym_ref == nullptr) {
    throw std::runtime_error("Invalid symbol reference");
  }

  // class_name is already baked to dot-form at prepare time
  Klass* k = owner_klass_->getClassLoader()->loadClass(sym_ref->class_name);
  slot     = k;
  return k;
}

Field* RuntimeConstantPool::resolveField(U2 index) {
  auto& slot = infos_.at(index);

  if (auto* field = std::get_if<Field*>(&slot)) {
    // if is already resolved, return
    return *field;
  }

  // otherwise, resolve it
  auto* sym_ref = std::get_if<SymRef_Field>(&slot);
  if (sym_ref == nullptr) {
    throw std::runtime_error("Invalid symbol reference");
  }

  Klass* target_klass = this->resolveClass(sym_ref->class_cp_index);

  Field* resolved_field = target_klass->findField(sym_ref->member_name, sym_ref->descriptor);
  slot                  = resolved_field;
  return resolved_field;
}

Method* RuntimeConstantPool::resolveMethod(U2 index) {
  auto& slot = infos_.at(index);

  if (auto* method = std::get_if<Method*>(&slot)) {
    return *method;
  }

  auto* sym_ref = std::get_if<SymRef_Method>(&slot);
  if (sym_ref == nullptr) {
    throw std::runtime_error("Invalid symbol reference");
  }

  Klass* target_klass = this->resolveClass(sym_ref->class_cp_index);

  Method* resolved_method = target_klass->findMethod(sym_ref->member_name, sym_ref->descriptor);
  slot                    = resolved_method;
  return resolved_method;
}

}  // namespace jvm::oops