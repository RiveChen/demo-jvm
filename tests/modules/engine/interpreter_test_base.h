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

namespace {

// Base test fixture for interpreter tests
class InterpreterTestBase : public ::testing::Test {
 protected:
  void SetUp() override {
    test_classpath_ = TEST_CLASS_PATH;
    classpath_list_ = {test_classpath_};
    loader_         = std::make_unique<class_loader::ClassLoader>(nullptr, classpath_list_);
    runtime::MethodArea::getInstance().reset();
  }

  void TearDown() override { loader_.reset(); }

  // Helper to execute a static method and get int result
  Jint executeIntMethod(const std::string& class_name, const std::string& method_name,
                        const std::string& descriptor, Jint arg1, Jint arg2) {
    auto* klass = loader_->loadClass(class_name);
    if (klass == nullptr) {
      throw std::runtime_error("Failed to load class: " + class_name);
    }

    auto* method = klass->findMethod(method_name, descriptor);
    if (method == nullptr) {
      throw std::runtime_error("Method " + method_name + " with descriptor " + descriptor +
                               " not found");
    }

    runtime::Thread     thread;
    engine::Interpreter interpreter;

    // Create caller frame to receive return value
    runtime::Frame caller_frame(method);
    caller_frame.setCallerPC(method->getCode().size());
    thread.pushFrame(std::move(caller_frame));
    thread.setPC(method->getCode().size());

    // Create callee frame
    runtime::Frame callee_frame(method);
    callee_frame.getLocalVariables().setInt(0, arg1);
    callee_frame.getLocalVariables().setInt(1, arg2);
    thread.pushFrame(std::move(callee_frame));
    thread.setPC(0);

    interpreter.interpret(&thread);

    if (!thread.isStackEmpty()) {
      auto& current_frame = thread.getCurrentFrame();
      return current_frame.getOperandStack().popInt();
    }
    throw std::runtime_error("Stack is empty after execution");
  }

  // Helper to execute a static method with one int argument
  Jint executeIntMethod1Arg(const std::string& class_name, const std::string& method_name,
                            const std::string& descriptor, Jint arg) {
    auto* klass = loader_->loadClass(class_name);
    if (klass == nullptr) {
      throw std::runtime_error("Failed to load class: " + class_name);
    }

    auto* method = klass->findMethod(method_name, descriptor);
    if (method == nullptr) {
      throw std::runtime_error("Method " + method_name + " with descriptor " + descriptor +
                               " not found");
    }

    runtime::Thread     thread;
    engine::Interpreter interpreter;

    runtime::Frame caller_frame(method);
    caller_frame.setCallerPC(method->getCode().size());
    thread.pushFrame(std::move(caller_frame));
    thread.setPC(method->getCode().size());

    runtime::Frame callee_frame(method);
    callee_frame.getLocalVariables().setInt(0, arg);
    thread.pushFrame(std::move(callee_frame));
    thread.setPC(0);

    interpreter.interpret(&thread);

    if (!thread.isStackEmpty()) {
      auto& current_frame = thread.getCurrentFrame();
      return current_frame.getOperandStack().popInt();
    }
    throw std::runtime_error("Stack is empty after execution");
  }

  // Helper to execute a static method and get long result
  Jlong executeLongMethod(const std::string& class_name, const std::string& method_name,
                          const std::string& descriptor, Jlong arg1, Jlong arg2) {
    auto* klass = loader_->loadClass(class_name);
    if (klass == nullptr) {
      throw std::runtime_error("Failed to load class: " + class_name);
    }

    auto* method = klass->findMethod(method_name, descriptor);
    if (method == nullptr) {
      throw std::runtime_error("Method " + method_name + " with descriptor " + descriptor +
                               " not found");
    }

    runtime::Thread     thread;
    engine::Interpreter interpreter;

    runtime::Frame caller_frame(method);
    caller_frame.setCallerPC(method->getCode().size());
    thread.pushFrame(std::move(caller_frame));
    thread.setPC(method->getCode().size());

    runtime::Frame callee_frame(method);
    callee_frame.getLocalVariables().setLong(0, arg1);
    callee_frame.getLocalVariables().setLong(2, arg2);  // long takes 2 slots
    thread.pushFrame(std::move(callee_frame));
    thread.setPC(0);

    interpreter.interpret(&thread);

    if (!thread.isStackEmpty()) {
      auto& current_frame = thread.getCurrentFrame();
      return current_frame.getOperandStack().popLong();
    }
    throw std::runtime_error("Stack is empty after execution");
  }

