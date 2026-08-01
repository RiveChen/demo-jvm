#pragma once

#include "classfile/class_loader.hpp"
#include "engine/interpreter.hpp"
#include "engine/native_registry.hpp"
#include "engine/stub_intercepts.hpp"
#include "oops/klass.hpp"
#include "oops/method.hpp"
#include "oops/method_area.hpp"
#include "runtime/frame.hpp"
#include "runtime/thread.hpp"
#include "utilities/descriptor.hpp"
#include "utilities/types.hpp"

#include <gtest/gtest.h>
#include <memory>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <variant>
#include <vector>

using namespace jvm;

namespace detail {
using engine::VmValue;

// Traits for JVM types.
template <typename T>
struct JvmTraits;

template <>
struct JvmTraits<Jint> {
  static constexpr std::string_view descriptor = "I";
  static constexpr U2               slots      = 1;
  static VmValue                    toVmValue(Jint val) { return val; }
  static Jint                       fromVmValue(const VmValue& v) { return std::get<Jint>(v); }
};

template <>
struct JvmTraits<Jlong> {
  static constexpr std::string_view descriptor = "J";
  static constexpr U2               slots      = 2;
  static VmValue                    toVmValue(Jlong val) { return val; }
  static Jlong                      fromVmValue(const VmValue& v) { return std::get<Jlong>(v); }
};

template <>
struct JvmTraits<Jfloat> {
  static constexpr std::string_view descriptor = "F";
  static constexpr U2               slots      = 1;
  static VmValue                    toVmValue(Jfloat val) { return val; }
  static Jfloat                     fromVmValue(const VmValue& v) { return std::get<Jfloat>(v); }
};

template <>
struct JvmTraits<Jdouble> {
  static constexpr std::string_view descriptor = "D";
  static constexpr U2               slots      = 2;
  static VmValue                    toVmValue(Jdouble val) { return val; }
  static Jdouble                    fromVmValue(const VmValue& v) { return std::get<Jdouble>(v); }
};

template <>
struct JvmTraits<Jboolean> {
  static constexpr std::string_view descriptor = "Z";
  static constexpr U2               slots      = 1;
  static VmValue                    toVmValue(Jboolean val) { return static_cast<Jint>(val); }
  static Jboolean fromVmValue(const VmValue& v) { return static_cast<Jboolean>(std::get<Jint>(v)); }
};

template <>
struct JvmTraits<Jbyte> {
  static constexpr std::string_view descriptor = "B";
  static constexpr U2               slots      = 1;
  static VmValue                    toVmValue(Jbyte val) { return static_cast<Jint>(val); }
  static Jbyte fromVmValue(const VmValue& v) { return static_cast<Jbyte>(std::get<Jint>(v)); }
};

template <>
struct JvmTraits<Jshort> {
  static constexpr std::string_view descriptor = "S";
  static constexpr U2               slots      = 1;
  static VmValue                    toVmValue(Jshort val) { return static_cast<Jint>(val); }
  static Jshort fromVmValue(const VmValue& v) { return static_cast<Jshort>(std::get<Jint>(v)); }
};

template <>
struct JvmTraits<Jchar> {
  static constexpr std::string_view descriptor = "C";
  static constexpr U2               slots      = 1;
  static VmValue                    toVmValue(Jchar val) { return static_cast<Jint>(val); }
  static Jchar fromVmValue(const VmValue& v) { return static_cast<Jchar>(std::get<Jint>(v)); }
};

template <>
struct JvmTraits<Jref> {
  static constexpr std::string_view descriptor = "Ljava/lang/Object;";
  static constexpr U2               slots      = 1;
  static VmValue                    toVmValue(Jref val) { return val; }
  static Jref                       fromVmValue(const VmValue& v) { return std::get<Jref>(v); }
};

template <>
struct JvmTraits<void> {
  static constexpr std::string_view descriptor = "V";
};

// Marshal a VmValue into local variable slots according to the JVM slot
// layout for the given method signature. slot index is advanced by the
// signature (category-2 => 2 slots) — the Method descriptor is authoritative.
inline void marshalArgument(runtime::LocalVariables& locals, U2& slot, descriptor::TypeKind kind,
                            const VmValue& value) {
  switch (kind) {
    case descriptor::TypeKind::Int:
      locals.setInt(slot, std::get<Jint>(value));
      slot += 1;
      break;
    case descriptor::TypeKind::Float:
      locals.setFloat(slot, std::get<Jfloat>(value));
      slot += 1;
      break;
    case descriptor::TypeKind::Ref:
      locals.setRef(slot, std::get<Jref>(value));
      slot += 1;
      break;
    case descriptor::TypeKind::Long:
      locals.setLong(slot, std::get<Jlong>(value));
      slot += 2;
      break;
    case descriptor::TypeKind::Double:
      locals.setDouble(slot, std::get<Jdouble>(value));
      slot += 2;
      break;
    case descriptor::TypeKind::Void:
      break;
  }
}

// True if T is one of the supported JVM argument types (primitives + Jref).
template <typename T>
struct IsJvmArg
  : std::bool_constant<
      std::is_same_v<std::decay_t<T>, Jint> || std::is_same_v<std::decay_t<T>, Jlong> ||
      std::is_same_v<std::decay_t<T>, Jfloat> || std::is_same_v<std::decay_t<T>, Jdouble> ||
      std::is_same_v<std::decay_t<T>, Jboolean> || std::is_same_v<std::decay_t<T>, Jbyte> ||
      std::is_same_v<std::decay_t<T>, Jshort> || std::is_same_v<std::decay_t<T>, Jchar> ||
      std::is_same_v<std::decay_t<T>, Jref>> {};

// True if every type in the pack is a supported JVM argument type.
template <typename... Args>
struct AllJvmArgs;

template <>
struct AllJvmArgs<> : std::true_type {};

template <typename T, typename... Rest>
struct AllJvmArgs<T, Rest...>
  : std::bool_constant<IsJvmArg<T>::value && AllJvmArgs<Rest...>::value> {};

// Verify the VmValue's runtime type matches the expected descriptor kind.
inline void checkArgumentType(descriptor::TypeKind expected, const VmValue& value,
                              const std::string& class_name, const std::string& method_name,
                              const std::string& descriptor, size_t arg_index) {
  const bool ok = [&] {
    switch (expected) {
      case descriptor::TypeKind::Int:
        return std::holds_alternative<Jint>(value);
      case descriptor::TypeKind::Float:
        return std::holds_alternative<Jfloat>(value);
      case descriptor::TypeKind::Ref:
        return std::holds_alternative<Jref>(value);
      case descriptor::TypeKind::Long:
        return std::holds_alternative<Jlong>(value);
      case descriptor::TypeKind::Double:
        return std::holds_alternative<Jdouble>(value);
      case descriptor::TypeKind::Void:
        return false;
    }
    return false;
  }();
  if (!ok) {
    FAIL() << "Argument type mismatch for " << class_name << "." << method_name << descriptor
           << " (argument " << arg_index << ")";
  }
}

}  // namespace detail

