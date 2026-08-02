/**
 * @file constant_pool.cpp
 * @brief Runtime constant pool resolution (class, field, method references).
 */

#include "constant_pool.hpp"

#include "classfile/class_loader.hpp"
#include "klass.hpp"
#include "oops/method_area.hpp"
#include "utilities/basic_type.hpp"

#include <cassert>
#include <cstddef>
#include <stdexcept>
#include <variant>

namespace jvm::oops {

// file-local helper — resolves array names like "[I", "[[Ljava/lang/String;"
static Klass* arrayKlassForName(classfile::ClassLoader* loader, const std::string& name) {
  if (name[0] != '[') {
    return nullptr;
  }

  size_t dim = 0;
  while (name[dim] == '[') {
    dim++;
  }

  ArrayKlass* res = nullptr;
  if (name[dim] == 'L') {
    // remove 'L' and ';'
    auto* klass =
      static_cast<InstanceKlass*>(loader->loadClass(name.substr(dim + 1, name.size() - dim - 2)));
    res = oops::MethodArea::getSingleton().getOrCreateObjArrayKlass(klass);
  } else {
    auto type = charToBasicType(name[dim]);
    res       = oops::MethodArea::getSingleton().getOrCreateTypeArrayKlass(type);
  }

  while (--dim != 0) {
    res = oops::MethodArea::getSingleton().getOrCreateObjArrayKlass(res);
  }
  return res;
}

Klass* RuntimeConstantPool::resolveClass(U2 index) {
  auto& slot = infos_.at(index);
  if (auto* klass = std::get_if<Klass*>(&slot)) {
    return *klass;
  }

  auto* sym_ref = std::get_if<SymRef_Class>(&slot);
  if (sym_ref == nullptr) {
    throw std::runtime_error("Invalid symbol reference");
  }
  if (sym_ref->class_name[0] == '[') {
    Klass* k = arrayKlassForName(owner_klass_->getClassLoader(), sym_ref->class_name);
    slot     = k;
    return k;
  }

  // class_name is already baked to slash-form at prepare time
  InstanceKlass* k = owner_klass_->getClassLoader()->loadClass(sym_ref->class_name);
  slot             = k;
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

  auto* target_klass = static_cast<InstanceKlass*>(this->resolveClass(sym_ref->class_cp_index));
  assert(target_klass->kind() == Klass::Kind::Instance);

  Field* resolved_field = target_klass->findField(sym_ref->member_name, sym_ref->descriptor);
  slot                  = resolved_field;
  return resolved_field;
}

Method* RuntimeConstantPool::resolveMethod(U2 index) {
  auto& slot = infos_.at(index);

  if (auto* method = std::get_if<Method*>(&slot)) {
    return *method;
  }

  if (auto* sym_ref = std::get_if<SymRef_Method>(&slot)) {
    auto* target = static_cast<InstanceKlass*>(resolveClass(sym_ref->class_cp_index));
    assert(target->kind() == Klass::Kind::Instance);
    Method* m = target->findMethod(sym_ref->member_name, sym_ref->descriptor);
    slot      = m;
    return m;
  }

  if (auto* im_ref = std::get_if<SymRef_InterfaceMethod>(&slot)) {
    auto* target = static_cast<InstanceKlass*>(resolveClass(im_ref->class_cp_index));
    assert(target->kind() == Klass::Kind::Instance);
    Method* m = target->findMethod(im_ref->member_name, im_ref->descriptor);
    slot      = m;
    return m;
  }

  throw std::runtime_error("Invalid method symbol reference");
}

std::optional<std::string> RuntimeConstantPool::symbolicKey(U2 index) {
  const auto& slot  = infos_.at(index);
  auto        build = [&](U2 class_cp, const std::string& m,
                          const std::string& d) -> std::optional<std::string> {
    // the target Class may already be resolved (Klass*) by another ref to the
    // same class -> then this ref's class is loaded, so it's never a stub
    // candidate.
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