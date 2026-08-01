/**
 * @file interpreter.hpp
 * @brief The JVM bytecode interpreter.
 *
 * Implements the execution engine as described in JVM Specification §2.8.
 * The Interpreter fetches and executes bytecode instructions for the
 * currently active method in a given Thread.
 */

#pragma once

#include "utilities/types.hpp"

#include <cstdint>
#include <variant>

namespace jvm::runtime {
class Thread;
}  // namespace jvm::runtime

namespace jvm::engine {

/// @brief A single JVM-level value used for argument marshalling and
/// return-value transfer between the host and interpreted methods.
///
/// boolean/byte/char/short are represented as Int at the operand/local
/// layer. References are opaque pointers (Jref).
using VmValue = std::variant<Jint, Jlong, Jfloat, Jdouble, Jref>;

struct ReturnValue {
  enum Kind : uint8_t {
    Int,
    Long,
    Float,
    Double,
    Reference,
  } kind;
  union {
    Jint    i;
    Jlong   l;
    Jfloat  f;
    Jdouble d;
    Jref    r;
  };
};

/// @brief A completed method invocation whose declared return type is void.
struct CompletedVoid {};

/// @brief A completed method invocation that produced a value.
template <typename T>
struct Completed {
  T result;
};

/// @brief The outcome of running the interpreter to completion.
///
/// Successful completions are represented by CompletedVoid (void return) or
/// Completed<ReturnValue> (a typed return value). Future stages will add
/// UncaughtException, YieldedAtSafepoint, Blocked, and VmError as additional
/// alternatives.
struct RunOutcome {
  std::variant<CompletedVoid, Completed<ReturnValue>> value;

  bool isCompleted() const { return value.index() <= 1; }
  bool isVoid() const { return std::holds_alternative<CompletedVoid>(value); }
};

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
   * @brief Run the top-most frame (and any frames it invokes) until the
   * thread's call stack is empty or a non-completion outcome is produced.
   *
   * Each method invocation (INVOKEVIRTUAL, INVOKESPECIAL, INVOKESTATIC)
   * pushes a new frame; each return pops a frame. When the last (entry)
   * frame returns, the interpreter returns to the host together with the
   * method's return value.
   *
   * @param thread The execution thread whose current frame will be interpreted.
   * @return The outcome of the interpretation run.
   */
  RunOutcome interpret(runtime::Thread* thread);
};

}  // namespace jvm::engine