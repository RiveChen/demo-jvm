#include "method_area.h"

namespace jvm::runtime {

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
  return classes_.find(identifier) != classes_.end();
}

}  // namespace jvm::runtime