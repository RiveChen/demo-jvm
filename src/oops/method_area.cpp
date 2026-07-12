/**
 * @file method_area.cpp
 * @brief Method area singleton storage for loaded classes.
 */

#include "method_area.h"

namespace jvm::oops {

void MethodArea::addClass(ClassIdentifier identifier, ClassData class_data) {
  classes_.emplace(std::move(identifier), std::move(class_data));
}

Klass* MethodArea::getClass(const ClassIdentifier& identifier) {
  auto it = classes_.find(identifier);
  if (it != classes_.end()) {
    return it->second.first.get();
  }
  return nullptr;
}

bool MethodArea::hasClass(const ClassIdentifier& identifier) const {
  return classes_.contains(identifier);
}

}  // namespace jvm::oops