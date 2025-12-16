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

  void   pushInt(Jint value) { stack_.push({.i = value}); }
  Jint   popInt() { return popSlot().i; }
  void   pushFloat(Jfloat value) { stack_.push({.f = value}); }
  Jfloat popFloat() { return popSlot().f; }
  void   pushLong(Jlong value) {
    // Long values occupy 2 slots in the operand stack
    // Push a placeholder first (second slot), then push the actual value (first slot)
    // When popped, the value is on top, then the placeholder
    stack_.push({.i = 0});      // Push a placeholder slot first (second slot)
    stack_.push({.l = value});  // Push the actual value (first slot, on top)
  }
  Jlong popLong() {
    // Long values occupy 2 slots, pop both
    // The value is on top, then the placeholder
    Jlong value = popSlot().l;  // Pop the first slot with the actual value
    popSlot();                  // Pop the second (placeholder) slot
    return value;
  }
  void pushDouble(Jdouble value) {
    // Double values occupy 2 slots in the operand stack
    // Push a placeholder first (second slot), then push the actual value (first slot)
    // When popped, the value is on top, then the placeholder
    stack_.push({.i = 0});      // Push a placeholder slot first (second slot)
    stack_.push({.d = value});  // Push the actual value (first slot, on top)
  }
  Jdouble popDouble() {
    // Double values occupy 2 slots, pop both
    // The value is on top, then the placeholder
    Jdouble value = popSlot().d;  // Pop the first slot with the actual value
    popSlot();                    // Pop the second (placeholder) slot
    return value;
  }
  void pushRef(Jref value) { stack_.push({.r = value}); }
  Jref popRef() { return popSlot().r; }

 private:
  std::stack<Slot> stack_;
};

}  // namespace jvm::runtime