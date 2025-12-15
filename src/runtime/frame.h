#pragma once

#include "local_variables.h"
#include "method.h"
#include "operand_stack.h"

namespace jvm::runtime {

class Frame {
 public:
  explicit Frame(Method* method) : method_(method), local_variables_(method->getMaxLocals()) {}
  Frame(const Frame&)            = delete;
  Frame(Frame&&)                 = default;
  Frame& operator=(const Frame&) = delete;
  Frame& operator=(Frame&&)      = default;
  ~Frame()                       = default;

  LocalVariables& getLocalVariables() { return local_variables_; }
  OperandStack&   getOperandStack() { return operand_stack_; }
  Method*         getMethod() { return method_; }

 private:
  Method*        method_;  // points to method area
  LocalVariables local_variables_;
  OperandStack   operand_stack_;
};

}  // namespace jvm::runtime