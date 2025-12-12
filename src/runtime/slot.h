/**
 * @file slot.h
 * @author Rive Chen
 * @brief Slots in local vars and op stack
 * @version 0.1
 * @date 2025-12-12
 *
 * @copyright Copyright (c) 2025
 *
 */
#pragma once

#include "common/types.h"

namespace jvm::runtime {

// The size of Slot is 8 bytes, so storing a Jint or Jfloat incurs extra
// space overhead.
struct Slot {
  union {
    Jint    i;
    Jfloat  f;
    Jlong   l;
    Jdouble d;
    Jref    r;  // object reference
  };
};

}  // namespace jvm::runtime