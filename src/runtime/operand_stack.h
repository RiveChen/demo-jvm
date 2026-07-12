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

#include <cstddef>
#include <vector>

#include "utilities/slot.h"

namespace jvm::runtime {

class OperandStack {
 public:
  OperandStack()                               = default;
  OperandStack(const OperandStack&)            = delete;
  OperandStack(OperandStack&&)                 = default;
  OperandStack& operator=(const OperandStack&) = delete;
  OperandStack& operator=(OperandStack&&)      = default;
  ~OperandStack()                              = default;

  size_t getSize() const { return stack_.size(); }

  void pushSlot(Slot value) { stack_.push_back(value); }
  Slot popSlot() {
    if (stack_.empty()) {
      throw std::runtime_error("Operand stack is empty");
    }
    auto value = stack_.back();
    stack_.pop_back();
    return value;
  }

  void pushWide(Slot value) {
    // Push a placeholder first (second slot), then push the actual value (first slot)
    // When popped, the value is on top, then the placeholder
    stack_.push_back({.i = 0});  // placeholder
    stack_.push_back(value);
  }
  Slot popWide() {
    if (stack_.empty()) {
      throw std::runtime_error("Operand stack is empty");
    }
    auto value = stack_.back();
    stack_.pop_back();
    if (stack_.empty()) {
      throw std::runtime_error("Operand stack is empty");
    }
    stack_.pop_back();  // pop placeholder
    return value;
  }

  void    pushInt(Jint value) { pushSlot({.i = value}); }
  Jint    popInt() { return popSlot().i; }
  void    pushFloat(Jfloat value) { pushSlot({.f = value}); }
  Jfloat  popFloat() { return popSlot().f; }
  void    pushLong(Jlong value) { pushWide({.l = value}); }
  Jlong   popLong() { return popWide().l; }
  void    pushDouble(Jdouble value) { pushWide({.d = value}); }
  Jdouble popDouble() { return popWide().d; }
  void    pushRef(Jref value) { pushSlot({.r = value}); }
  Jref    popRef() { return popSlot().r; }
  Jref    peekRef(size_t depth) const { return stack_.at(stack_.size() - 1 - depth).r; }

 private:
  std::vector<Slot> stack_;
};

}  // namespace jvm::runtime