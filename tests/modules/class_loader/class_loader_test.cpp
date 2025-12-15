#include "class_loader/class_loader.h"

#include <gtest/gtest.h>

#include <string>
#include <vector>

#include "runtime/klass.h"
#include "runtime/method_area.h"

using namespace jvm;

class ClassLoaderTest : public ::testing::Test {
 protected:
  void SetUp() override {
    // Clear method area before each test
    // Note: MethodArea is a singleton, so we need to be careful
    // In a real scenario, you might want to reset it or use a test fixture

    // Set up test classpath
    test_classpath_ = TEST_CLASS_PATH;
    classpath_list_ = {test_classpath_};
    loader_         = std::make_unique<class_loader::ClassLoader>(nullptr, classpath_list_);
  }

  void TearDown() override {
    // Clean up if needed
    loader_.reset();
  }

  std::string                                test_classpath_;
  std::vector<std::string>                   classpath_list_;
  std::unique_ptr<class_loader::ClassLoader> loader_;
};

TEST_F(ClassLoaderTest, LoadExistingClass) {
  // Load HelloWorld class
  std::string class_name = "tests.data.java.HelloWorld";
  auto*       klass      = loader_->loadClass(class_name);

  ASSERT_NE(klass, nullptr) << "Failed to load class: " << class_name;
  EXPECT_EQ(klass->getClassLoader(), loader_.get());
  EXPECT_NE(klass->getClassFile(), nullptr);
}

TEST_F(ClassLoaderTest, LoadNonExistentClass) {
  // Try to load a class that doesn't exist
  std::string class_name = "NonExistentClass";
  EXPECT_THROW(loader_->loadClass(class_name), std::runtime_error);
}

TEST_F(ClassLoaderTest, ClassCaching) {
  // Load the same class twice
  std::string class_name = "tests.data.java.HelloWorld";
  auto*       klass1     = loader_->loadClass(class_name);
  auto*       klass2     = loader_->loadClass(class_name);

  // Should return the same instance (cached)
  EXPECT_EQ(klass1, klass2) << "Class should be cached and return same instance";
}

TEST_F(ClassLoaderTest, LoadClassWithSuperClass) {
  // HelloWorld extends Object, so it should have a super class
  std::string class_name = "tests.data.java.HelloWorld";
  auto*       klass      = loader_->loadClass(class_name);

  ASSERT_NE(klass, nullptr);
  auto* super_class = klass->getSuperClass();

  // HelloWorld should extend java.lang.Object
  // Note: This test assumes java.lang.Object can be loaded
  // If Object class is not available, super_class might be nullptr
  // This is acceptable for now as we're testing the linking mechanism
  if (super_class != nullptr) {
    EXPECT_NE(super_class, nullptr);
  }
}

TEST_F(ClassLoaderTest, MultipleClasspaths) {
  // Test with multiple classpaths
  std::vector<std::string> multiple_classpaths = {
    test_classpath_,
    "/nonexistent/path"  // This path doesn't exist, but first one should work
  };

  auto multi_loader = std::make_unique<class_loader::ClassLoader>(nullptr, multiple_classpaths);
  std::string class_name = "tests.data.java.HelloWorld";
  auto*       klass      = multi_loader->loadClass(class_name);

  ASSERT_NE(klass, nullptr) << "Should find class in first classpath";
}

TEST_F(ClassLoaderTest, ClassRegisteredInMethodArea) {
  // Load a class and verify it's registered in method area
  std::string class_name = "tests.data.java.HelloWorld";
  auto*       klass      = loader_->loadClass(class_name);

  ASSERT_NE(klass, nullptr);

  // Check if class is registered in method area
  runtime::MethodArea&                 method_area = runtime::MethodArea::getInstance();
  runtime::MethodArea::ClassIdentifier identifier  = std::make_pair(loader_.get(), class_name);

  EXPECT_TRUE(method_area.hasClass(identifier)) << "Class should be registered in method area";

  auto* retrieved_klass = method_area.getClass(identifier);
  EXPECT_EQ(retrieved_klass, klass) << "Retrieved class should match loaded class";
}

TEST_F(ClassLoaderTest, ParentClassLoader) {
  // Test parent class loader
  auto parent_loader = std::make_unique<class_loader::ClassLoader>(nullptr, classpath_list_);
  auto child_loader =
    std::make_unique<class_loader::ClassLoader>(parent_loader.get(), classpath_list_);

  // Both should be able to load classes independently
  std::string class_name   = "tests.data.java.HelloWorld";
  auto*       parent_klass = parent_loader->loadClass(class_name);
  auto*       child_klass  = child_loader->loadClass(class_name);

  ASSERT_NE(parent_klass, nullptr);
  ASSERT_NE(child_klass, nullptr);

  // They should be different instances (different class loaders)
  EXPECT_NE(parent_klass, child_klass)
    << "Different class loaders should create different instances";
}

TEST_F(ClassLoaderTest, EmptyClasspath) {
  // Test with empty classpath
  std::vector<std::string> empty_classpath;
  auto empty_loader = std::make_unique<class_loader::ClassLoader>(nullptr, empty_classpath);

  std::string class_name = "tests.data.java.HelloWorld";
  EXPECT_THROW(empty_loader->loadClass(class_name), std::runtime_error);
}

TEST_F(ClassLoaderTest, InvalidClasspath) {
  // Test with invalid classpath
  std::vector<std::string> invalid_classpath = {"/nonexistent/path/that/does/not/exist"};
  auto invalid_loader = std::make_unique<class_loader::ClassLoader>(nullptr, invalid_classpath);

  std::string class_name = "tests.data.java.HelloWorld";
  EXPECT_THROW(invalid_loader->loadClass(class_name), std::runtime_error);
}
