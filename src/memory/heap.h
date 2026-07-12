#pragma once
#include <cstddef>
#include <vector>

namespace jvm::oops {
class Klass;
class Object;
}  // namespace jvm::oops
namespace jvm::memory {

class Heap {
 public:
  explicit Heap(size_t capacity_bytes) : arena_(capacity_bytes) {}

  oops::Object* newInstance(oops::Klass* klass);
  void*         allocate(size_t bytes);

 private:
  std::vector<std::byte> arena_;
  size_t                 top_ = 0;
};
}  // namespace jvm::memory