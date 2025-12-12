#pragma once

#include "local_variables.h"
#include "operand_stack.h"

namespace jvm::runtime {

// class Method; // Not implemented yet

class Frame {
 public:
  Frame()                        = default;
  Frame(const Frame&)            = delete;
  Frame(Frame&&)                 = delete;
  Frame& operator=(const Frame&) = delete;
  Frame& operator=(Frame&&)      = delete;
  ~Frame()                       = default;

  LocalVariables& getLocalVariables() { return local_variables_; }
  OperandStack&   getOperandStack() { return operand_stack_; }
  // Method*         getMethod() { return method_; }

 private:
  LocalVariables local_variables_;
  OperandStack   operand_stack_;
  // Method*        method_;  // points to method area
};

}  // namespace jvm::runtime