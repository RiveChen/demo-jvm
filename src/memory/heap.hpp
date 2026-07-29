/**
 * @file heap.h
 * @brief Simple bump-pointer object heap.
 *
 * Implements the heap as described in JVM Specification §2.5.3.
 * Currently uses a pre-allocated arena with a bump-pointer allocator.
 * No garbage collection is implemented.
 *
 * @note This is an educational implementation. A production JVM would
 *       include generational garbage collection, compaction, etc.
 */

#pragma once
#include <cstddef>
#include <vector>

namespace jvm::oops {
class Klass;
class InstanceOopDesc;
}  // namespace jvm::oops
namespace jvm::memory {

/// Alignment mask for heap allocation (8-byte alignment).
constexpr size_t kAlignment = 7;
/// Maximum heap size (64 MB).
constexpr size_t kHeapSize = static_cast<const size_t>(64 * 1024 * 1024);

/**
 * @brief Singleton bump-pointer heap allocator.
 *
 * Allocates objects from a pre-allocated arena. Allocation is O(1)
 * but there is currently no deallocation or garbage collection.
 * The heap will throw an exception when exhausted.
 */
class Heap {
 public:
  /// Meyer's singleton (thread-safe since C++11).
  static Heap& getSingleton() {
    static Heap instance;
    return instance;
  }

  /// @brief Allocate and construct a new Object of the given class.
  /// @param klass The runtime class of the object to create.
  /// @return Pointer to the newly allocated object.
  oops::InstanceOopDesc* newInstance(oops::Klass* klass);

  /// @brief Allocate raw memory of the given size (aligned).
  /// @param bytes Number of bytes to allocate.
  /// @return Pointer to the allocated memory.
  void* allocate(size_t bytes);

  Heap(const Heap&)            = delete;
  Heap(Heap&&)                 = delete;
  Heap& operator=(const Heap&) = delete;
  Heap& operator=(Heap&&)      = delete;

 private:
  Heap()  = default;
  ~Heap() = default;

  std::vector<std::byte> arena_{kHeapSize};  ///< The pre-allocated heap memory.
  size_t                 top_ = 0;           ///< Current allocation position (bump pointer).
};

}  // namespace jvm::memory