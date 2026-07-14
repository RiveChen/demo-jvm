/**
 * @file field.h
 * @brief Runtime representation of a Java field.
 *
 * A Field object stores metadata about a class or instance field:
 * its name, descriptor (type), access flags, owner class, and
 * slot index in the instance or static data layout.
 */

#pragma once

#include <string>

#include "utilities/access_flags.hpp"
#include "utilities/descriptor.hpp"

namespace jvm::oops {
class Klass;

/**
 * @brief Runtime representation of a single field in a class.
 *
 * Created by Klass during class loading. The slot index determines
 * the field's offset within instance data (for instance fields) or
 * the static variable array (for static fields).
 */
class Field {
 public:
  /// @brief Check whether this field is static.
  bool               isStatic() const { return access_flags_.has(flags::Field::STATIC); }
  const std::string& getName() const { return name_; }
  const std::string& getDescriptor() const { return descriptor_; }
  /// @brief The parsed field type.
  descriptor::TypeKind getSignature() const { return signature_; }
  /// @brief The class that owns this field.
  Klass* getOwnerKlass() const { return owner_klass_; }
  /// @brief The index of this field in the instance or static slot array.
  size_t getSlotIndex() const { return slot_index_; }

 private:
  /// Fields can only be constructed by Klass (friend).
  Field() = default;
  Field(AccessFlags<flags::Field> access_flags, std::string name, std::string descriptor,
        Klass* owner_klass)
    : access_flags_(access_flags),
      name_(std::move(name)),
      descriptor_(std::move(descriptor)),
      owner_klass_(owner_klass),
      signature_(descriptor::parseFieldType(descriptor_)) {}

  AccessFlags<flags::Field> access_flags_;
  std::string               name_;
  std::string               descriptor_;

  Klass*               owner_klass_{nullptr};
  descriptor::TypeKind signature_{};
  size_t               slot_index_{};

  friend class Klass;
};

}  // namespace jvm::oops