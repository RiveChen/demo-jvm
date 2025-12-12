#pragma once

#include <cstdint>

namespace jvm {

// Aliases for JVM specification types
using U1 = uint8_t;
using U2 = uint16_t;
using U4 = uint32_t;
using U8 = uint64_t;

// Aliases for Java's primitive types used in the runtime
using Jbyte    = int8_t;
using Jshort   = int16_t;
using Jint     = int32_t;
using Jlong    = int64_t;
using Jchar    = uint16_t;  // Java char is 16-bit UTF-16, so unsigned
using Jfloat   = float;
using Jdouble  = double;
using Jboolean = uint8_t;  // JVM often uses 8-bit int for booleans in arrays
using Jref     = void*;    // Not implemented yet

}  // namespace jvm