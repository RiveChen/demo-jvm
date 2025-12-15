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

#include <vector>

#include "runtime/slot.h"

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

  U2 getSize() const { return variables_.size(); }

  void setInt(U2 index, Jint value) {
    checkBounds(index);
    variables_[index].i = value;
  }
  Jint getInt(U2 index) {
    checkBounds(index);
    return variables_[index].i;
  }
  void setFloat(U2 index, Jfloat value) {
    checkBounds(index);
    variables_[index].f = value;
  }
  Jfloat getFloat(U2 index) {
    checkBounds(index);
    return variables_[index].f;
  }
  void setLong(U2 index, Jlong value) {
    checkBounds(index);
    variables_[index].l = value;
  }
  Jlong getLong(U2 index) {
    checkBounds(index);
    return variables_[index].l;
  }
  void setDouble(U2 index, Jdouble value) {
    checkBounds(index);
    variables_[index].d = value;
  }
  Jdouble getDouble(U2 index) {
    checkBounds(index);
    return variables_[index].d;
  }
  void setRef(U2 index, Jref value) {
    checkBounds(index);
    variables_[index].r = value;
  }
  Jref getRef(U2 index) {
    checkBounds(index);
    return variables_[index].r;
  }

 private:
  std::vector<Slot> variables_;

  void checkBounds(U2 index) {
    if (index >= variables_.size()) {
      throw std::out_of_range("Index out of bounds in local variables: " + std::to_string(index));
    }
  }
};

}  // namespace jvm::runtime