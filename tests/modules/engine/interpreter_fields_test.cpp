#include <gtest/gtest.h>

#include "common/types.h"
#include "interpreter_test_base.h"

using namespace jvm;

namespace {

class InterpreterFieldsTest : public InterpreterTestBase {
 public:
  static constexpr const char* kClassName = "tests.data.java.StaticFieldTest";
};

// ============================================================================
// GETSTATIC Tests - Read static fields
// ============================================================================

TEST_F(InterpreterFieldsTest, GetStaticInt_InitialValue) {
  // Initial value should be 0
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testGetStaticInt"), 0);
}

TEST_F(InterpreterFieldsTest, GetStaticInt_AfterPutStatic) {
  // Set value and get it back
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testPutStaticInt", 42), 42);
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testGetStaticInt"), 42);
}

TEST_F(InterpreterFieldsTest, GetStaticInt_NegativeValue) {
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testPutStaticInt", -100), -100);
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testGetStaticInt"), -100);
}

TEST_F(InterpreterFieldsTest, GetStaticInt_Zero) {
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testPutStaticInt", 0), 0);
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testGetStaticInt"), 0);
}

// ============================================================================
// PUTSTATIC Tests - Write static fields
// ============================================================================

TEST_F(InterpreterFieldsTest, PutStaticInt_Basic) {
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testPutStaticInt", 100), 100);
}

TEST_F(InterpreterFieldsTest, PutStaticInt_UpdateValue) {
  // First set to 10
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testPutStaticInt", 10), 10);
  // Then update to 20
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testPutStaticInt", 20), 20);
  // Verify it's now 20
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testGetStaticInt"), 20);
}

// ============================================================================
// Long static field tests
// ============================================================================

TEST_F(InterpreterFieldsTest, GetStaticLong_InitialValue) {
  EXPECT_EQ(executeStaticMethod<Jlong>(kClassName, "testGetStaticLong"), 0LL);
}

TEST_F(InterpreterFieldsTest, PutStaticLong_Basic) {
  EXPECT_EQ(executeStaticMethod<Jlong>(kClassName, "testPutStaticLong", 123456789LL), 123456789LL);
  EXPECT_EQ(executeStaticMethod<Jlong>(kClassName, "testGetStaticLong"), 123456789LL);
}

TEST_F(InterpreterFieldsTest, PutStaticLong_NegativeValue) {
  EXPECT_EQ(executeStaticMethod<Jlong>(kClassName, "testPutStaticLong", -987654321LL),
            -987654321LL);
  EXPECT_EQ(executeStaticMethod<Jlong>(kClassName, "testGetStaticLong"), -987654321LL);
}

TEST_F(InterpreterFieldsTest, PutStaticLong_LargeValue) {
  EXPECT_EQ(executeStaticMethod<Jlong>(kClassName, "testPutStaticLong", 9223372036854775807LL),
            9223372036854775807LL);
  EXPECT_EQ(executeStaticMethod<Jlong>(kClassName, "testGetStaticLong"), 9223372036854775807LL);
}

// ============================================================================
// Float static field tests
// ============================================================================

TEST_F(InterpreterFieldsTest, GetStaticFloat_InitialValue) {
  EXPECT_FLOAT_EQ(executeStaticMethod<Jfloat>(kClassName, "testGetStaticFloat"), 0.0F);
}

TEST_F(InterpreterFieldsTest, PutStaticFloat_Basic) {
  EXPECT_FLOAT_EQ(executeStaticMethod<Jfloat>(kClassName, "testPutStaticFloat", 3.14F), 3.14F);
  EXPECT_FLOAT_EQ(executeStaticMethod<Jfloat>(kClassName, "testGetStaticFloat"), 3.14F);
}

TEST_F(InterpreterFieldsTest, PutStaticFloat_NegativeValue) {
  EXPECT_FLOAT_EQ(executeStaticMethod<Jfloat>(kClassName, "testPutStaticFloat", -2.5F), -2.5F);
  EXPECT_FLOAT_EQ(executeStaticMethod<Jfloat>(kClassName, "testGetStaticFloat"), -2.5F);
}

TEST_F(InterpreterFieldsTest, PutStaticFloat_Zero) {
  EXPECT_FLOAT_EQ(executeStaticMethod<Jfloat>(kClassName, "testPutStaticFloat", 0.0F), 0.0F);
  EXPECT_FLOAT_EQ(executeStaticMethod<Jfloat>(kClassName, "testGetStaticFloat"), 0.0F);
}

// ============================================================================
// Double static field tests
// ============================================================================

TEST_F(InterpreterFieldsTest, GetStaticDouble_InitialValue) {
  EXPECT_DOUBLE_EQ(executeStaticMethod<Jdouble>(kClassName, "testGetStaticDouble"), 0.0);
}

TEST_F(InterpreterFieldsTest, PutStaticDouble_Basic) {
  EXPECT_DOUBLE_EQ(executeStaticMethod<Jdouble>(kClassName, "testPutStaticDouble", 2.718281828),
                   2.718281828);
  EXPECT_DOUBLE_EQ(executeStaticMethod<Jdouble>(kClassName, "testGetStaticDouble"), 2.718281828);
}

TEST_F(InterpreterFieldsTest, PutStaticDouble_NegativeValue) {
  EXPECT_DOUBLE_EQ(executeStaticMethod<Jdouble>(kClassName, "testPutStaticDouble", -1.41421356),
                   -1.41421356);
  EXPECT_DOUBLE_EQ(executeStaticMethod<Jdouble>(kClassName, "testGetStaticDouble"), -1.41421356);
}

// ============================================================================
// Multiple operations tests
// ============================================================================

TEST_F(InterpreterFieldsTest, MultiplePutGetOperations) {
  // Test multiple PUTSTATIC and GETSTATIC operations in sequence
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testMultipleOperations", 5, 10), 15);
  // After the operation, staticIntField should be 10 (the last value set)
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testGetStaticInt"), 10);
}

TEST_F(InterpreterFieldsTest, InitialValueTest) {
  // Reset by setting a new value
  executeStaticMethod<Jint>(kClassName, "testPutStaticInt", 999);
  // Then test that we can read it back
  // This test method just returns the current value
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testInitialValue"), 999);
}

}  // namespace
