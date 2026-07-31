/**
 * @file method.hpp
 * @brief Runtime representation of a Java method.
 *
 * A Method object holds the bytecode, max stack/locals, access flags,
 * and parsed signature for a single method. Method objects are created
 * by Klass during class loading.
 */

#pragma once

#include "utilities/access_flags.hpp"
#include "utilities/descriptor.hpp"

#include <string>
#include <vector>

namespace jvm::oops {

class Frame;
class InstanceKlass;

/**
 * @brief Runtime representation of a single Java method.
 *
 * Stores the method's bytecode, metadata (max_stack, max_locals),
 * access flags, and parsed method signature. Created and owned
 * by the Klass object representing the declaring class.
 */
class Method {
 public:
  /// @name Query Access Flags
  ///@{
  bool isStatic() const { return access_flags_.has(flags::Method::STATIC); }
  bool isNative() const { return access_flags_.has(flags::Method::NATIVE); }
  ///@}

  /// @name Method Identity
  ///@{
  const std::string&            getName() const { return name_; }
  const std::string&            getDescriptor() const { return descriptor_; }
  const descriptor::MethodType& getSignature() const { return signature_; }
  ///@}

  /// @brief The class that owns this method.
  InstanceKlass* getOwnerKlass() const { return owner_klass_; }

  /// @name Bytecode and Frame Layout
  ///@{
  const std::vector<U1>& getCode() const { return code_; }
  U2                     getMaxStack() const { return max_stack_; }
  U2                     getMaxLocals() const { return max_locals_; }
  ///@}

 private:
  /// Methods can only be constructed by Klass (friend).
  Method() = default;
  Method(AccessFlags<flags::Method> access_flags, std::string name, std::string descriptor,
         InstanceKlass* owner_klass)
    : access_flags_(access_flags),
      name_(std::move(name)),
      descriptor_(std::move(descriptor)),
      owner_klass_(owner_klass),
      signature_(descriptor::parseMethodDescriptor(descriptor_)) {}

  AccessFlags<flags::Method> access_flags_;
  std::string                name_;
  std::string                descriptor_;

  InstanceKlass*         owner_klass_{nullptr};
  descriptor::MethodType signature_;

  U2              max_stack_{};
  U2              max_locals_{};
  std::vector<U1> code_;

  friend class InstanceKlass;
};

}  // namespace jvm::oops