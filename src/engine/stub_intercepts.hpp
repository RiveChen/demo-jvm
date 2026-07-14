/**
 * @file stub_intercepts.h
 * @brief Pre-resolution interceptors for JDK class stubs.
 *
 * When the interpreter encounters GETSTATIC, INVOKEVIRTUAL, or INVOKESPECIAL,
 * it can check this intercept table before attempting normal resolution.
 * This allows stubbing JDK classes (java.lang.System.out,
 * java.io.PrintStream.println, java.lang.Object.\<init\>) without needing
 * actual JDK class files on the classpath.
 */

#pragma once

#include <string>
#include <unordered_map>

#include "oops/constant_pool.hpp"

namespace jvm::runtime {
class OperandStack;
}

namespace jvm::engine {

/// Function pointer type for intercept handlers.
using InterceptFn = void (*)(runtime::OperandStack&);

/**
 * @brief Table of intercept handlers for stub JDK classes.
 *
 * Keys follow the format "className.memberName.descriptor" for fields
 * and methods. If a key matches, the corresponding handler is called
 * instead of normal class resolution.
 */
class StubIntercepts {
 public:
  static StubIntercepts& getSingleton() {
    static StubIntercepts instance;
    return instance;
  }

  /// @brief Register an intercept handler.
  void bind(std::string key, InterceptFn fn) { table_.emplace(std::move(key), fn); }

  /// @brief Look up an intercept handler by key.
  /// @return The handler, or nullptr if not intercepted.
  InterceptFn find(const std::string& key) const {
    auto it = table_.find(key);
    return it != table_.end() ? it->second : nullptr;
  }

 private:
  std::unordered_map<std::string, InterceptFn> table_;
};

/// @brief Register all built-in stub intercepts (System.out, println, Object.\<init\>).
void registerStubIntercepts();

/**
 * @brief Try to intercept a field/method reference before resolving it.
 *
 * Synthesizes a symbolic key from the constant pool entry and checks
 * the StubIntercepts table. If found, the handler is invoked and
 * the function returns true.
 *
 * @param cp The runtime constant pool.
 * @param index The constant pool index of the reference.
 * @param op_stack The current frame's operand stack.
 * @return true if the reference was intercepted and handled.
 */
inline bool tryStubIntercept(oops::RuntimeConstantPool& cp, U2 index,
                             runtime::OperandStack& op_stack) {
  if (auto key = cp.symbolicKey(index)) {
    if (auto fn = StubIntercepts::getSingleton().find(*key)) {
      fn(op_stack);
      return true;
    }
  }
  return false;
}
}  // namespace jvm::engine