/**
 * @file bytecode_reader.hpp
 * @brief Sequential bytecode reader for the interpreter.
 *
 * Reads opcodes and operands from a method's bytecode array.
 * Tracks the program counter (pc) by reference, allowing the
 * interpreter to modify the pc for branch instructions.
 */

#pragma once

#include "utilities/types.hpp"

#include <stdexcept>
#include <vector>

namespace jvm::engine {

/**
 * @brief Sequential reader over a method's bytecode array.
 *
 * Provides typed read operations (U1, U2, U4, signed variants)
 * and pc-alignment for switch instructions. The pc_ is held by
 * reference so that branch instructions can modify it directly.
 */
class BytecodeReader {
 public:
  /**
   * @brief Construct a reader over the given bytecode.
   * @param code The method bytecode array.
   * @param pc Reference to the program counter (modified by reads).
   */
  BytecodeReader(const std::vector<U1>& code, size_t& pc) : code_(code), pc_(pc) {}

  /// @brief Read an unsigned 8-bit value.
  U1 readU1() {
    if (pc_ >= code_.size()) {
      throw std::out_of_range("BytecodeReader: read past end of code");
    }
    return code_[pc_++];
  }

  /// @brief Read an unsigned 16-bit big-endian value.
  U2 readU2() {
    U1 high = readU1();
    U1 low  = readU1();
    return static_cast<U2>(high << 8U) | low;
  }

  /// @brief Read an unsigned 32-bit big-endian value.
  U4 readU4() {
    U2 high = readU2();
    U2 low  = readU2();
    return (static_cast<U4>(high) << 16U) | low;
  }

  /// @brief Read a signed 8-bit value (for BIPUSH offset).
  Jbyte readSU1() { return static_cast<Jbyte>(readU1()); }
  /// @brief Read a signed 16-bit value (for branch offset).
  Jshort readSU2() { return static_cast<Jshort>(readU2()); }
  /// @brief Read a signed 32-bit value (for wide branch).
  Jint readSU4() { return static_cast<Jint>(readU4()); }

  /// @brief Return the current pc value.
  size_t currentPC() const { return pc_; }

  /// @brief Advance the pc by @p n bytes (for skipping operands).
  void skip(size_t n) { pc_ += n; }

  /// @brief Align pc to the next 4-byte boundary (for
  /// TABLESWITCH/LOOKUPSWITCH).
  void align4() {
    while (pc_ % 4 != 0) {
      pc_++;
    }
  }

 private:
  /// @brief Bytecode array reference (non-owning).
  const std::vector<U1>& code_;
  /// @brief Program counter reference (shared with the interpreter loop).
  size_t& pc_;
};

}  // namespace jvm::engine