/**
 * @file interpreter.h
 * @brief The JVM bytecode interpreter.
 *
 * Implements the execution engine as described in JVM Specification §2.8.
 * The Interpreter fetches and executes bytecode instructions for the
 * currently active method in a given Thread.
 */

#pragma once

namespace jvm::runtime {
class Thread;
}  // namespace jvm::runtime

namespace jvm::engine {

/**
 * @brief Stack-based bytecode interpreter for the JVM.
 *
 * Drives the bytecode execution loop: fetch opcode, decode, and execute
 * using the current frame's operand stack and local variables. This is
 * the central dispatch point for all JVM instructions.
 */
class Interpreter {
 public:
  /**
   * @brief Execute bytecodes for the top-most frame on the given thread.
   *
   * Runs the interpretation loop until the thread's call stack is empty.
   * Each method invocation (INVOKEVIRTUAL, INVOKESPECIAL, INVOKESTATIC)
   * pushes a new frame; each return pops a frame.
   *
   * @param thread The execution thread whose current frame will be interpreted.
   */
  void interpret(runtime::Thread* thread);
};

}  // namespace jvm::engine