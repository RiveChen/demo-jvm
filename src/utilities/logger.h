/**
 * @file logger.h
 * @brief Minimal compile-time / runtime level-controlled logger.
 *
 * Five levels: TRACE < DEBUG < INFO < WARN < ERROR.
 * Default runtime level: INFO (set via JVM_LOG_LEVEL env var).
 * Compile-time max level: define JVM_LOG_MAX_LEVEL=N to strip levels > N
 *   (e.g. -DJVM_LOG_MAX_LEVEL=2 strips TRACE and DEBUG at compile time).
 */

#pragma once

#include <cstdlib>
#include <iostream>
#include <string_view>

namespace jvm::log {

enum class Level : int { TRACE = 0, DEBUG = 1, INFO = 2, WARN = 3, ERROR = 4, OFF = 5 };

/// @brief Resolve the minimum output level from environment variable JVM_LOG_LEVEL.
inline Level min_level() {
  static const Level l = [] {
    if (auto* env = std::getenv("JVM_LOG_LEVEL")) {
      std::string_view s(env);
      if (s == "TRACE") {
        return Level::TRACE;
      }
      if (s == "DEBUG") {
        return Level::DEBUG;
      }
      if (s == "INFO") {
        return Level::INFO;
      }
      if (s == "WARN") {
        return Level::WARN;
      }
      if (s == "ERROR") {
        return Level::ERROR;
      }
    }
    return Level::INFO;
  }();
  return l;
}

/// @brief Return a 5-char label for the given level.
inline const char* level_name(Level lv) {
  switch (lv) {
    case Level::TRACE:
      return "TRACE";
    case Level::DEBUG:
      return "DEBUG";
    case Level::INFO:
      return "INFO ";
    case Level::WARN:
      return "WARN ";
    case Level::ERROR:
      return "ERROR";
    default:
      return "?????";
  }
}

/// @brief Core log function — prints level, file:line, and variadic args to stderr.
template <typename... Args>
void log(Level lv, const char* file, int line, Args&&... args) {
  if (lv < min_level()) {
    return;
  }
  std::cerr << "[" << level_name(lv) << "] " << file << ":" << line << " ";
  ((std::cerr << std::forward<Args>(args)), ...);
  std::cerr << '\n';
}

}  // namespace jvm::log

// ---- User-facing macros ----
#define LOG_TRACE(...) jvm::log::log(jvm::log::Level::TRACE, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_DEBUG(...) jvm::log::log(jvm::log::Level::DEBUG, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_INFO(...)  jvm::log::log(jvm::log::Level::INFO, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_WARN(...)  jvm::log::log(jvm::log::Level::WARN, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_ERROR(...) jvm::log::log(jvm::log::Level::ERROR, __FILE__, __LINE__, __VA_ARGS__)

// ---- Compile-time level stripping ----
// Define JVM_LOG_MAX_LEVEL at build time to compile-out higher verbosity levels.
// Level values: TRACE=0, DEBUG=1, INFO=2, WARN=3, ERROR=4
#ifdef JVM_LOG_MAX_LEVEL
#if JVM_LOG_MAX_LEVEL <= 0
#undef LOG_TRACE
#define LOG_TRACE(...) (void)0
#endif
#if JVM_LOG_MAX_LEVEL <= 1
#undef LOG_DEBUG
#define LOG_DEBUG(...) (void)0
#endif
#if JVM_LOG_MAX_LEVEL <= 2
#undef LOG_INFO
#define LOG_INFO(...) (void)0
#endif
#if JVM_LOG_MAX_LEVEL <= 3
#undef LOG_WARN
#define LOG_WARN(...) (void)0
#endif
#endif
