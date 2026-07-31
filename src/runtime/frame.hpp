/**
 * @file frame.hpp
 * @brief A single JVM stack frame.
 *
 * Represents a method invocation context as defined in JVM Specification §2.6.
 * Each frame contains a local variable array, an operand stack, and a
 * program counter.
 */

#pragma once

#include "local_variables.hpp"
#include "oops/method.hpp"
#include "operand_stack.hpp"

namespace jvm::runtime {

/**
 * @brief A single stack frame for a method invocation.
 *
 * Created each time a method is invoked (by INVOKEVIRTUAL, INVOKESPECIAL,
 * INVOKESTATIC, etc.) and destroyed upon return. The frame holds:
 * - The method being executed
 * - Local variables (method parameters + user locals)
 * - Operand stack (for bytecode instruction operands)
 * - Program counter tracking the current bytecode index
 */
class Frame {
 public:
  /// @brief Construct a frame for the given method.
  /// @param method The method to execute in this frame.
  explicit Frame(jvm::oops::Method* method)
    : method_(method), local_variables_(method->getMaxLocals()) {}
  Frame(const Frame&)            = delete;
  Frame(Frame&&)                 = default;
  Frame& operator=(const Frame&) = delete;
  Frame& operator=(Frame&&)      = default;
  ~Frame()                       = default;

  /// @name Accessors
  ///@{
  LocalVariables& getLocalVariables() { return local_variables_; }
  OperandStack&   getOperandStack() { return operand_stack_; }
  /// @brief The method currently being executed in this frame.
  jvm::oops::Method* getMethod() { return method_; }
  ///@}

  /// @name Program Counter
  ///@{
  /// @brief Get the current bytecode index (0-based).
  size_t getPC() const { return pc_; }
  /// @brief Set the current bytecode index.
  void setPC(size_t pc) { pc_ = pc; }
  ///@}

 private:
  jvm::oops::Method* method_;           ///< Points to the method in the method area.
  LocalVariables     local_variables_;  ///< Local variable array.
  OperandStack       operand_stack_;    ///< Operand stack.
  size_t             pc_{0};            ///< Bytecode program counter.
};

}  // namespace jvm::runtime