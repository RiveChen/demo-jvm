#include "runtime/klass.h"

#include <gtest/gtest.h>

#include <memory>
#include <string>
#include <vector>

#include "class_loader/class_loader.h"
#include "runtime/method_area.h"

using namespace jvm;

namespace {

class KlassTest : public ::testing::Test {
 protected:
  void SetUp() override {
    test_classpath_ = TEST_CLASS_PATH;
    classpath_list_ = {test_classpath_};
    loader_         = std::make_unique<class_loader::ClassLoader>(nullptr, classpath_list_);
    runtime::MethodArea::getInstance().reset();
  }

  std::string                                test_classpath_;
  std::vector<std::string>                   classpath_list_;
  std::unique_ptr<class_loader::ClassLoader> loader_;
};

}  // namespace

TEST_F(KlassTest, ConstructFromClassFile) {
  std::string class_name = "tests.data.java.KlassTestData";
  auto*       klass      = loader_->loadClass(class_name);

  ASSERT_NE(klass, nullptr);
  EXPECT_EQ(klass->getClassLoader(), loader_.get());
  EXPECT_NE(klass->getClassFile(), nullptr);
}

TEST_F(KlassTest, FindMethod) {
  std::string class_name = "tests.data.java.KlassTestData";
  auto*       klass      = loader_->loadClass(class_name);
  ASSERT_NE(klass, nullptr);

  // <init>()V exists
  auto* ctor = klass->findMethod("<init>", "()V");
  ASSERT_NE(ctor, nullptr);
  EXPECT_EQ(ctor->getName(), "<init>");
  EXPECT_EQ(ctor->getDescriptor(), "()V");
  EXPECT_EQ(ctor->getOwnerKlass(), klass);

  // add(II)I exists
  auto* add_method = klass->findMethod("add", "(II)I");
  ASSERT_NE(add_method, nullptr);
  EXPECT_FALSE(add_method->isStatic());

  // staticAdd(II)I exists and is static
  auto* static_add_method = klass->findMethod("staticAdd", "(II)I");
  ASSERT_NE(static_add_method, nullptr);
  EXPECT_TRUE(static_add_method->isStatic());

  // non-existing method
  EXPECT_EQ(klass->findMethod("notExist", "()V"), nullptr);
}

TEST_F(KlassTest, FindField) {
  std::string class_name = "tests.data.java.KlassTestData";
  auto*       klass      = loader_->loadClass(class_name);
  ASSERT_NE(klass, nullptr);

  // instance fields
  auto* x_field = klass->findField("x", "I");
  ASSERT_NE(x_field, nullptr);
  EXPECT_FALSE(x_field->isStatic());
  EXPECT_EQ(x_field->getSlotIndex(), 0);

  auto* y_field = klass->findField("y", "J");
  ASSERT_NE(y_field, nullptr);
  EXPECT_FALSE(y_field->isStatic());
  EXPECT_EQ(y_field->getSlotIndex(), 1);

  auto* dz_field = klass->findField("dz", "D");
  ASSERT_NE(dz_field, nullptr);
  EXPECT_FALSE(dz_field->isStatic());
  EXPECT_EQ(dz_field->getSlotIndex(), 3);

  // static fields
  auto* sx_field = klass->findField("sx", "I");
  ASSERT_NE(sx_field, nullptr);
  EXPECT_TRUE(sx_field->isStatic());
  EXPECT_EQ(sx_field->getSlotIndex(), 0);

  auto* sd_field = klass->findField("sd", "D");
  ASSERT_NE(sd_field, nullptr);
  EXPECT_TRUE(sd_field->isStatic());
  EXPECT_EQ(sd_field->getSlotIndex(), 1);

  // non-existing field
  EXPECT_EQ(klass->findField("notExist", "I"), nullptr);
}

TEST_F(KlassTest, SlotCountsAndStatics) {
  std::string class_name = "tests.data.java.KlassTestData";
  auto*       klass      = loader_->loadClass(class_name);
  ASSERT_NE(klass, nullptr);

  // instance fields: x(int)=1, y(long)=2, dz(double)=2 => total 5
  EXPECT_EQ(klass->getInstanceSlotCount(), 5);
  // static fields: sx(int)=1, sd(double)=2 => total 3
  EXPECT_EQ(klass->getStaticSlotCount(), 3);
}

TEST_F(KlassTest, InterfacesAndSuperClassDefaults) {
  std::string class_name = "tests.data.java.HelloWorld";
  auto*       klass      = loader_->loadClass(class_name);
  ASSERT_NE(klass, nullptr);

  // Interfaces vector sized but unset; should contain only nullptrs
  for (auto* iface : klass->getInterfaces()) {
    EXPECT_EQ(iface, nullptr);
  }

  // Super class is not linked yet by ClassLoader::loadClass
  EXPECT_EQ(klass->getSuperClass(), nullptr);
}
