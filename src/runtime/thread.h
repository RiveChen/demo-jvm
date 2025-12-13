#pragma once

#include "stack.h"

namespace jvm::runtime {

class Frame;

class Thread {
 public:
  Thread()                         = default;
  Thread(const Thread&)            = delete;
  Thread(Thread&&)                 = default;
  Thread& operator=(const Thread&) = delete;
  Thread& operator=(Thread&&)      = default;
  ~Thread()                        = default;

  size_t getPC() const { return pc_; }
  void   setPC(size_t pc) { pc_ = pc; }
  void   incrementPC() { pc_++; }

  Stack& getStack() { return stack_; }
  void   pushFrame(Frame frame) { stack_.push(std::move(frame)); }
  void   popFrame() { stack_.pop(); }
  Frame& getCurrentFrame() { return stack_.top(); }
  bool   isStackEmpty() { return stack_.empty(); }

 private:
  size_t pc_{0};
  Stack  stack_;
};

}  // namespace jvm::runtime