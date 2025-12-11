/**
 * @file version.h
 * @author Rive Chen
 * @brief Version for representing class file's version
 * @version 0.1
 * @date 2025-12-11
 *
 * @copyright Copyright (c) 2025
 *
 */
#pragma once

#include <string>

#include "common/types.h"

namespace class_loader {

class Version {
 public:
  Version(common::U2 major, common::U2 minor) : major_(major), minor_(minor) {}
  Version(const Version&)            = default;
  Version(Version&&)                 = default;
  Version& operator=(const Version&) = default;
  Version& operator=(Version&&)      = default;
  ~Version()                         = default;

  common::U2 getMajor() const { return major_; }
  common::U2 getMinor() const { return minor_; }

  std::string toString() const { return std::to_string(major_) + "." + std::to_string(minor_); }

 private:
  common::U2 major_;
  common::U2 minor_;
};

}  // namespace class_loader