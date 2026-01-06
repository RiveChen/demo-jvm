/**
 * @file access_flags.hpp
 * @author Rive Chen
 * @brief Access flags for field, method and class
 * @version 0.1
 * @date 2025-12-12
 *
 * @copyright Copyright (c) 2025
 *
 */
#pragma once

#include "types.h"

namespace jvm {

namespace flags {

// --- Class Access Flags (JVM Spec §4.1) ---
// From JVM Specification Table 4.1-A. class access and property modifiers
enum class Class : U2 {
  PUBLIC = 0x0001,  // Declared public; may be accessed from outside its package.
  FINAL  = 0x0010,  // Declared final; no subclasses allowed.
  SUPER =
    0x0020,  // Treat superclass methods specially when invoked by the invokespecial instruction.
  INTERFACE  = 0x0200,  // Is an interface, not a class.
  ABSTRACT   = 0x0400,  // Declared abstract; must not be instantiated.
  SYNTHETIC  = 0x1000,  // Declared synthetic; not present in the source code.
  ANNOTATION = 0x2000,  // Declared as an annotation type.
  ENUM       = 0x4000,  // Declared as an enum type.
};

// --- Method Access Flags (JVM Spec §4.6) ---
enum class Method : U2 {
  PUBLIC       = 0x0001,
  PRIVATE      = 0x0002,
  PROTECTED    = 0x0004,
  STATIC       = 0x0008,
  FINAL        = 0x0010,
  SYNCHRONIZED = 0x0020,
  BRIDGE       = 0x0040,
  VARARGS      = 0x0080,
  NATIVE       = 0x0100,
  ABSTRACT     = 0x0400,
  STRICT       = 0x0800,
  SYNTHETIC    = 0x1000,
};

// --- Field Access Flags (JVM Spec §4.5) ---
enum class Field : U2 {
  PUBLIC    = 0x0001,
  PRIVATE   = 0x0002,
  PROTECTED = 0x0004,
  STATIC    = 0x0008,
  FINAL     = 0x0010,
  VOLATILE  = 0x0040,
  TRANSIENT = 0x0080,
  SYNTHETIC = 0x1000,
  ENUM      = 0x4000,
};
}  // namespace flags

template <typename FlagEnum>
class AccessFlags {
 public:
  AccessFlags() : flags_(0) {}
  explicit AccessFlags(U2 flags) : flags_(flags) {}

  bool has(FlagEnum flag) const { return flags_ & static_cast<U2>(flag); }

  U2 getFlags() const { return flags_; }

 private:
  U2 flags_;
};

}  // namespace jvm