  // Helper to execute a static method with one long argument
  Jlong executeLongMethod1Arg(const std::string& class_name, const std::string& method_name,
                              const std::string& descriptor, Jlong arg) {
    auto* klass = loader_->loadClass(class_name);
    if (klass == nullptr) {
      throw std::runtime_error("Failed to load class: " + class_name);
    }

    auto* method = klass->findMethod(method_name, descriptor);
    if (method == nullptr) {
      throw std::runtime_error("Method " + method_name + " with descriptor " + descriptor +
                               " not found");
    }

    runtime::Thread     thread;
    engine::Interpreter interpreter;

    runtime::Frame caller_frame(method);
    caller_frame.setCallerPC(method->getCode().size());
    thread.pushFrame(std::move(caller_frame));
    thread.setPC(method->getCode().size());

    runtime::Frame callee_frame(method);
    callee_frame.getLocalVariables().setLong(0, arg);
    thread.pushFrame(std::move(callee_frame));
    thread.setPC(0);

    interpreter.interpret(&thread);

    if (!thread.isStackEmpty()) {
      auto& current_frame = thread.getCurrentFrame();
      return current_frame.getOperandStack().popLong();
    }
    throw std::runtime_error("Stack is empty after execution");
  }

  // Helper to execute a static method and get float result
  Jfloat executeFloatMethod(const std::string& class_name, const std::string& method_name,
                            const std::string& descriptor, Jfloat arg1, Jfloat arg2) {
    auto* klass = loader_->loadClass(class_name);
    if (klass == nullptr) {
      throw std::runtime_error("Failed to load class: " + class_name);
    }

    auto* method = klass->findMethod(method_name, descriptor);
    if (method == nullptr) {
      throw std::runtime_error("Method " + method_name + " with descriptor " + descriptor +
                               " not found");
    }

    runtime::Thread     thread;
    engine::Interpreter interpreter;

    runtime::Frame caller_frame(method);
    caller_frame.setCallerPC(method->getCode().size());
    thread.pushFrame(std::move(caller_frame));
    thread.setPC(method->getCode().size());

    runtime::Frame callee_frame(method);
    callee_frame.getLocalVariables().setFloat(0, arg1);
    callee_frame.getLocalVariables().setFloat(1, arg2);
    thread.pushFrame(std::move(callee_frame));
    thread.setPC(0);

    interpreter.interpret(&thread);

    if (!thread.isStackEmpty()) {
      auto& current_frame = thread.getCurrentFrame();
      return current_frame.getOperandStack().popFloat();
    }
    throw std::runtime_error("Stack is empty after execution");
  }

  // Helper to execute a static method with one float argument
  Jfloat executeFloatMethod1Arg(const std::string& class_name, const std::string& method_name,
                                const std::string& descriptor, Jfloat arg) {
    auto* klass = loader_->loadClass(class_name);
    if (klass == nullptr) {
      throw std::runtime_error("Failed to load class: " + class_name);
    }

    auto* method = klass->findMethod(method_name, descriptor);
    if (method == nullptr) {
      throw std::runtime_error("Method " + method_name + " with descriptor " + descriptor +
                               " not found");
    }

    runtime::Thread     thread;
    engine::Interpreter interpreter;

    runtime::Frame caller_frame(method);
    caller_frame.setCallerPC(method->getCode().size());
    thread.pushFrame(std::move(caller_frame));
    thread.setPC(method->getCode().size());

    runtime::Frame callee_frame(method);
    callee_frame.getLocalVariables().setFloat(0, arg);
    thread.pushFrame(std::move(callee_frame));
    thread.setPC(0);

    interpreter.interpret(&thread);

    if (!thread.isStackEmpty()) {
      auto& current_frame = thread.getCurrentFrame();
      return current_frame.getOperandStack().popFloat();
    }
    throw std::runtime_error("Stack is empty after execution");
  }