class InterpreterTestBase : public ::testing::Test {
 protected:
  std::unique_ptr<classfile::ClassLoader> loader_;
  std::vector<std::string>                classpath_list_;
  std::string                             test_classpath_;

  void SetUp() override {
    test_classpath_ = TEST_CLASS_PATH;
    classpath_list_ = {test_classpath_};
    loader_         = std::make_unique<classfile::ClassLoader>(nullptr, classpath_list_);
    oops::MethodArea::getSingleton().reset();
    // register built-ins so intercepted JDK calls (System.out/println,
    // Object.<init>) and ACC_NATIVE methods work in interpreter tests (bind is
    // idempotent)
    engine::registerBuiltinNatives();
    engine::registerStubIntercepts();
  }

  void TearDown() override { loader_.reset(); }

  /**
   * @brief Low-level test entry point: push a single entry frame for the
   * given method and run the interpreter without JVM invocation semantics
   * (no class initialization is triggered).
   *
   * This is intended for initialization-state machine tests that must
   * deliberately control the class state. Most tests should use
   * invokeStatic / executeStaticMethod instead.
   */
  engine::RunOutcome runEntryMethodForTest(oops::Method*                       method,
                                           const std::vector<engine::VmValue>& arguments = {}) {
    if (method == nullptr) {
      throw std::runtime_error("runEntryMethodForTest: null method");
    }
    // Validate argument count and types against the parsed Method signature.
    const auto& signature = method->getSignature();
    if (arguments.size() != signature.params.size()) {
      throw std::runtime_error("runEntryMethodForTest: argument count mismatch for " +
                               method->getOwnerKlass()->getName() + "." + method->getName() +
                               method->getDescriptor());
    }
    for (size_t i = 0; i < arguments.size(); ++i) {
      detail::checkArgumentType(signature.params[i], arguments[i],
                                method->getOwnerKlass()->getName(), method->getName(),
                                method->getDescriptor(), i);
    }

    jvm::runtime::Thread thread;
    jvm::runtime::Frame  frame(method);
    auto&                locals = frame.getLocalVariables();
    U2                   slot   = 0;
    for (size_t i = 0; i < arguments.size(); ++i) {
      detail::marshalArgument(locals, slot, signature.params[i], arguments[i]);
    }

    thread.pushFrame(std::move(frame));
    jvm::engine::Interpreter interpreter;
    return interpreter.interpret(&thread);
  }

