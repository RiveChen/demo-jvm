/**
 * @file object.h
 * @brief Runtime representation of a Java object instance.
 *
 * Models a Java heap-allocated object with a Klass pointer,
 * a MarkWord (for synchronization/GC metadata), and an inline
 * array of field slots.
 */

#pragma once

#include "klass.hpp"
#include "utilities/slot.hpp"
#include "utilities/types.hpp"

#include <cstddef>
#include <cstdint>

namespace jvm::oops {

/**
 * @brief The MarkWord of a Java object header.
 *
 * Inspired by HotSpot's MarkWord, this stores object metadata
 * including identity hash code and locking information.
 * Currently used as a placeholder for future GC/synchronization support.
 */
class MarkWord {
 public:
  MarkWord() = default;
  explicit MarkWord(uintptr_t data) : bits_(data) {}
  uintptr_t raw() const { return bits_; }

 private:
  uintptr_t bits_{};
};

/**
 * @brief Base class for all heap-allocated objects.
 *
 * Every Java object (both instances and arrays) starts with
 * a Klass pointer and a MarkWord, matching HotSpot's two-word header.
 */
// NOLINTBEGIN(cppcoreguidelines-pro-bounds-pointer-arithmetic,
//              cppcoreguidelines-pro-type-reinterpret-cast)
class OopDesc {
 protected:
  Klass*   klass_;  ///< Pointer to the object's runtime class (any Klass subtype).
  MarkWord mark_;   ///< Object header metadata.

 public:
  explicit OopDesc(Klass* klass) : klass_(klass), mark_({}) {}
  OopDesc(Klass* klass, MarkWord mark) : klass_(klass), mark_(mark) {}

  /// @brief The runtime class of this object.
  Klass*  getKlass() const { return klass_; }
  MarkWord& getMarkword() { return mark_; }
};

/**
 * @brief A Java object instance on the heap.
 *
 * Objects are allocated by Heap::newInstance() and consist of:
 * - A Klass pointer (pointer to the object's class)
 * - A MarkWord (metadata for locking/GC)
 * - Instance field slots stored contiguously after the header
 *
 * The field slots are accessed via pointer arithmetic:
 * `fields() = reinterpret_cast<Slot*>(this + 1)`
 */
class InstanceOopDesc : public OopDesc {
 public:
  explicit InstanceOopDesc(InstanceKlass* klass) : OopDesc(klass) {}

  /// @brief Access a field slot by index.
  Slot& fieldSlot(size_t i) { return fields()[i]; }

  /// @name Typed field accessors
  ///@{
  Jint   getInt(size_t i) { return fieldSlot(i).i; }
  void   setInt(size_t i, Jint v) { fieldSlot(i) = {.i = v}; }
  Jfloat getFloat(size_t i) { return fieldSlot(i).f; }
  void   setFloat(size_t i, Jfloat v) { fieldSlot(i) = {.f = v}; }
  Jref   getRef(size_t i) { return fieldSlot(i).r; }
  void   setRef(size_t i, Jref v) { fieldSlot(i) = {.r = v}; }
  Jlong  getLong(size_t i) { return fieldSlot(i).l; }
  void   setLong(size_t i, Jlong v) {
    fieldSlot(i)     = {.l = v};
    fieldSlot(i + 1) = {.i = 0};  // placeholder for second slot
  }
  Jdouble getDouble(size_t i) { return fieldSlot(i).d; }
  void    setDouble(size_t i, Jdouble v) {
    fieldSlot(i)     = {.d = v};
    fieldSlot(i + 1) = {.i = 0};  // placeholder for second slot
  }
  ///@}

 private:
  // Followed by instance_slot_count * Slot inline
  /// Compute the pointer to the first field slot (after the header).
  Slot*       fields() { return reinterpret_cast<Slot*>(this + 1); }
  const Slot* fields() const { return reinterpret_cast<const Slot*>(this + 1); }
};

/**
 * @brief A Java array on the heap.
 *
 * Layout: [ Klass* | MarkWord | Jint length | element[N] ]
 * The `base()` method returns the address of element[0].
 */
class ArrayOopDesc : public OopDesc {
 public:
  explicit ArrayOopDesc(Klass* klass) : OopDesc(klass) {}

  /// @brief The number of elements in this array.
  Jint  length() { return *reinterpret_cast<Jint*>(this + 1); }
  void  setLength(Jint l) { *reinterpret_cast<Jint*>(this + 1) = l; }

  /// @brief Pointer to the first element (after the length field).
  void* base() { return reinterpret_cast<char*>(this + 1) + sizeof(Jint); }
};
// NOLINTEND(cppcoreguidelines-pro-bounds-pointer-arithmetic,
//            cppcoreguidelines-pro-type-reinterpret-cast)

}  // namespace jvm::oops