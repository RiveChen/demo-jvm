#include "interpreter_test_base.hpp"
#include "utilities/types.hpp"

#include <gtest/gtest.h>

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

// ============================================================================
// Category-2 (long/double) argument passing through invokestatic — covers A2
// (typed setWide/popWide transfer + per-arg local offsets). Factorial only
// exercises int args, so these are what actually verify the A2 fix.
// ============================================================================

TEST_F(InterpreterMethodInvocationTest, InvokeStatic_LongArgs_Value) {
  EXPECT_EQ(executeStaticMethod<Jlong>(kClassName, "testInvokeSubLong", Jlong{10}, Jlong{3}), 7);
}

TEST_F(InterpreterMethodInvocationTest, InvokeStatic_LongArgs_Ordering) {
  // non-commutative: if a/b were swapped during transfer the sign flips
  EXPECT_EQ(executeStaticMethod<Jlong>(kClassName, "testInvokeSubLong", Jlong{3}, Jlong{10}), -7);
}

TEST_F(InterpreterMethodInvocationTest, InvokeStatic_LongArgs_WideValueSurvives) {
  // value beyond 32-bit range must survive the two-slot transfer intact
  const Jlong big = Jlong{0x1122334455667788};
  EXPECT_EQ(executeStaticMethod<Jlong>(kClassName, "testInvokeSubLong", big, Jlong{0}), big);
}

TEST_F(InterpreterMethodInvocationTest, InvokeStatic_SecondLongArg_Offset) {
  // returns b: the second category-2 arg must land at the callee's slot 2
  EXPECT_EQ(
    executeStaticMethod<Jlong>(kClassName, "testInvokePickSecondLong", Jlong{111}, Jlong{222}),
    222);
}

TEST_F(InterpreterMethodInvocationTest, InvokeStatic_DoubleArgs) {
  EXPECT_DOUBLE_EQ(
    executeStaticMethod<Jdouble>(kClassName, "testInvokeSubDouble", Jdouble{10.5}, Jdouble{3.25}),
    7.25);
}

TEST_F(InterpreterMethodInvocationTest, InvokeStatic_MixedOffsets_IntLongInt) {
  // int(slot0) long(slot1-2) int(slot3): category-2 between two category-1 args
  EXPECT_EQ(
    executeStaticMethod<Jlong>(kClassName, "testInvokePackMix", Jint{1}, Jlong{22}, Jint{333}),
    1022333);
}

}  // namespace