  /**
   * @brief Core JVM invocation layer: invoke a static method with exact
   * descriptor and JVM-typed arguments.
   *
   * Performs full JVM call semantics:
   *   - locate the method by class_name + method_name + exact descriptor
   *   - verify it is static
   *   - validate each argument against the parsed Method signature
   *   - marshal arguments into a single entry frame per JVM slot layout
   *   - initialize the declaring class (active use)
   *   - run the interpreter to completion
   *
   * @return The interpreter's RunOutcome (value transferred to the host).
   */
  engine::RunOutcome invokeStatic(const std::string& class_name, const std::string& method_name,
                                  const std::string&                  exact_descriptor,
                                  const std::vector<engine::VmValue>& arguments) {
    auto* klass = loader_->loadClass(class_name);
    if (klass == nullptr) {
      throw std::runtime_error("Class not found: " + class_name);
    }

    auto* method = klass->findMethod(method_name, exact_descriptor);
    if (method == nullptr) {
      throw std::runtime_error("Method not found: " + class_name + "." + method_name + " " +
                               exact_descriptor);
    }
    if (!method->isStatic()) {
      throw std::runtime_error("invokeStatic: method is not static: " + class_name + "." +
                               method_name + " " + exact_descriptor);
    }

    // Class initialization: invoking a static method is an active use.
    // Reuses the same initialization machinery as the INVOKESTATIC handler:
    // if the class (or a superclass) has a <clinit>, execute it first; the
    // init state machine runs its own frames on a dedicated thread.
    if (klass->getState() == oops::InstanceKlass::Linked) {
      jvm::runtime::Thread     init_thread;
      jvm::engine::Interpreter init_interpreter;
      klass->initialize(&init_thread);
      if (!init_thread.isStackEmpty()) {
        // <clinit> frame(s) pushed; run them to completion (ignoring their
        // void result) before continuing with the entry method.
        init_interpreter.interpret(&init_thread);
      }
    }

    return runEntryMethodForTest(method, arguments);
  }

  /**
   * @brief Convenience template layer for test writing.
   *
   * Wraps C++ arguments into VmValue, invokes the core invokeStatic, and
   * strictly validates the RunOutcome before converting the return value.
   *
   * This overload derives the method descriptor from the C++ argument types
   * and is only suitable for non-ambiguous primitive/reference signatures.
   * For reference parameters with a specific runtime type or overload
   * disambiguation, use the overload that takes an explicit descriptor.
   */
  template <typename Ret, typename... Args>
    requires detail::AllJvmArgs<Args...>::value
  Ret executeStaticMethod(const std::string& class_name, const std::string& method_name,
                          Args... args) {
    std::string descriptor = "(";
    ((descriptor += detail::JvmTraits<Args>::descriptor), ...);
    descriptor += ")";
    descriptor += detail::JvmTraits<Ret>::descriptor;
    return executeStaticMethodWithDescriptor<Ret>(class_name, method_name, descriptor,
                                                  std::move(args)...);
  }

  /**
   * @brief Explicit-descriptor overload.
   *
   * @param exact_descriptor The precise JVM method descriptor, e.g.
   *   "(Ljava/lang/String;)I". Required for reference arguments (Jref is an
   *   opaque pointer and cannot encode the target type) and for overload
   *   disambiguation.
   */
  template <typename Ret, typename... Args>
  Ret executeStaticMethod(const std::string& class_name, const std::string& method_name,
                          const std::string& exact_descriptor, Args... args) {
    return executeStaticMethodWithDescriptor<Ret>(class_name, method_name, exact_descriptor,
                                                  std::move(args)...);
  }

  // Shared implementation behind both executeStaticMethod overloads.
  template <typename Ret, typename... Args>
  Ret executeStaticMethodWithDescriptor(const std::string& class_name,
                                        const std::string& method_name,
                                        const std::string& exact_descriptor, Args... args) {
    // Wrap arguments into VmValue.
    std::vector<engine::VmValue> values;
    values.reserve(sizeof...(Args));
    ((values.push_back(detail::JvmTraits<Args>::toVmValue(std::move(args)))), ...);

    engine::RunOutcome outcome = invokeStatic(class_name, method_name, exact_descriptor, values);

    // Strict outcome validation. FAIL() expands to a `return void;` and
    // therefore cannot be used inside this non-void helper; ADD_FAILURE records
    // the failure and we return a default value (never observed by the caller
    // because the test has already failed).
    if (!outcome.isCompleted()) {
      ADD_FAILURE() << "executeStaticMethod: non-completed outcome for " << class_name << "."
                    << method_name << " " << exact_descriptor;
      if constexpr (std::is_same_v<Ret, void>) {
        return;
      } else {
        return Ret{};
      }
    }

    if constexpr (std::is_same_v<Ret, void>) {
      // void methods must report CompletedVoid.
      if (!std::holds_alternative<engine::CompletedVoid>(outcome.value)) {
        ADD_FAILURE() << "executeStaticMethod<void>: expected CompletedVoid for " << class_name
                      << "." << method_name << " " << exact_descriptor;
      }
      return;
    } else {
      // Non-void: the outcome must carry a Completed<VmValue>.
      const auto* completed = std::get_if<engine::Completed<engine::VmValue>>(&outcome.value);
      if (completed == nullptr) {
        ADD_FAILURE() << "executeStaticMethod: expected Completed<VmValue> for " << class_name
                      << "." << method_name << " " << exact_descriptor;
        return Ret{};
      }
      return detail::JvmTraits<Ret>::fromVmValue(completed->result);
    }
  }
};