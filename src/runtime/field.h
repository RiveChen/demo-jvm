#pragma once

#include <string>

#include "common/access_flags.hpp"

namespace jvm::runtime {
class Klass;

class Field {
 public:
  bool               isStatic() const { return access_flags_.has(flags::Field::STATIC); }
  const std::string& getName() const { return name_; }
  const std::string& getDescriptor() const { return descriptor_; }
  Klass*             getOwnerKlass() const { return owner_klass_; }
  size_t             getSlotIndex() const { return slot_index_; }

 private:
  Field() = default;
  Field(AccessFlags<flags::Field> access_flags, std::string name, std::string descriptor,
        Klass* owner_klass)
    : access_flags_(access_flags),
      name_(std::move(name)),
      descriptor_(std::move(descriptor)),
      owner_klass_(owner_klass) {}

  AccessFlags<flags::Field> access_flags_;
  std::string               name_;
  std::string               descriptor_;

  Klass* owner_klass_{nullptr};
  size_t slot_index_{};

  friend class Klass;
};

}  // namespace jvm::runtime