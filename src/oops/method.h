#pragma once

#include <string>
#include <vector>

#include "utilities/access_flags.hpp"
#include "utilities/descriptor.h"

namespace jvm::oops {

class Frame;  // avoid circular dependency
class Klass;
// using NativeMethod = void (*)(Frame*);

class Method {
 public:
  bool               isStatic() const { return access_flags_.has(flags::Method::STATIC); }
  bool               isNative() const { return access_flags_.has(flags::Method::NATIVE); }
  const std::string& getName() const { return name_; }
  const std::string& getDescriptor() const { return descriptor_; }
  const descriptor::MethodType& getSignature() const { return signature_; }
  Klass*                        getOwnerKlass() const { return owner_klass_; }
  const std::vector<U1>&        getCode() const { return code_; }
  U2                            getMaxStack() const { return max_stack_; }
  U2                            getMaxLocals() const { return max_locals_; }

 private:
  Method() = default;
  Method(AccessFlags<flags::Method> access_flags, std::string name, std::string descriptor,
         Klass* owner_klass)
    : access_flags_(access_flags),
      name_(std::move(name)),
      descriptor_(std::move(descriptor)),
      owner_klass_(owner_klass),
      signature_(descriptor::parseMethodDescriptor(descriptor_)) {}

  AccessFlags<flags::Method> access_flags_;
  std::string                name_;
  std::string                descriptor_;

  Klass*                 owner_klass_{nullptr};
  descriptor::MethodType signature_;

  U2              max_stack_{};
  U2              max_locals_{};
  std::vector<U1> code_;

  // NativeMethod native_function_;

  friend class Klass;
};

}  // namespace jvm::oops