#include "oops/constant_pool.hpp"

#include <gtest/gtest.h>

#include <memory>
#include <optional>
#include <string>
#include <variant>
#include <vector>

#include "classfile/class_loader.hpp"
#include "oops/klass.hpp"
#include "oops/method_area.hpp"

using namespace jvm;

namespace {

class ConstantPoolTest : public ::testing::Test {
 protected:
  void SetUp() override {
    test_classpath_ = TEST_CLASS_PATH;
    classpath_list_ = {test_classpath_};
    loader_         = std::make_unique<classfile::ClassLoader>(nullptr, classpath_list_);
    oops::MethodArea::getSingleton().reset();
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
  rcp.setConstant(
    2, oops::SymRef_Method{.class_cp_index = 1, .member_name = "add", .descriptor = "(II)I"});

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
  rcp.setConstant(3,
                  oops::SymRef_Field{.class_cp_index = 1, .member_name = "sd", .descriptor = "D"});

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

// symbolicKey builds "class.member descriptor" from an unresolved SymRef without
// resolving. The format must match what registerStubIntercepts() binds.
TEST_F(ConstantPoolTest, SymbolicKeyFromMethodAndFieldRefs) {
  auto* klass = loader_->loadClass(kClassName);
  ASSERT_NE(klass, nullptr);
  auto& rcp = klass->getRuntimeConstantPool();

  // class names are the internal slash form; the key must match registerStubIntercepts()
  rcp.setConstant(1, oops::SymRef_Class{.class_name = "java/io/PrintStream"});
  rcp.setConstant(2, oops::SymRef_Method{.class_cp_index = 1,
                                         .member_name    = "println",
                                         .descriptor     = "(Ljava/lang/String;)V"});
  auto mkey = rcp.symbolicKey(2);
  ASSERT_TRUE(mkey.has_value());
  EXPECT_EQ(*mkey, "java/io/PrintStream.println (Ljava/lang/String;)V");

  rcp.setConstant(3, oops::SymRef_Class{.class_name = "java/lang/System"});
  rcp.setConstant(4, oops::SymRef_Field{.class_cp_index = 3,
                                        .member_name    = "out",
                                        .descriptor     = "Ljava/io/PrintStream;"});
  auto fkey = rcp.symbolicKey(4);
  ASSERT_TRUE(fkey.has_value());
  EXPECT_EQ(*fkey, "java/lang/System.out Ljava/io/PrintStream;");
}

// A resolved slot (Method*) is not symbolic -> no key (keeps the invoke hot path cheap).
TEST_F(ConstantPoolTest, SymbolicKeyNulloptForResolvedSlot) {
  auto* klass = loader_->loadClass(kClassName);
  ASSERT_NE(klass, nullptr);
  auto& rcp = klass->getRuntimeConstantPool();

  rcp.setConstant(1, oops::SymRef_Class{.class_name = kClassName});
  rcp.setConstant(2, oops::SymRef_Method{.class_cp_index = 1,
                                         .member_name    = "add",
                                         .descriptor     = "(II)I"});
  ASSERT_TRUE(rcp.symbolicKey(2).has_value());  // symbolic before resolve
  rcp.resolveMethod(2);                          // slot 2 now holds Method*
  EXPECT_FALSE(rcp.symbolicKey(2).has_value());  // resolved -> nullopt
}

// Regression: the target Class entry may already be resolved (Klass*) by another ref
// to the same class while this method ref is still symbolic. symbolicKey must return
// nullopt, not throw bad_variant_access.
TEST_F(ConstantPoolTest, SymbolicKeyNoThrowWhenTargetClassResolved) {
  auto* klass = loader_->loadClass(kClassName);
  ASSERT_NE(klass, nullptr);
  auto& rcp = klass->getRuntimeConstantPool();

  rcp.setConstant(1, oops::SymRef_Class{.class_name = kClassName});
  rcp.setConstant(2, oops::SymRef_Method{.class_cp_index = 1,
                                         .member_name    = "add",
                                         .descriptor     = "(II)I"});
  rcp.resolveClass(1);  // slot 1 (class) -> Klass*, slot 2 still SymRef_Method

  std::optional<std::string> key;
  ASSERT_NO_THROW(key = rcp.symbolicKey(2));
  EXPECT_FALSE(key.has_value());
}
