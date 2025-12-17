#pragma once

#include <vector>

#include "common/types.h"

namespace jvm::engine {
class BytecodeReader {
 public:
  BytecodeReader(const std::vector<U1>& code, size_t& pc) : code_(code), pc_(pc) {}

  U1 readU1() { return code_[pc_++]; }

  U2 readU2() {
    U1 high = code_[pc_++];
    U1 low  = code_[pc_++];
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
    return static_cast<U2>(high << 8U) | low;
    // high << 8U will happen Integral Promotion
  }

  U4 readU4() {
    U2 high = readU2();
    U2 low  = readU2();
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
    return (static_cast<U4>(high) << 16U) | low;
  }

  // in C++20, static cast from unsigned to signed is guaranteed to be safe, it's just
  // reinterpretation
  // maybe we should define new typees like SU1 SU2 SU4
  // for now, we just use JVM types here
  // the signed versions are used for bytecode offsets, for control flow instructions
  Jbyte  readSU1() { return static_cast<Jbyte>(readU1()); }
  Jshort readSU2() { return static_cast<Jshort>(readU2()); }
  Jint   readSU4() { return static_cast<Jint>(readU4()); }

  size_t currentPC() const { return pc_; }
  void   skip(size_t n) { pc_ += n; }

  // align to 4 bytes for switch instruction
  void align4() {
    while (pc_ % 4 != 0) {
      pc_++;
    }
  }

 private:
  // considering the lifetime is determined, we can use reference here
  // NOLINTBEGIN(cppcoreguidelines-avoid-const-or-ref-data-members)
  const std::vector<U1>& code_;
  size_t&                pc_;
  // NOLINTEND(cppcoreguidelines-avoid-const-or-ref-data-members)
};

}  // namespace jvm::engine