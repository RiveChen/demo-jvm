#pragma once

#include "types.h"

namespace jvm {
// it will cost 8 bytes (because of alignment) for each slot

union Slot {
  Jint    i;
  Jfloat  f;
  Jlong   l;
  Jdouble d;
  Jref    r;  // object reference
};

// returnAddress is nearly useless in Java SE 8, so we don't support it yet

}  // namespace jvm