/**
 * @file local_variables.h
 * @author Rive Chen
 * @brief Local variables in frame
 * @version 0.1
 * @date 2025-12-12
 *
 * @copyright Copyright (c) 2025
 *
 */
#pragma once

#include <cstddef>
#include <vector>

#include "utilities/slot.h"

namespace jvm::runtime {

class LocalVariables {
 public:
  LocalVariables() = default;
  explicit LocalVariables(U2 size) : variables_(size) {}
  LocalVariables(const LocalVariables&)            = delete;
  LocalVariables(LocalVariables&&)                 = default;
  LocalVariables& operator=(const LocalVariables&) = delete;
  LocalVariables& operator=(LocalVariables&&)      = default;
  ~LocalVariables()                                = default;

  size_t getSize() const { return variables_.size(); }

  void    setInt(U2 index, Jint value) { setSlot(index, {.i = value}); }
  Jint    getInt(U2 index) { return getSlot(index).i; }
  void    setFloat(U2 index, Jfloat value) { setSlot(index, {.f = value}); }
  Jfloat  getFloat(U2 index) { return getSlot(index).f; }
  void    setLong(U2 index, Jlong value) { setWide(index, {.l = value}); }
  Jlong   getLong(U2 index) { return getWide(index).l; }
  void    setDouble(U2 index, Jdouble value) { setWide(index, {.d = value}); }
  Jdouble getDouble(U2 index) { return getWide(index).d; }
  void    setRef(U2 index, Jref value) { setSlot(index, {.r = value}); }
  Jref    getRef(U2 index) { return getSlot(index).r; }

  void setSlot(U2 index, Slot value) {
    checkBounds(index);
    variables_[index] = value;
  }
  Slot getSlot(U2 index) {
    checkBounds(index);
    return variables_[index];
  }

  void setWide(U2 index, Slot value) {
    // a wide value occupies index and index+1; checking the higher one covers both
    checkBounds(static_cast<size_t>(index) + 1);
    variables_[index]     = value;
    variables_[index + 1] = {.i = 0};
  }
  Slot getWide(U2 index) {
    checkBounds(index);
    return variables_[index];
  }

 private:
  std::vector<Slot> variables_;

  void checkBounds(size_t index) {
    if (index >= variables_.size()) {
      throw std::out_of_range("Index out of bounds in local variables: " + std::to_string(index));
    }
  }
};

}  // namespace jvm::runtime