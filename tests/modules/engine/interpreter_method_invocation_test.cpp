#include <gtest/gtest.h>

#include "common/types.h"
#include "interpreter_test_base.h"

using namespace jvm;

namespace {

class InterpreterMethodInvocationTest : public InterpreterTestBase {
 public:
  static constexpr const char* kClassName = "tests.data.java.MethodInvocationTest";
};

// ============================================================================
// INVOKESTATIC Tests - Invoke static method (factorial)
// ============================================================================

TEST_F(InterpreterMethodInvocationTest, InvokeStaticFactorial_Zero) {
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testInvokeStaticFactorial", 0), 1);
}

TEST_F(InterpreterMethodInvocationTest, InvokeStaticFactorial_One) {
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testInvokeStaticFactorial", 1), 1);
}

TEST_F(InterpreterMethodInvocationTest, InvokeStaticFactorial_Two) {
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testInvokeStaticFactorial", 2), 2);
}

TEST_F(InterpreterMethodInvocationTest, InvokeStaticFactorial_Three) {
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testInvokeStaticFactorial", 3), 6);
}

TEST_F(InterpreterMethodInvocationTest, InvokeStaticFactorial_Four) {
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testInvokeStaticFactorial", 4), 24);
}

TEST_F(InterpreterMethodInvocationTest, InvokeStaticFactorial_Five) {
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testInvokeStaticFactorial", 5), 120);
}

TEST_F(InterpreterMethodInvocationTest, InvokeStaticFactorial_Six) {
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testInvokeStaticFactorial", 6), 720);
}

TEST_F(InterpreterMethodInvocationTest, InvokeStaticFactorial_Seven) {
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testInvokeStaticFactorial", 7), 5040);
}

TEST_F(InterpreterMethodInvocationTest, InvokeStaticFactorial_Negative) {
  // For negative input, should return 1 (as per implementation)
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testInvokeStaticFactorial", -1), 1);
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testInvokeStaticFactorial", -5), 1);
}

}  // namespace
