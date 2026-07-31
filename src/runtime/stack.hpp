/**
 * @file stack.hpp
 * @brief JVM virtual machine stack (a stack of frames).
 *
 * Implements the VM stack as defined in JVM Specification §2.5.2.
 * Each Java thread has a private stack, storing frames for
 * method invocations (including native methods).
 */

#pragma once

#include "frame.hpp"

#include <stack>

namespace jvm::runtime {

/**
 * @brief A LIFO stack of frames representing the call stack of a thread.
 *
 * Frames are pushed on method invocation and popped on method return.
 * The stack is owned by a Thread instance.
 */
class Stack {
 public:
  Stack()                        = default;
  Stack(const Stack&)            = delete;
  Stack(Stack&&)                 = default;
  Stack& operator=(const Stack&) = delete;
  Stack& operator=(Stack&&)      = default;
  ~Stack()                       = default;

  /// @brief Push a new frame onto the stack.
  void push(Frame&& frame) { frames_.push(std::move(frame)); }

  /// @brief Pop the top-most frame.
  /// @throws std::runtime_error if the stack is empty.
  void pop() {
    if (frames_.empty()) {
      throw std::runtime_error("Stack is empty");
    }
    frames_.pop();
  }

  /// @brief Access the top-most frame (mutable).
  /// @throws std::runtime_error if the stack is empty.
  Frame& top() {
    if (frames_.empty()) {
      throw std::runtime_error("Stack is empty");
    }
    return frames_.top();
  }

  /// @brief Check whether the stack has any frames.
  bool empty() { return frames_.empty(); }

 private:
  std::stack<Frame> frames_;
};

}  // namespace jvm::runtime