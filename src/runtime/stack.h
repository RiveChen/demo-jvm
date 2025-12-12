#pragma once

#include <stack>

#include "frame.h"

namespace jvm::runtime {

class Stack {
 public:
  Stack()                        = default;
  Stack(const Stack&)            = delete;
  Stack(Stack&&)                 = delete;
  Stack& operator=(const Stack&) = delete;
  Stack& operator=(Stack&&)      = delete;
  ~Stack()                       = default;

  void   push(Frame frame) { frames_.push(frame); }
  void   pop() { frames_.pop(); }
  Frame& top() { return frames_.top(); }
  bool   empty() { return frames_.empty(); }

 private:
  std::stack<Frame> frames_;
};

}  // namespace jvm::runtime