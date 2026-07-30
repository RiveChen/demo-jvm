/**
 * @file method_area.cpp
 * @brief Method area singleton storage for loaded classes.
 */

#include "method_area.hpp"

#include "oops/klass.hpp"
#include "utilities/basic_type.hpp"

#include <memory>
#include <utility>

namespace jvm::oops {

void MethodArea::addClass(ClassIdentifier identifier, ClassData class_data) {
  instance_klasses_.emplace(std::move(identifier), std::move(class_data));
}

InstanceKlass* MethodArea::getClass(const ClassIdentifier& identifier) {
  auto it = instance_klasses_.find(identifier);
  if (it != instance_klasses_.end()) {
    return it->second.first.get();
  }
  return nullptr;
}

TypeArrayKlass* MethodArea::getOrCreateTypeArrayKlass(BasicType type) {
  std::string name = basicTypeToArrayName(type);
  auto        it   = array_klasses_.find(name);
  if (it != array_klasses_.end()) {
    return static_cast<TypeArrayKlass*>(it->second.get());
  }
  Jint  elem_size = basicTypeElementSize(type);
  auto  klass     = std::make_unique<TypeArrayKlass>(type, elem_size, name);
  auto* klass_ptr = static_cast<TypeArrayKlass*>(klass.get());
  array_klasses_.emplace(name, std::move(klass));
  return klass_ptr;
}

ObjArrayKlass* MethodArea::getOrCreateObjArrayKlass(Klass* component) {
  std::string name = "[" + component->getDescriptorName();
  auto        it   = array_klasses_.find(name);
  if (it != array_klasses_.end()) {
    return static_cast<ObjArrayKlass*>(it->second.get());
  }

  auto  klass     = std::make_unique<ObjArrayKlass>(component, name);
  auto* klass_ptr = static_cast<ObjArrayKlass*>(klass.get());
  array_klasses_.emplace(name, std::move(klass));
  return klass_ptr;
}

bool MethodArea::hasClass(const ClassIdentifier& identifier) const { return instance_klasses_.contains(identifier); }

}  // namespace jvm::oops