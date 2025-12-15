#include "runtime/constant_pool.h"

#include <gtest/gtest.h>

#include <memory>
#include <optional>
#include <string>
#include <variant>
#include <vector>

#include "class_loader/class_file.h"
#include "class_loader/class_loader.h"
#include "class_loader/constant_pool.h"
#include "runtime/klass.h"
#include "runtime/method_area.h"

using namespace jvm;

namespace {

class ConstantPoolTest : public ::testing::Test {
 protected:
  void SetUp() override {
    test_classpath_ = TEST_CLASS_PATH;
    classpath_list_ = {test_classpath_};
    loader_         = std::make_unique<class_loader::ClassLoader>(nullptr, classpath_list_);
    runtime::MethodArea::getInstance().reset();
  }

  std::optional<U2> findNameAndTypeIndex(class_loader::ClassFile* class_file,
                                         const std::string& name, const std::string& descriptor) {
    const auto& cp = class_file->constant_pool;
    for (size_t i = 1; i < cp.size(); i++) {
      auto* info = dynamic_cast<const class_loader::NameAndTypeInfo*>(cp.getConstantInfo(i));
      if (info == nullptr) {
        continue;
      }
      // std::cerr << "Name: " << cp.getUtf8String(info->name_index) << std::endl;
      // std::cerr << "Descriptor: " << cp.getUtf8String(info->descriptor_index) << std::endl;
      if (cp.getUtf8String(info->name_index) == name &&
          cp.getUtf8String(info->descriptor_index) == descriptor) {
        return i;
      }
    }
    return std::nullopt;
  }

  std::string                                test_classpath_;
  std::vector<std::string>                   classpath_list_;
  std::unique_ptr<class_loader::ClassLoader> loader_;
};

}  // namespace

TEST_F(ConstantPoolTest, ResolveClassCachesResult) {
  auto* klass = loader_->loadClass("tests.data.java.KlassTestData");
  ASSERT_NE(klass, nullptr);

  auto* class_file = klass->getClassFile();
  auto* class_info = dynamic_cast<const class_loader::ClassInfo*>(
    class_file->constant_pool.getConstantInfo(class_file->this_class_index));
  ASSERT_NE(class_info, nullptr);

  auto& rcp = klass->getRuntimeConstantPool();
  rcp.setConstant(1, runtime::SymRef_Class{.name_index = class_info->name_index});

  auto* resolved_first  = rcp.resolveClass(1);
  auto* resolved_second = rcp.resolveClass(1);

  EXPECT_EQ(resolved_first, klass);
  EXPECT_EQ(resolved_first, resolved_second);
  EXPECT_TRUE(std::holds_alternative<runtime::Klass*>(rcp.getConstant(1)));
}

TEST_F(ConstantPoolTest, ResolveNameAndType) {
  auto* klass = loader_->loadClass("tests.data.java.KlassTestData");
  ASSERT_NE(klass, nullptr);

  auto* class_file = klass->getClassFile();
  auto  nt_index   = findNameAndTypeIndex(class_file, "add", "(II)I");
  ASSERT_TRUE(nt_index.has_value());

  auto [name, descriptor] = klass->getRuntimeConstantPool().resolveNameAndType(nt_index.value());
  EXPECT_EQ(name, "add");
  EXPECT_EQ(descriptor, "(II)I");
}

TEST_F(ConstantPoolTest, ResolveMethodAndCache) {
  auto* klass = loader_->loadClass("tests.data.java.KlassTestData");
  ASSERT_NE(klass, nullptr);

  auto* class_file = klass->getClassFile();

  // find NameAndType for add(II)I
  auto nt_index = findNameAndTypeIndex(class_file, "add", "(II)I");
  ASSERT_TRUE(nt_index.has_value());

  // reuse this class's own Class entry for class_index
  auto class_index = class_file->this_class_index;

  auto& rcp = klass->getRuntimeConstantPool();
  rcp.setConstant(
    2, runtime::SymRef_Method{.class_index = class_index, .name_and_type_index = nt_index.value()});

  auto* resolved_first  = rcp.resolveMethod(2);
  auto* resolved_second = rcp.resolveMethod(2);

  ASSERT_NE(resolved_first, nullptr);
  EXPECT_EQ(resolved_first->getName(), "add");
  EXPECT_EQ(resolved_first->getDescriptor(), "(II)I");
  EXPECT_EQ(resolved_first->getOwnerKlass(), klass);
  EXPECT_EQ(resolved_first, resolved_second);
  EXPECT_TRUE(std::holds_alternative<runtime::Method*>(rcp.getConstant(2)));
}

TEST_F(ConstantPoolTest, ResolveFieldAndCache) {
  auto* klass = loader_->loadClass("tests.data.java.KlassTestData");
  ASSERT_NE(klass, nullptr);

  auto* class_file = klass->getClassFile();

  // static field sd:D
  auto nt_index = findNameAndTypeIndex(class_file, "sd", "D");
  ASSERT_TRUE(nt_index.has_value());

  auto class_index = class_file->this_class_index;

  auto& rcp = klass->getRuntimeConstantPool();
  rcp.setConstant(
    3, runtime::SymRef_Field{.class_index = class_index, .name_and_type_index = nt_index.value()});

  auto* resolved_first  = rcp.resolveField(3);
  auto* resolved_second = rcp.resolveField(3);

  ASSERT_NE(resolved_first, nullptr);
  EXPECT_EQ(resolved_first->getName(), "sd");
  EXPECT_EQ(resolved_first->getDescriptor(), "D");
  EXPECT_TRUE(resolved_first->isStatic());
  EXPECT_EQ(resolved_first->getOwnerKlass(), klass);
  EXPECT_EQ(resolved_first, resolved_second);
  EXPECT_TRUE(std::holds_alternative<runtime::Field*>(rcp.getConstant(3)));
}
