#pragma once
#include <string>
#include <unordered_map>

namespace jvm::runtime {
class OperandStack;
}

namespace jvm::engine {
using NativeFn = void (*)(runtime::OperandStack& stack);

class NativeRegistry {
 public:
  static NativeRegistry& getSingleton() {
    static NativeRegistry r;
    return r;
  }
  void     bind(const std::string& key, NativeFn fn) { table_[key] = fn; }
  NativeFn find(const std::string& key) const {
    auto it = table_.find(key);
    return it != table_.end() ? it->second : nullptr;
  }

 private:
  std::unordered_map<std::string, NativeFn> table_;
};

void registerBuiltinNatives();
}  // namespace jvm::engine