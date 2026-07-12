#include "oops/constant_pool.h"

#include <gtest/gtest.h>

#include <memory>
#include <string>
#include <variant>
#include <vector>

#include "classfile/class_loader.h"
#include "oops/klass.h"
#include "oops/method_area.h"

using namespace jvm;

namespace {

class ConstantPoolTest : public ::testing::Test {
 protected:
  void SetUp() override {
    test_classpath_ = TEST_CLASS_PATH;
    classpath_list_ = {test_classpath_};
    loader_         = std::make_unique<classfile::ClassLoader>(nullptr, classpath_list_);
    oops::MethodArea::getInstance().reset();
  }

  static constexpr const char* kClassName = "tests.data.java.KlassTestData";

  std::string                             test_classpath_;
  std::vector<std::string>                classpath_list_;
  std::unique_ptr<classfile::ClassLoader> loader_;
};

}  // namespace

// SymRef entries are self-sufficient (baked strings + runtime-cp class index);
// resolution must not touch the ClassFile. Each test plants a SymRef and resolves it.

TEST_F(ConstantPoolTest, ResolveClassCachesResult) {
  auto* klass = loader_->loadClass(kClassName);
  ASSERT_NE(klass, nullptr);

  auto& rcp = klass->getRuntimeConstantPool();
  rcp.setConstant(1, oops::SymRef_Class{.class_name = kClassName});

  auto* resolved_first  = rcp.resolveClass(1);
  auto* resolved_second = rcp.resolveClass(1);

  EXPECT_EQ(resolved_first, klass);
  EXPECT_EQ(resolved_first, resolved_second);
  // resolved result is cached back into the slot
  EXPECT_TRUE(std::holds_alternative<oops::Klass*>(rcp.getConstant(1)));
}

TEST_F(ConstantPoolTest, ResolveMethodAndCache) {
  auto* klass = loader_->loadClass(kClassName);
  ASSERT_NE(klass, nullptr);

  auto& rcp = klass->getRuntimeConstantPool();
  // slot 1: the Class entry the method ref points at (via class_cp_index)
  rcp.setConstant(1, oops::SymRef_Class{.class_name = kClassName});
  rcp.setConstant(2, oops::SymRef_Method{.class_cp_index = 1,
                                         .member_name    = "add",
                                         .descriptor     = "(II)I"});

  auto* resolved_first  = rcp.resolveMethod(2);
  auto* resolved_second = rcp.resolveMethod(2);

  ASSERT_NE(resolved_first, nullptr);
  EXPECT_EQ(resolved_first->getName(), "add");
  EXPECT_EQ(resolved_first->getDescriptor(), "(II)I");
  EXPECT_EQ(resolved_first->getOwnerKlass(), klass);
  EXPECT_EQ(resolved_first, resolved_second);
  EXPECT_TRUE(std::holds_alternative<oops::Method*>(rcp.getConstant(2)));
}

TEST_F(ConstantPoolTest, ResolveFieldAndCache) {
  auto* klass = loader_->loadClass(kClassName);
  ASSERT_NE(klass, nullptr);

  auto& rcp = klass->getRuntimeConstantPool();
  rcp.setConstant(1, oops::SymRef_Class{.class_name = kClassName});
  rcp.setConstant(3, oops::SymRef_Field{.class_cp_index = 1,
                                        .member_name    = "sd",
                                        .descriptor     = "D"});

  auto* resolved_first  = rcp.resolveField(3);
  auto* resolved_second = rcp.resolveField(3);

  ASSERT_NE(resolved_first, nullptr);
  EXPECT_EQ(resolved_first->getName(), "sd");
  EXPECT_EQ(resolved_first->getDescriptor(), "D");
  EXPECT_TRUE(resolved_first->isStatic());
  EXPECT_EQ(resolved_first->getOwnerKlass(), klass);
  EXPECT_EQ(resolved_first, resolved_second);
  EXPECT_TRUE(std::holds_alternative<oops::Field*>(rcp.getConstant(3)));
}