  // Helper to execute a static method and get double result
  Jdouble executeDoubleMethod(const std::string& class_name, const std::string& method_name,
                              const std::string& descriptor, Jdouble arg1, Jdouble arg2) {
    auto* klass = loader_->loadClass(class_name);
    if (klass == nullptr) {
      throw std::runtime_error("Failed to load class: " + class_name);
    }

    auto* method = klass->findMethod(method_name, descriptor);
    if (method == nullptr) {
      throw std::runtime_error("Method " + method_name + " with descriptor " + descriptor +
                               " not found");
    }

    runtime::Thread     thread;
    engine::Interpreter interpreter;

    runtime::Frame caller_frame(method);
    caller_frame.setCallerPC(method->getCode().size());
    thread.pushFrame(std::move(caller_frame));
    thread.setPC(method->getCode().size());

    runtime::Frame callee_frame(method);
    callee_frame.getLocalVariables().setDouble(0, arg1);
    callee_frame.getLocalVariables().setDouble(2, arg2);  // double takes 2 slots
    thread.pushFrame(std::move(callee_frame));
    thread.setPC(0);

    interpreter.interpret(&thread);

    if (!thread.isStackEmpty()) {
      auto& current_frame = thread.getCurrentFrame();
      return current_frame.getOperandStack().popDouble();
    }
    throw std::runtime_error("Stack is empty after execution");
  }

  // Helper to execute a static method with one double argument
  Jdouble executeDoubleMethod1Arg(const std::string& class_name, const std::string& method_name,
                                  const std::string& descriptor, Jdouble arg) {
    auto* klass = loader_->loadClass(class_name);
    if (klass == nullptr) {
      throw std::runtime_error("Failed to load class: " + class_name);
    }

    auto* method = klass->findMethod(method_name, descriptor);
    if (method == nullptr) {
      throw std::runtime_error("Method " + method_name + " with descriptor " + descriptor +
                               " not found");
    }

    runtime::Thread     thread;
    engine::Interpreter interpreter;

    runtime::Frame caller_frame(method);
    caller_frame.setCallerPC(method->getCode().size());
    thread.pushFrame(std::move(caller_frame));
    thread.setPC(method->getCode().size());

    runtime::Frame callee_frame(method);
    callee_frame.getLocalVariables().setDouble(0, arg);
    thread.pushFrame(std::move(callee_frame));
    thread.setPC(0);

    interpreter.interpret(&thread);

    if (!thread.isStackEmpty()) {
      auto& current_frame = thread.getCurrentFrame();
      return current_frame.getOperandStack().popDouble();
    }
    throw std::runtime_error("Stack is empty after execution");
  }

  // Helper to execute a static method with no arguments and get int result
  Jint executeIntMethod0Arg(const std::string& class_name, const std::string& method_name,
                            const std::string& descriptor) {
    auto* klass = loader_->loadClass(class_name);
    if (klass == nullptr) {
      throw std::runtime_error("Failed to load class: " + class_name);
    }

    auto* method = klass->findMethod(method_name, descriptor);
    if (method == nullptr) {
      throw std::runtime_error("Method " + method_name + " with descriptor " + descriptor +
                               " not found");
    }

    runtime::Thread     thread;
    engine::Interpreter interpreter;

    runtime::Frame caller_frame(method);
    caller_frame.setCallerPC(method->getCode().size());
    thread.pushFrame(std::move(caller_frame));
    thread.setPC(method->getCode().size());

    runtime::Frame callee_frame(method);
    thread.pushFrame(std::move(callee_frame));
    thread.setPC(0);

    interpreter.interpret(&thread);

    if (!thread.isStackEmpty()) {
      auto& current_frame = thread.getCurrentFrame();
      return current_frame.getOperandStack().popInt();
    }
    throw std::runtime_error("Stack is empty after execution");
  }

