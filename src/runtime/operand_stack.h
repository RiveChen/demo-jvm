/**
 * @file operand_stack.h
 * @author Rive Chen
 * @brief Operand stack in frame
 * @version 0.1
 * @date 2025-12-12
 *
 * @copyright Copyright (c) 2025
 *
 */
#pragma once

#include <stack>

#include "runtime/slot.h"

namespace jvm::runtime {

class OperandStack {
 public:
  OperandStack()                               = default;
  OperandStack(const OperandStack&)            = delete;
  OperandStack(OperandStack&&)                 = default;
  OperandStack& operator=(const OperandStack&) = delete;
  OperandStack& operator=(OperandStack&&)      = default;
  ~OperandStack()                              = default;

  U2 getSize() const { return stack_.size(); }

  void pushSlot(Slot value) { stack_.push(value); }
  Slot popSlot() {
    if (stack_.empty()) {
      throw std::runtime_error("Operand stack is empty");
    }
    auto value = stack_.top();
    stack_.pop();
    return value;
  }

  void    pushInt(Jint value) { stack_.push({.i = value}); }
  Jint    popInt() { return popSlot().i; }
  void    pushFloat(Jfloat value) { stack_.push({.f = value}); }
  Jfloat  popFloat() { return popSlot().f; }
  void    pushLong(Jlong value) { stack_.push({.l = value}); }
  Jlong   popLong() { return popSlot().l; }
  void    pushDouble(Jdouble value) { stack_.push({.d = value}); }
  Jdouble popDouble() { return popSlot().d; }
  void    pushRef(Jref value) { stack_.push({.r = value}); }
  Jref    popRef() { return popSlot().r; }

 private:
  std::stack<Slot> stack_;
};

}  // namespace jvm::runtime