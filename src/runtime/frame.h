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

  size_t getCallerPC() const { return caller_pc_; }
  void   setCallerPC(size_t pc) { caller_pc_ = pc; }

 private:
  Method*        method_;  // points to method area
  LocalVariables local_variables_;
  OperandStack   operand_stack_;

  size_t caller_pc_{0};
};

}  // namespace jvm::runtime