#include "runtime/method_area.h"

#include <gtest/gtest.h>

#include <memory>
#include <string>
#include <vector>

#include "class_loader/class_loader.h"
#include "runtime/klass.h"

using namespace jvm;

class MethodAreaTest : public ::testing::Test {
 protected:
  void SetUp() override {
    // Set up test classpath
    test_classpath_ = TEST_CLASS_PATH;
    classpath_list_ = {test_classpath_};
    loader_         = std::make_unique<class_loader::ClassLoader>(nullptr, classpath_list_);
    // Reset method area because it is a global singleton
    runtime::MethodArea::getInstance().reset();
  }

  void TearDown() override { loader_.reset(); }

  std::string                                test_classpath_;
  std::vector<std::string>                   classpath_list_;
  std::unique_ptr<class_loader::ClassLoader> loader_;
};

TEST_F(MethodAreaTest, SingletonInstance) {
  // Test that getInstance returns the same instance
  runtime::MethodArea& instance1 = runtime::MethodArea::getInstance();
  runtime::MethodArea& instance2 = runtime::MethodArea::getInstance();

  EXPECT_EQ(&instance1, &instance2) << "MethodArea should be a singleton";
}

TEST_F(MethodAreaTest, AddAndGetClass) {
  // Load a class which will be added to method area
  std::string class_name = "tests.data.java.HelloWorld";
  auto*       klass      = loader_->loadClass(class_name);

  ASSERT_NE(klass, nullptr);

  runtime::MethodArea&                 method_area = runtime::MethodArea::getInstance();
  runtime::MethodArea::ClassIdentifier identifier  = std::make_pair(loader_.get(), class_name);

  // Verify class is in method area
  EXPECT_TRUE(method_area.hasClass(identifier)) << "Class should be in method area";

  // Retrieve class from method area
  auto* retrieved_klass = method_area.getClass(identifier);
  EXPECT_EQ(retrieved_klass, klass) << "Retrieved class should match";
}

TEST_F(MethodAreaTest, HasClass) {
  runtime::MethodArea& method_area = runtime::MethodArea::getInstance();

  // Test with non-existent class
  runtime::MethodArea::ClassIdentifier non_existent =
    std::make_pair(loader_.get(), "NonExistentClass");
  EXPECT_FALSE(method_area.hasClass(non_existent)) << "Non-existent class should not be found";

  // Load a class and verify it exists
  std::string class_name = "tests.data.java.HelloWorld";
  auto*       klass      = loader_->loadClass(class_name);
  ASSERT_NE(klass, nullptr);

  runtime::MethodArea::ClassIdentifier identifier = std::make_pair(loader_.get(), class_name);
  EXPECT_TRUE(method_area.hasClass(identifier)) << "Loaded class should exist in method area";
}

TEST_F(MethodAreaTest, GetClassReturnsNullptrForNonExistent) {
  runtime::MethodArea& method_area = runtime::MethodArea::getInstance();

  runtime::MethodArea::ClassIdentifier non_existent =
    std::make_pair(loader_.get(), "NonExistentClass");

  auto* klass = method_area.getClass(non_existent);
  EXPECT_EQ(klass, nullptr) << "Non-existent class should return nullptr";
}

TEST_F(MethodAreaTest, DifferentClassLoadersSameClassName) {
  // Create two different class loaders
  auto loader1 = std::make_unique<class_loader::ClassLoader>(nullptr, classpath_list_);
  auto loader2 = std::make_unique<class_loader::ClassLoader>(nullptr, classpath_list_);

  std::string class_name = "tests.data.java.HelloWorld";

  // Load same class with different loaders
  auto* klass1 = loader1->loadClass(class_name);
  auto* klass2 = loader2->loadClass(class_name);

  ASSERT_NE(klass1, nullptr);
  ASSERT_NE(klass2, nullptr);

  runtime::MethodArea& method_area = runtime::MethodArea::getInstance();

  // They should be stored separately in method area
  runtime::MethodArea::ClassIdentifier id1 = std::make_pair(loader1.get(), class_name);
  runtime::MethodArea::ClassIdentifier id2 = std::make_pair(loader2.get(), class_name);

  EXPECT_TRUE(method_area.hasClass(id1));
  EXPECT_TRUE(method_area.hasClass(id2));

  EXPECT_NE(method_area.getClass(id1), method_area.getClass(id2))
    << "Different class loaders should create different class instances";
}
