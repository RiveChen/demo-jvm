/**
 * @file version.h
 * @brief Class file version information.
 *
 * Represents a class file's major/minor version as specified in
 * JVM Specification §4.1 ("The ClassFile Structure").
 *
 */
#pragma once

#include <string>

#include "utilities/types.hpp"

namespace jvm::classfile {

/**
 * @brief Class file version pair (major.minor).
 *
 * Used to check that the JVM can understand a given class file.
 */
class Version {
 public:
  Version(U2 major, U2 minor) : major_(major), minor_(minor) {}
  Version(const Version&)            = delete;
  Version(Version&&)                 = default;
  Version& operator=(const Version&) = delete;
  Version& operator=(Version&&)      = default;
  ~Version()                         = default;

  /// @brief Get the major version number.
  U2 getMajor() const { return major_; }
  /// @brief Get the minor version number.
  U2 getMinor() const { return minor_; }
  /// @brief Format as "major.minor".
  std::string toString() const { return std::to_string(major_) + "." + std::to_string(minor_); }

 private:
  U2 major_;  ///< Major version number.
  U2 minor_;  ///< Minor version number.
};

}  // namespace jvm::classfile