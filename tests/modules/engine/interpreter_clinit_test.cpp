#include "interpreter_test_base.hpp"
#include "oops/klass.hpp"
#include "utilities/types.hpp"

#include <gtest/gtest.h>

using namespace jvm;

namespace {

class InterpreterClinitTest : public InterpreterTestBase {
 public:
  static constexpr const char* kClassName  = "tests.data.java.ClinitTest";
  static constexpr const char* kFieldClass = "tests.data.java.StaticFieldTest";
};

// ============================================================================
// ConstantValue fields (static final compile-time constants)
// Set during preparation, NOT by <clinit>
// ============================================================================

TEST_F(InterpreterClinitTest, ConstantValueInt) {
  // CONST_INT = 42 is a ConstantValue attribute — set during linking, before
  // <clinit>
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "getConstInt"), 42);
}

// ============================================================================
// <clinit> triggered by GETSTATIC
// ============================================================================

TEST_F(InterpreterClinitTest, GetStaticTriggersClinit) {
  // staticIntFromClinit is initialized in <clinit> = 100.
  // The first GETSTATIC should trigger <clinit>, then return 100.
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "getIntFromClinit"), 100);
}

TEST_F(InterpreterClinitTest, GetStaticLongFromClinit) {
  // staticLongFromClinit is initialized in <clinit> = 200L
  EXPECT_EQ(executeStaticMethod<Jlong>(kClassName, "getLongFromClinit"), 200L);
}

// ============================================================================
// <clinit> triggered by PUTSTATIC
// ============================================================================

TEST_F(InterpreterClinitTest, PutStaticTriggersClinit) {
  // PUTSTATIC to staticIntFromClinit should trigger <clinit> first (setting it
  // to 100), then the method's own assignment sets it to 42.
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "putThenGetInt", 42), 42);
}

// ============================================================================
// After <clinit>, class is FullyInitialized — subsequent calls don't re-trigger
// ============================================================================

TEST_F(InterpreterClinitTest, ClinitExecutesOnlyOnce) {
  // First call triggers <clinit> → staticIntFromClinit = 100
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "getIntFromClinit"), 100);

  // Second call: class already fully initialized, skip <clinit>, read stale
  // value. The value persists because this test shares the same loader. After
  // first call the class is FullyInitialized; read should still be 100.
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "getIntFromClinit"), 100);
}

// ============================================================================
// Klass::getState() reflects correct initialization state
// ============================================================================

TEST_F(InterpreterClinitTest, KlassStateAfterLoad) {
  // Load class without executing any method
  auto* klass = loader_->loadClass(kClassName);
  ASSERT_NE(klass, nullptr);

  // After loadClass + link, the class should be Linked, not yet initialized
  EXPECT_EQ(klass->getState(), oops::InstanceKlass::Linked);
}

TEST_F(InterpreterClinitTest, KlassStateAfterFirstActiveUse) {
  // Load class and trigger first active use via GETSTATIC
  auto* klass = loader_->loadClass(kClassName);
  ASSERT_NE(klass, nullptr);

  EXPECT_EQ(klass->getState(), oops::InstanceKlass::Linked);

  // Trigger <clinit> by executing a method that reads a field initialized in
  // <clinit>
  executeStaticMethod<Jint>(kClassName, "getIntFromClinit");

  // After first access, class should be fully initialized
  EXPECT_EQ(klass->getState(), oops::InstanceKlass::FullyInitialized);
}

// ============================================================================
// Klass with no <clinit> stays Linked until first active use
// ============================================================================

TEST_F(InterpreterClinitTest, ClassWithoutClinitStaysLinked) {
  auto* klass = loader_->loadClass(kFieldClass);
  ASSERT_NE(klass, nullptr);

  // StaticFieldTest has no <clinit>, so it stays Linked until a trigger.
  EXPECT_EQ(klass->getState(), oops::InstanceKlass::Linked);

  // First active use should initialize it immediately (no <clinit> to run).
  executeStaticMethod<Jint>(kFieldClass, "testGetStaticInt");
  EXPECT_EQ(klass->getState(), oops::InstanceKlass::FullyInitialized);
}

}  // namespace