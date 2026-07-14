/**
 * @file thread.h
 * @brief Representation of a single Java thread.
 *
 * Models a single-threaded execution context as defined in
 * JVM Specification §2.5. Each thread has a private memory area
 * consisting of a Program Counter register and a VM stack.
 *
 * @note This implementation is single-threaded; no threading or
 * synchronization primitives are provided yet.
 */

#pragma once

#include "stack.hpp"

namespace jvm::runtime {

class Frame;

/**
 * @brief A single Java-level thread of execution.
 *
 * Holds a private VM stack containing frames for each pending method
 * invocation. This is the top-level execution context for the
 * interpreter loop.
 */
class Thread {
 public:
  Thread()                         = default;
  Thread(const Thread&)            = delete;
  Thread(Thread&&)                 = default;
  Thread& operator=(const Thread&) = delete;
  Thread& operator=(Thread&&)      = default;
  ~Thread()                        = default;

  /// @name Stack Operations
  ///@{
  /// @brief Access the underlying frame stack.
  Stack& getStack() { return stack_; }
  /// @brief Push a new frame onto this thread's call stack.
  void pushFrame(Frame frame) { stack_.push(std::move(frame)); }
  /// @brief Pop the top-most frame (method return).
  void popFrame() { stack_.pop(); }
  /// @brief Access the currently executing frame (top of stack).
  Frame& getCurrentFrame() { return stack_.top(); }
  /// @brief Check if the call stack has any frames left.
  bool isStackEmpty() { return stack_.empty(); }
  ///@}

 private:
  Stack stack_;  ///< The thread's private VM frame stack.
};

}  // namespace jvm::runtime