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

std::optional<std::string> RuntimeConstantPool::symbolicKey(U2 index) {
  const auto& slot  = infos_.at(index);
  auto        build = [&](U2 class_cp, const std::string& m,
                   const std::string& d) -> std::optional<std::string> {
    // the target Class may already be resolved (Klass*) by another ref to the same
    // class -> then this ref's class is loaded, so it's never a stub candidate.
    const auto* c = std::get_if<SymRef_Class>(&infos_.at(class_cp));
    if (c == nullptr) {
      return std::nullopt;
    }
    return c->class_name + '.' + m + ' ' + d;
  };
  if (const auto* mr = std::get_if<SymRef_Method>(&slot)) {
    return build(mr->class_cp_index, mr->member_name, mr->descriptor);
  }
  if (const auto* fr = std::get_if<SymRef_Field>(&slot)) {
    return build(fr->class_cp_index, fr->member_name, fr->descriptor);
  }
  return std::nullopt;
}

}  // namespace jvm::oops