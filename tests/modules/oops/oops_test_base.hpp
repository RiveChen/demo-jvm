/**
 * @file oops_test_base.hpp
 * @brief Shared fixture for oops-layer tests.
 *
 * Provides a ClassLoader over the compiled test classes and resets the global
 * MethodArea before each test. Tests that need loaded classes derive from this
 * instead of repeating the identical setup. (string_pool_test needs no loader
 * and stays standalone.)
 */

#pragma once

#include "classfile/class_loader.hpp"
#include "oops/method_area.hpp"

#include <gtest/gtest.h>

#include <memory>
#include <string>
#include <vector>

class OopsTestBase : public ::testing::Test {
 protected:
  void SetUp() override {
    test_classpath_ = TEST_CLASS_PATH;
    classpath_list_ = {test_classpath_};
    loader_         = std::make_unique<jvm::classfile::ClassLoader>(nullptr, classpath_list_);
    // MethodArea is a global singleton; clear both instance and array klasses.
    jvm::oops::MethodArea::getSingleton().reset();
  }

  void TearDown() override { loader_.reset(); }

  std::string                                  test_classpath_;
  std::vector<std::string>                     classpath_list_;
  std::unique_ptr<jvm::classfile::ClassLoader> loader_;
};
