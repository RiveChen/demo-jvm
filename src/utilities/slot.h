/**
 * @file slot.h
 * @brief Union type representing a Java VM stack slot.
 *
 * Each slot occupies 8 bytes (due to alignment) and can hold any JVM
 * value type: int, float, long, double, or object reference.
 *
 * @note returnAddress is nearly unused in Java SE 8, so it's not supported.
 */

#pragma once

#include "types.h"

namespace jvm {

/// @brief A single JVM stack slot, 8 bytes wide.
///
/// Stores all JVM primitive types and object references in a single
/// memory-aligned union. Accessing an inactive member is permitted
/// for type-punning purposes in this implementation.
union Slot {
  /// @name Slot Accessors (mutually exclusive, one active at a time)
  ///@{
  Jint    i;  ///< Treat slot as a Jint
  Jfloat  f;  ///< Treat slot as a Jfloat
  Jlong   l;  ///< Treat slot as a Jlong (occupies 2 slots)
  Jdouble d;  ///< Treat slot as a Jdouble (occupies 2 slots)
  Jref    r;  ///< Treat slot as an object reference (pointer)
  ///@}
};

}  // namespace jvm