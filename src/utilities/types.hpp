/**
 * @file types.hpp
 * @brief Cross-platform type aliases for JVM specification types and Java primitives.
 *
 * All JVM-specified unsigned integral types (u1, u2, u4, u8) and
 * Java primitive type aliases used throughout the runtime.
 */

#pragma once

#include <cstdint>

namespace jvm {

/// @name JVM Specification Types
/// These map directly to the types defined in the JVM Specification §2.3.
///@{
using U1 = uint8_t;   ///< u1: unsigned 8-bit integer
using U2 = uint16_t;  ///< u2: unsigned 16-bit integer
using U4 = uint32_t;  ///< u4: unsigned 32-bit integer
using U8 = uint64_t;  ///< u8: unsigned 64-bit integer
///@}

/// @name Java Primitive Type Aliases
/// These correspond to the primitive types used in the Java Virtual Machine.
///@{
using Jbyte    = int8_t;    ///< byte: signed 8-bit  (JVM §2.3.1)
using Jshort   = int16_t;   ///< short: signed 16-bit (JVM §2.3.1)
using Jint     = int32_t;   ///< int: signed 32-bit (JVM §2.3.1)
using Jlong    = int64_t;   ///< long: signed 64-bit (JVM §2.3.1)
using Jchar    = uint16_t;  ///< char: unsigned 16-bit UTF-16 code unit (JVM §2.3.1)
using Jfloat   = float;     ///< float: 32-bit IEEE 754 single precision (JVM §2.3.2)
using Jdouble  = double;    ///< double: 64-bit IEEE 754 double precision (JVM §2.3.2)
using Jboolean = uint8_t;   ///< boolean: 8-bit integer representing true/false (JVM §2.3.4)
using Jref     = void*;     ///< reference: opaque pointer to a heap-allocated object
///@}

}  // namespace jvm