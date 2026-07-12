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

  Stack& getStack() { return stack_; }
  void   pushFrame(Frame frame) { stack_.push(std::move(frame)); }
  void   popFrame() { stack_.pop(); }
  Frame& getCurrentFrame() { return stack_.top(); }
  bool   isStackEmpty() { return stack_.empty(); }

 private:
  Stack stack_;
};

}  // namespace jvm::runtime