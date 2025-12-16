#pragma once

#include <gtest/gtest.h>

#include <memory>
#include <string>
#include <vector>

#include "class_loader/class_loader.h"
#include "engine/interpreter.h"
#include "runtime/frame.h"
#include "runtime/method_area.h"
#include "runtime/thread.h"

using namespace jvm;

namespace detail {
// Traits for JVM types
template <typename T>
struct JvmTraits;

// Specialization: Jint
template <>
struct JvmTraits<Jint> {
  static constexpr std::string_view descriptor = "I";
  static constexpr U2               slots      = 1;
  // Wrapper for LocalVariables set operation
  static void setLocal(jvm::runtime::LocalVariables& vars, U2 index, Jint val) {
    vars.setInt(index, val);
  }
  // Wrapper for OperandStack pop operation
  static Jint popStack(jvm::runtime::OperandStack& stack) { return stack.popInt(); }
};

// Specialization: Jlong (note slots = 2)
template <>
struct JvmTraits<Jlong> {
  static constexpr std::string_view descriptor = "J";
  static constexpr U2               slots      = 2;
  static void setLocal(jvm::runtime::LocalVariables& vars, U2 index, Jlong val) {
    vars.setLong(index, val);
  }
  static Jlong popStack(jvm::runtime::OperandStack& stack) { return stack.popLong(); }
};

// Specialization: Jfloat
template <>
struct JvmTraits<Jfloat> {
  static constexpr std::string_view descriptor = "F";
  static constexpr U2               slots      = 1;
  static void setLocal(jvm::runtime::LocalVariables& vars, U2 index, Jfloat val) {
    vars.setFloat(index, val);
  }
  static Jfloat popStack(jvm::runtime::OperandStack& stack) { return stack.popFloat(); }
};

// Specialization: Jdouble (note slots = 2)
template <>
struct JvmTraits<Jdouble> {
  static constexpr std::string_view descriptor = "D";
  static constexpr U2               slots      = 2;
  static void setLocal(jvm::runtime::LocalVariables& vars, U2 index, Jdouble val) {
    vars.setDouble(index, val);
  }
  static Jdouble popStack(jvm::runtime::OperandStack& stack) { return stack.popDouble(); }
};

// Specialization: void (only used for return value)
template <>
struct JvmTraits<void> {
  static constexpr std::string_view descriptor = "V";
  // void type does not need setLocal or popStack
};
}  // namespace detail

class InterpreterTestBase : public ::testing::Test {
 protected:
  std::unique_ptr<class_loader::ClassLoader> loader_;
  std::vector<std::string>                   classpath_list_;
  std::string                                test_classpath_;

  void SetUp() override {
    test_classpath_ = TEST_CLASS_PATH;
    classpath_list_ = {test_classpath_};
    loader_         = std::make_unique<class_loader::ClassLoader>(nullptr, classpath_list_);
    runtime::MethodArea::getInstance().reset();
  }

  void TearDown() override { loader_.reset(); }

  /**
   * @brief Generic static method execution helper function
   * @tparam Ret Return value type (explicitly specified, e.g. executeStaticMethod<Jint>(...))
   * @tparam Args Parameter types (automatically deduced by compiler)
   */
  template <typename Ret, typename... Args>
  Ret executeStaticMethod(const std::string& class_name, const std::string& method_name,
                          Args... args) {
    // 1. Dynamically build method descriptor: (Arg1Arg2...)Ret
    std::string descriptor = "(";
    // Use fold expression to concatenate all parameter descriptors
    ((descriptor += detail::JvmTraits<Args>::descriptor), ...);
    descriptor += ")";
    descriptor += detail::JvmTraits<Ret>::descriptor;

    // 2. Load class and find method
    auto* klass = loader_->loadClass(class_name);
    if (!klass) throw std::runtime_error("Class not found: " + class_name);

    auto* method = klass->findMethod(method_name, descriptor);
    if (!method) throw std::runtime_error("Method not found: " + method_name + " " + descriptor);

    jvm::runtime::Thread     thread;
    jvm::engine::Interpreter interpreter;

    // 3. Prepare caller frame (Caller Frame)
    jvm::runtime::Frame caller_frame(method);
    caller_frame.setCallerPC(method->getCode().size());
    thread.pushFrame(std::move(caller_frame));
    thread.setPC(method->getCode().size());

    // 4. Prepare callee frame (Callee Frame)
    jvm::runtime::Frame callee_frame(method);

    // 5. Parameter passing (generic handling)
    U2 current_slot = 0;
    // Define lambda to automatically handle different parameter types and slot index increment
    auto set_arg = [&](auto val) {
      using T = decltype(val);
      detail::JvmTraits<T>::setLocal(callee_frame.getLocalVariables(), current_slot, val);
      current_slot += detail::JvmTraits<T>::slots;
    };
    // Use fold expression to expand parameter pack
    (set_arg(args), ...);

    thread.pushFrame(std::move(callee_frame));
    thread.setPC(0);

    // 6. Execute
    interpreter.interpret(&thread);

    // 7. Get return value (using if constexpr to handle void)
    if constexpr (std::is_same_v<Ret, void>) {
      return;
    } else {
      if (!thread.isStackEmpty()) {
        return detail::JvmTraits<Ret>::popStack(thread.getCurrentFrame().getOperandStack());
      }
      throw std::runtime_error("Stack is empty after execution");
    }
  }
};
