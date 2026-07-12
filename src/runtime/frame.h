#pragma once

#include "local_variables.h"
#include "oops/method.h"
#include "operand_stack.h"

namespace jvm::runtime {

class Frame {
 public:
  explicit Frame(jvm::oops::Method* method)
    : method_(method), local_variables_(method->getMaxLocals()) {}
  Frame(const Frame&)            = delete;
  Frame(Frame&&)                 = default;
  Frame& operator=(const Frame&) = delete;
  Frame& operator=(Frame&&)      = default;
  ~Frame()                       = default;

  LocalVariables&    getLocalVariables() { return local_variables_; }
  OperandStack&      getOperandStack() { return operand_stack_; }
  jvm::oops::Method* getMethod() { return method_; }

  size_t getPC() const { return pc_; }
  void   setPC(size_t pc) { pc_ = pc; }

 private:
  jvm::oops::Method* method_;  // points to method area
  LocalVariables     local_variables_;
  OperandStack       operand_stack_;

  size_t pc_{0};
};

}  // namespace jvm::runtime