#include "heap.h"

#include <new>

#include "oops/klass.h"
#include "oops/object.h"

namespace jvm::memory {
void* Heap::allocate(size_t bytes) {
  bytes = (bytes + 7) & ~size_t{7};
  if (top_ + bytes > arena_.size()) {
    throw std::bad_alloc();
  }
  void* p = arena_.data() + top_;
  top_ += bytes;
  return p;
}

oops::Object* Heap::newInstance(oops::Klass* klass) {
  size_t n    = klass->getInstanceSlotCount();
  size_t size = sizeof(oops::Object) + (n * sizeof(Slot));
  void*  mem  = allocate(size);
  return new (mem) oops::Object(klass);  // placement-new
}
}  // namespace jvm::memory