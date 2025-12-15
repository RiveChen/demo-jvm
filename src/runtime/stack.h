#pragma once

#include <stack>

#include "frame.h"

namespace jvm::runtime {

class Stack {
 public:
  Stack()                        = default;
  Stack(const Stack&)            = delete;
  Stack(Stack&&)                 = default;
  Stack& operator=(const Stack&) = delete;
  Stack& operator=(Stack&&)      = default;
  ~Stack()                       = default;

  void push(Frame&& frame) { frames_.push(std::move(frame)); }
  void pop() {
    if (frames_.empty()) {
      throw std::runtime_error("Stack is empty");
    }
    frames_.pop();
  }
  Frame& top() {
    if (frames_.empty()) {
      throw std::runtime_error("Stack is empty");
    }
    return frames_.top();
  }
  bool empty() { return frames_.empty(); }

 private:
  std::stack<Frame> frames_;
};

}  // namespace jvm::runtime