/**
 * @file local_variables.hpp
 * @brief Local variable array within a JVM stack frame.
 *
 * Implements the local variables array as described in JVM Specification §2.6.1.
 * Local variables are indexed by unsigned shorts, with category-2 types
 * (long, double) occupying two consecutive indices.
 *
 */
#pragma once

#include "utilities/slot.hpp"

#include <cstddef>
#include <vector>

namespace jvm::runtime {

/**
 * @brief Array of local variables in a stack frame.
 *
 * Stores method arguments and local variables for the duration of a
 * method invocation. The JVM uses 0-based indexing; index 0 is
 * `this` for instance methods, followed by method parameters and
 * user-defined local variables.
 */
class LocalVariables {
 public:
  LocalVariables() = default;
  /// @brief Allocate a local variables array of the given slot count.
  /// @param size Number of slot-sized entries (as specified by max_locals in Code attribute).
  explicit LocalVariables(U2 size) : variables_(size) {}
  LocalVariables(const LocalVariables&)            = delete;
  LocalVariables(LocalVariables&&)                 = default;
  LocalVariables& operator=(const LocalVariables&) = delete;
  LocalVariables& operator=(LocalVariables&&)      = default;
  ~LocalVariables()                                = default;

  /// @brief Return the total number of allocated slots.
  size_t getSize() const { return variables_.size(); }

  /// @name Typed get/set wrappers
  ///@{
  void    setInt(U2 index, Jint value) { setSlot(index, {.i = value}); }
  Jint    getInt(U2 index) const { return getSlot(index).i; }
  void    setFloat(U2 index, Jfloat value) { setSlot(index, {.f = value}); }
  Jfloat  getFloat(U2 index) const { return getSlot(index).f; }
  void    setLong(U2 index, Jlong value) { setWide(index, {.l = value}); }
  Jlong   getLong(U2 index) const { return getWide(index).l; }
  void    setDouble(U2 index, Jdouble value) { setWide(index, {.d = value}); }
  Jdouble getDouble(U2 index) const { return getWide(index).d; }
  void    setRef(U2 index, Jref value) { setSlot(index, {.r = value}); }
  Jref    getRef(U2 index) const { return getSlot(index).r; }
  ///@}

  /// @name Slot-level operations
  ///@{
  /// @brief Set a single slot at the given index.
  void setSlot(U2 index, Slot value) {
    checkBounds(index);
    variables_[index] = value;
  }
  /// @brief Get a single slot at the given index.
  Slot getSlot(U2 index) const {
    checkBounds(index);
    return variables_[index];
  }

  /// @brief Set a category-2 value at the given index, occupying index and index+1.
  void setWide(U2 index, Slot value) {
    checkBounds(index + 1);
    variables_[index]     = value;
    variables_[index + 1] = {.i = 0};  // placeholder for second slot
  }
  /// @brief Get a category-2 value at the given index.
  Slot getWide(U2 index) const {
    checkBounds(index + 1);
    return variables_[index];
  }
  ///@}

 private:
  std::vector<Slot> variables_;

  /// @brief Bounds-check an index and throw if out of range.
  /// @throws std::out_of_range if index >= variables_.size().
  void checkBounds(size_t index) const {
    if (index >= variables_.size()) {
      throw std::out_of_range("Index out of bounds in local variables: " + std::to_string(index));
    }
  }
};

}  // namespace jvm::runtime