#pragma once

#include <string>
#include <unordered_map>

#include "oops/constant_pool.h"

namespace jvm::runtime {
class OperandStack;
}

namespace jvm::engine {
using InterceptFn = void (*)(runtime::OperandStack&);

class StubIntercepts {
 public:
  static StubIntercepts& getSingleton() {
    static StubIntercepts instance;
    return instance;
  }
  void        bind(std::string key, InterceptFn fn) { table_.emplace(std::move(key), fn); }
  InterceptFn find(const std::string& key) const {
    auto it = table_.find(key);
    return it != table_.end() ? it->second : nullptr;
  }

 private:
  std::unordered_map<std::string, InterceptFn> table_;
};

void registerStubIntercepts();

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