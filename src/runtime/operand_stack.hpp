/**
 * @file operand_stack.h
 * @brief Operand stack within a JVM stack frame.
 *
 * Implements the operand stack as described in JVM Specification §2.6.2.
 * All JVM instructions that consume and produce values do so via this stack.
 *
 */
#pragma once

#include "utilities/slot.hpp"

#include <cstddef>
#include <vector>

namespace jvm::runtime {

/**
 * @brief The operand stack of a stack frame.
 *
 * A LIFO stack of Slot values used for JVM instruction operands.
 * Supports all primitive types and object references, with special
 * handling for category-2 types (long, double) that occupy two slots.
 */
class OperandStack {
 public:
  OperandStack()                               = default;
  OperandStack(const OperandStack&)            = delete;
  OperandStack(OperandStack&&)                 = default;
  OperandStack& operator=(const OperandStack&) = delete;
  OperandStack& operator=(OperandStack&&)      = default;
  ~OperandStack()                              = default;

  /// @brief Return the current number of slots on the stack.
  size_t getSize() const { return stack_.size(); }

  /// @name Slot-level operations
  ///@{
  void pushSlot(Slot value) { stack_.push_back(value); }
  /// @brief Pop a single slot from the top of the stack.
  /// @throws std::runtime_error if the stack is empty.
  Slot popSlot() {
    if (stack_.empty()) {
      throw std::runtime_error("Operand stack is empty");
    }
    auto value = stack_.back();
    stack_.pop_back();
    return value;
  }
  /// @brief Push a category-2 value (long or double) onto the stack.
  /// Inserts a placeholder slot first, then the actual value.
  void pushWide(Slot value) {
    stack_.push_back({.i = 0});  // placeholder
    stack_.push_back(value);
  }
  /// @brief Pop a category-2 value (long or double) from the stack.
  /// @throws std::runtime_error if the stack is empty or has only one slot.
  Slot popWide() {
    if (stack_.size() <= 1) {
      throw std::runtime_error("Operand stack is nearly empty");
    }
    auto value = stack_.back();
    stack_.pop_back();
    stack_.pop_back();  // pop placeholder
    return value;
  }
  ///@}

  /// @name Typed push/pop wrappers
  ///@{
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
  /// @brief Peek at a reference @p depth slots below the top.
  /// @param depth 0 = top of stack, 1 = one below top, etc.
  Jref peekRef(size_t depth) const { return stack_.at(stack_.size() - 1 - depth).r; }
  ///@}

 private:
  std::vector<Slot> stack_;
};

}  // namespace jvm::runtime