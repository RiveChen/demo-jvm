#pragma once
#include <cstddef>
#include <cstdint>

#include "utilities/slot.h"
namespace jvm::oops {
class Klass;

class MarkWord {
 public:
  MarkWord() = default;
  explicit MarkWord(uintptr_t data) : bits_(data) {};
  uintptr_t raw() const { return bits_; }

 private:
  uintptr_t bits_{};
};

// NOLINTBEGIN(cppcoreguidelines-pro-bounds-pointer-arithmetic,cppcoreguidelines-pro-type-reinterpret-cast)
class Object {
 public:
  explicit Object(Klass* klass) : klass_(klass), mark_(0) {}

  Klass* getKlass() const { return klass_; }

  Slot& fieldSlot(size_t i) { return fields()[i]; }

  // typed accessors
  Jint   getInt(size_t i) { return fieldSlot(i).i; }
  void   setInt(size_t i, Jint v) { fieldSlot(i) = {.i = v}; }
  Jfloat getFloat(size_t i) { return fieldSlot(i).f; }
  void   setFloat(size_t i, Jfloat v) { fieldSlot(i) = {.f = v}; }
  Jref   getRef(size_t i) { return fieldSlot(i).r; }
  void   setRef(size_t i, Jref v) { fieldSlot(i) = {.r = v}; }
  Jlong  getLong(size_t i) { return fieldSlot(i).l; }
  void   setLong(size_t i, Jlong v) {
    fieldSlot(i)     = {.l = v};
    fieldSlot(i + 1) = {.i = 0};
  }
  Jdouble getDouble(size_t i) { return fieldSlot(i).d; }
  void    setDouble(size_t i, Jdouble v) {
    fieldSlot(i)     = {.d = v};
    fieldSlot(i + 1) = {.i = 0};
  }

 private:
  Klass*   klass_;
  MarkWord mark_;
  // followed by instance_slot_count * Slot

  Slot*       fields() { return reinterpret_cast<Slot*>(this + 1); }
  const Slot* fields() const { return reinterpret_cast<const Slot*>(this + 1); }
};
// NOLINTEND(cppcoreguidelines-pro-bounds-pointer-arithmetic,cppcoreguidelines-pro-type-reinterpret-cast)
}  // namespace jvm::oops