  // Helper to execute a static method with no arguments and get long result
  Jlong executeLongMethod0Arg(const std::string& class_name, const std::string& method_name,
                              const std::string& descriptor) {
    auto* klass = loader_->loadClass(class_name);
    if (klass == nullptr) {
      throw std::runtime_error("Failed to load class: " + class_name);
    }

    auto* method = klass->findMethod(method_name, descriptor);
    if (method == nullptr) {
      throw std::runtime_error("Method " + method_name + " with descriptor " + descriptor +
                               " not found");
    }

    runtime::Thread     thread;
    engine::Interpreter interpreter;

    runtime::Frame caller_frame(method);
    caller_frame.setCallerPC(method->getCode().size());
    thread.pushFrame(std::move(caller_frame));
    thread.setPC(method->getCode().size());

    runtime::Frame callee_frame(method);
    thread.pushFrame(std::move(callee_frame));
    thread.setPC(0);

    interpreter.interpret(&thread);

    if (!thread.isStackEmpty()) {
      auto& current_frame = thread.getCurrentFrame();
      return current_frame.getOperandStack().popLong();
    }
    throw std::runtime_error("Stack is empty after execution");
  }

  // Helper to execute a static method with no arguments and get float result
  Jfloat executeFloatMethod0Arg(const std::string& class_name, const std::string& method_name,
                                const std::string& descriptor) {
    auto* klass = loader_->loadClass(class_name);
    if (klass == nullptr) {
      throw std::runtime_error("Failed to load class: " + class_name);
    }

    auto* method = klass->findMethod(method_name, descriptor);
    if (method == nullptr) {
      throw std::runtime_error("Method " + method_name + " with descriptor " + descriptor +
                               " not found");
    }

    runtime::Thread     thread;
    engine::Interpreter interpreter;

    runtime::Frame caller_frame(method);
    caller_frame.setCallerPC(method->getCode().size());
    thread.pushFrame(std::move(caller_frame));
    thread.setPC(method->getCode().size());

    runtime::Frame callee_frame(method);
    thread.pushFrame(std::move(callee_frame));
    thread.setPC(0);

    interpreter.interpret(&thread);

    if (!thread.isStackEmpty()) {
      auto& current_frame = thread.getCurrentFrame();
      return current_frame.getOperandStack().popFloat();
    }
    throw std::runtime_error("Stack is empty after execution");
  }

  // Helper to execute a static method with no arguments and get double result
  Jdouble executeDoubleMethod0Arg(const std::string& class_name, const std::string& method_name,
                                  const std::string& descriptor) {
    auto* klass = loader_->loadClass(class_name);
    if (klass == nullptr) {
      throw std::runtime_error("Failed to load class: " + class_name);
    }

    auto* method = klass->findMethod(method_name, descriptor);
    if (method == nullptr) {
      throw std::runtime_error("Method " + method_name + " with descriptor " + descriptor +
                               " not found");
    }

    runtime::Thread     thread;
    engine::Interpreter interpreter;

    runtime::Frame caller_frame(method);
    caller_frame.setCallerPC(method->getCode().size());
    thread.pushFrame(std::move(caller_frame));
    thread.setPC(method->getCode().size());

    runtime::Frame callee_frame(method);
    thread.pushFrame(std::move(callee_frame));
    thread.setPC(0);

    interpreter.interpret(&thread);

    if (!thread.isStackEmpty()) {
      auto& current_frame = thread.getCurrentFrame();
      return current_frame.getOperandStack().popDouble();
    }
    throw std::runtime_error("Stack is empty after execution");
  }

  // Helper to execute a method with no arguments and no return value
  void executeVoidMethod(const std::string& class_name, const std::string& method_name,
                         const std::string& descriptor) {
    auto* klass = loader_->loadClass(class_name);
    if (klass == nullptr) {
      throw std::runtime_error("Failed to load class: " + class_name);
    }

    auto* method = klass->findMethod(method_name, descriptor);
    if (method == nullptr) {
      throw std::runtime_error("Method " + method_name + " with descriptor " + descriptor +
                               " not found");
    }

    runtime::Thread     thread;
    engine::Interpreter interpreter;

    runtime::Frame caller_frame(method);
    caller_frame.setCallerPC(method->getCode().size());
    thread.pushFrame(std::move(caller_frame));
    thread.setPC(method->getCode().size());

    runtime::Frame callee_frame(method);
    thread.pushFrame(std::move(callee_frame));
    thread.setPC(0);

    interpreter.interpret(&thread);
  }

  std::string                                test_classpath_;
  std::vector<std::string>                   classpath_list_;
  std::unique_ptr<class_loader::ClassLoader> loader_;
};

}  // namespace
