/**
 * @file slot.h
 * @author Rive Chen
 * @brief Slots in local vars and op stack
 * @version 0.2
 * @date 2025-12-12
 *
 * @copyright Copyright (c) 2025
 *
 */
#pragma once

#include "types.h"

namespace jvm {

// precise-GC's StackMap is too hard to implement, so we use a simple tag to identify the type of
// the slot
enum SlotType : uint8_t { INVALID = 0, INT, FLOAT, REF, LONG, DOUBLE, PADDING };
// INVALID should never be used, it's only for initialization

// sadly, with tag, it will cost 16 bytes (because of alignment) for each slot, but it's more
// flexible and easier to extend
struct Slot {
  SlotType tag;
  union {
    Jint    i;
    Jfloat  f;
    Jlong   l;
    Jdouble d;
    Jref    r;  // object reference
  };
};
// returnAddress is nearly useless in Java SE 8, so we don't support it yet

}  // namespace jvm