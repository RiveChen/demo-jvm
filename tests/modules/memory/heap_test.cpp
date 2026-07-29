#include "memory/heap.hpp"

#include "classfile/class_loader.hpp"
#include "oops/klass.hpp"
#include "oops/method_area.hpp"
#include "oops/object.hpp"

#include <cstdint>
#include <gtest/gtest.h>
#include <memory>
#include <string>
#include <vector>

using namespace jvm;

namespace {

// Heap/Object exercised directly (not through the interpreter), since NEW's
// end-to-end path still needs INVOKESPECIAL (<init>). KlassTestData has
// instance fields: int x, long y, double dz -> 5 instance slots.
class HeapTest : public ::testing::Test {
 protected:
  void SetUp() override {
    test_classpath_ = TEST_CLASS_PATH;
    classpath_list_ = {test_classpath_};
    loader_         = std::make_unique<classfile::ClassLoader>(nullptr, classpath_list_);
    oops::MethodArea::getSingleton().reset();
    klass_ = loader_->loadClass("tests.data.java.KlassTestData");
    ASSERT_NE(klass_, nullptr);
  }

  // resolve slot indices robustly (don't hardcode the layout)
  size_t xIndex() const { return klass_->findField("x", "I")->getSlotIndex(); }
  size_t yIndex() const { return klass_->findField("y", "J")->getSlotIndex(); }
  size_t zIndex() const { return klass_->findField("dz", "D")->getSlotIndex(); }

  oops::Klass*                            klass_ = nullptr;
  std::string                             test_classpath_;
  std::vector<std::string>                classpath_list_;
  std::unique_ptr<classfile::ClassLoader> loader_;
};

}  // namespace

TEST_F(HeapTest, NewInstanceSetsKlass) {
  auto* obj = memory::Heap::getSingleton().newInstance(klass_);
  ASSERT_NE(obj, nullptr);
  EXPECT_EQ(obj->getKlass(), klass_);
}

TEST_F(HeapTest, FieldsDefaultToZero) {
  auto* obj = memory::Heap::getSingleton().newInstance(klass_);
  ASSERT_NE(obj, nullptr);
  EXPECT_EQ(obj->getInt(xIndex()), 0);
  EXPECT_EQ(obj->getLong(yIndex()), 0);
  EXPECT_DOUBLE_EQ(obj->getDouble(zIndex()), 0.0);
}

TEST_F(HeapTest, FieldRoundtrip) {
  auto* obj = memory::Heap::getSingleton().newInstance(klass_);

  obj->setInt(xIndex(), 42);
  obj->setLong(yIndex(), 0x1122334455667788LL);  // 64-bit value survives the wide slot
  obj->setDouble(zIndex(), 2.5);

  EXPECT_EQ(obj->getInt(xIndex()), 42);
  EXPECT_EQ(obj->getLong(yIndex()), 0x1122334455667788LL);
  EXPECT_DOUBLE_EQ(obj->getDouble(zIndex()), 2.5);
}

TEST_F(HeapTest, DistinctObjectsDoNotAlias) {
  auto* a = memory::Heap::getSingleton().newInstance(klass_);
  auto* b = memory::Heap::getSingleton().newInstance(klass_);
  ASSERT_NE(a, nullptr);
  ASSERT_NE(b, nullptr);
  EXPECT_NE(a, b);  // bump allocator hands out distinct addresses

  a->setInt(xIndex(), 111);
  b->setInt(xIndex(), 222);
  EXPECT_EQ(a->getInt(xIndex()), 111);
  EXPECT_EQ(b->getInt(xIndex()), 222);  // b's field is independent of a's
}

TEST_F(HeapTest, RefAccessorRoundtrip) {
  // KlassTestData has no reference instance field; exercise the ref accessor
  // mechanics by storing another object pointer in a slot.
  auto* obj   = memory::Heap::getSingleton().newInstance(klass_);
  auto* other = memory::Heap::getSingleton().newInstance(klass_);
  obj->setRef(xIndex(), static_cast<Jref>(other));
  EXPECT_EQ(obj->getRef(xIndex()), static_cast<Jref>(other));
}
