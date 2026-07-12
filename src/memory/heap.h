#pragma once
#include <cstddef>
#include <vector>

namespace jvm::oops {
class Klass;
class Object;
}  // namespace jvm::oops
namespace jvm::memory {

constexpr size_t kAlignment = 7;
constexpr size_t kHeapSize  = static_cast<const size_t>(64 * 1024 * 1024);

class Heap {
 public:
  static Heap& getSingleton() {
    static Heap instance;
    return instance;
  }

  oops::Object* newInstance(oops::Klass* klass);
  void*         allocate(size_t bytes);

  Heap(const Heap&)            = delete;
  Heap(Heap&&)                 = delete;
  Heap& operator=(const Heap&) = delete;
  Heap& operator=(Heap&&)      = delete;

 private:
  Heap()  = default;
  ~Heap() = default;

  std::vector<std::byte> arena_{kHeapSize};
  size_t                 top_ = 0;
};
}  // namespace jvm::memory