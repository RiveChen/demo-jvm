/**
 * @file string_pool.h
 * @brief Interned string pool singleton.
 *
 * Mimics the JVM's string interning mechanism (cf. String.intern()).
 * All unique string constants from the constant pool are stored here
 * and deduplicated.
 */

#pragma once
#include <string>
#include <unordered_set>

namespace jvm::oops {

/**
 * @brief Singleton pool for interned Java strings.
 *
 * Provides pointer stability: once a string is inserted, the pointer
 * to its storage remains valid for the lifetime of the pool (unless
 * reset() is called).
 */
class StringPool {
 public:
  /// Meyer's singleton (thread-safe since C++11).
  static StringPool& getSingleton() {
    static StringPool p;
    return p;
  }

  /// @brief Intern a string: return a pointer to the unique pool copy.
  /// @param s The string to intern.
  /// @return Stable pointer to the pooled string (valid until reset()).
  const std::string* intern(const std::string& s) { return &*pool_.insert(s).first; }

  /// @brief Clear all interned strings (for testing).
  void reset() { pool_.clear(); }

 private:
  StringPool() = default;
  std::unordered_set<std::string> pool_;
};

}  // namespace jvm::oops