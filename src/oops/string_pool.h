#pragma once
#include <string>
#include <unordered_set>

namespace jvm::oops {
class StringPool {
 public:
  static StringPool& getSingleton() {
    static StringPool p;
    return p;
  }

  const std::string* intern(const std::string& s) { return &*pool_.insert(s).first; }

  void reset() { pool_.clear(); }  // for test purpose

 private:
  StringPool() = default;
  std::unordered_set<std::string> pool_;
};
}  // namespace jvm::oops