/**
 * @file native_registry.hpp
 * @brief Registry for JNI-style native method implementations.
 *
 * Maps (className.methodName.descriptor) keys to function pointers.
 * When the interpreter encounters a native method via INVOKESTATIC,
 * it uses this registry to find the C++ implementation.
 *
 * @note This is separate from StubIntercepts (which intercepts before
 *       resolution) — NativeRegistry is used for declared `native` methods.
 */

#pragma once
#include <string>
#include <unordered_map>

namespace jvm::runtime {
class OperandStack;
}

namespace jvm::engine {

/// Function pointer type for native method implementations.
using NativeFn = void (*)(runtime::OperandStack& stack);

/**
 * @brief Singleton registry for native method implementations.
 *
 * Native methods are keyed by "className.methodName.descriptor".
 * Methods registered here are called from the interpreter when
 * INVOKESTATIC resolves to a method with the NATIVE access flag set.
 */
class NativeRegistry {
 public:
  static NativeRegistry& getSingleton() {
    static NativeRegistry r;
    return r;
  }

  /// @brief Register a native method implementation.
  /// @param key "className.methodName.descriptor" format.
  /// @param fn The C++ function implementing the method.
  void bind(const std::string& key, NativeFn fn) { table_[key] = fn; }

  /// @brief Look up a native method by key.
  /// @param key "className.methodName.descriptor" format.
  /// @return The function pointer, or nullptr if not found.
  NativeFn find(const std::string& key) const {
    auto it = table_.find(key);
    return it != table_.end() ? it->second : nullptr;
  }

 private:
  std::unordered_map<std::string, NativeFn> table_;
};

/// @brief Register all built-in native method implementations.
void registerBuiltinNatives();
}  // namespace jvm::engine