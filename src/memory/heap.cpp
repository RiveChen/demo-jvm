/**
 * @file heap.cpp
 * @brief Bump-pointer heap allocation implementation.
 */

#include "heap.hpp"

#include "oops/klass.hpp"
#include "oops/object.hpp"
#include "utilities/logger.hpp"

#include <new>

namespace jvm::memory {
void* Heap::allocate(size_t bytes) {
  bytes = (bytes + (kAlignment - 1)) & ~(kAlignment - 1);
  LOG_TRACE("Heap allocate: ", bytes, " bytes at offset ", top_);
  if (top_ + bytes > arena_.size()) {
    throw std::bad_alloc();
  }
  void* p = arena_.data() + top_;
  top_ += bytes;
  return p;
}

oops::InstanceOopDesc* Heap::newInstance(oops::InstanceKlass* klass) {
  size_t n    = klass->getInstanceSlotCount();
  size_t size = sizeof(oops::InstanceOopDesc) + (n * sizeof(Slot));
  LOG_TRACE("Heap newInstance: ", klass->getName(), " (", n, " slots, ", size, " bytes)");
  void* mem = allocate(size);
  return new (mem) oops::InstanceOopDesc(klass);  // placement-new
}
}  // namespace jvm::memory