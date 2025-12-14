#pragma once

#include "local_variables.h"
#include "operand_stack.h"

namespace jvm::runtime {

class Method;  // avoid circular dependency

class Frame {
 public:
  explicit Frame(Method* method) : method_(method) {}
  Frame(const Frame&)            = delete;
  Frame(Frame&&)                 = default;
  Frame& operator=(const Frame&) = delete;
  Frame& operator=(Frame&&)      = default;
  ~Frame()                       = default;

  LocalVariables& getLocalVariables() { return local_variables_; }
  OperandStack&   getOperandStack() { return operand_stack_; }
  Method*         getMethod() { return method_; }

 private:
  LocalVariables local_variables_;
  OperandStack   operand_stack_;
  Method*        method_;  // points to method area
};

}  // namespace jvm::runtime