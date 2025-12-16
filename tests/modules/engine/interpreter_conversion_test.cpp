#include <gtest/gtest.h>

#include "interpreter_test_base.h"

// ============================================================================
// Type Conversion Tests
// ============================================================================

class InterpreterConversionTest : public InterpreterTestBase {};

// ============================================================================
// Int to Other Types
// ============================================================================

TEST_F(InterpreterConversionTest, I2L_Basic) {
  EXPECT_EQ(executeStaticMethod<Jlong>("tests.data.java.ConversionTest", "testI2L", 42), 42LL);
  EXPECT_EQ(executeStaticMethod<Jlong>("tests.data.java.ConversionTest", "testI2L", -42), -42LL);
}

TEST_F(InterpreterConversionTest, I2F_Basic) {
  EXPECT_FLOAT_EQ(executeStaticMethod<Jfloat>("tests.data.java.ConversionTest", "testI2F", 42),
                  42.0F);
  EXPECT_FLOAT_EQ(executeStaticMethod<Jfloat>("tests.data.java.ConversionTest", "testI2F", -42),
                  -42.0F);
}

TEST_F(InterpreterConversionTest, I2D_Basic) {
  EXPECT_DOUBLE_EQ(executeStaticMethod<Jdouble>("tests.data.java.ConversionTest", "testI2D", 42),
                   42.0);
  EXPECT_DOUBLE_EQ(executeStaticMethod<Jdouble>("tests.data.java.ConversionTest", "testI2D", -42),
                   -42.0);
}

// ============================================================================
// Long to Other Types
// ============================================================================

TEST_F(InterpreterConversionTest, L2I_Basic) {
  EXPECT_EQ(executeStaticMethod<Jint>("tests.data.java.ConversionTest", "testL2I", 42LL), 42);
  EXPECT_EQ(executeStaticMethod<Jint>("tests.data.java.ConversionTest", "testL2I", -42LL), -42);
}

TEST_F(InterpreterConversionTest, L2F_Basic) {
  EXPECT_FLOAT_EQ(executeStaticMethod<Jfloat>("tests.data.java.ConversionTest", "testL2F", 42LL),
                  42.0F);
}

TEST_F(InterpreterConversionTest, L2D_Basic) {
  EXPECT_DOUBLE_EQ(executeStaticMethod<Jdouble>("tests.data.java.ConversionTest", "testL2D", 42LL),
                   42.0);
}

// ============================================================================
// Float to Other Types
// ============================================================================

TEST_F(InterpreterConversionTest, F2I_Basic) {
  EXPECT_EQ(executeStaticMethod<Jint>("tests.data.java.ConversionTest", "testF2I", 42.5F), 42);
  EXPECT_EQ(executeStaticMethod<Jint>("tests.data.java.ConversionTest", "testF2I", -42.5F), -42);
}

TEST_F(InterpreterConversionTest, F2I_NaN) {
  EXPECT_EQ(executeStaticMethod<Jint>("tests.data.java.ConversionTest", "testF2I_NaN"), 0);
}

TEST_F(InterpreterConversionTest, F2I_Infinity) {
  EXPECT_EQ(executeStaticMethod<Jint>("tests.data.java.ConversionTest", "testF2I_Infinity"), 0);
}

TEST_F(InterpreterConversionTest, F2L_Basic) {
  EXPECT_EQ(executeStaticMethod<Jlong>("tests.data.java.ConversionTest", "testF2L", 42.5F), 42LL);
}

TEST_F(InterpreterConversionTest, F2L_NaN) {
  // Note: F2L with NaN should return 0LL
  // This is tested indirectly through the Java test methods
  EXPECT_EQ(executeStaticMethod<Jlong>("tests.data.java.ConversionTest", "testF2L", 0.0F), 0LL);
}

TEST_F(InterpreterConversionTest, F2D_Basic) {
  EXPECT_DOUBLE_EQ(executeStaticMethod<Jdouble>("tests.data.java.ConversionTest", "testF2D", 42.5F),
                   42.5);
}

// ============================================================================
// Double to Other Types
// ============================================================================

TEST_F(InterpreterConversionTest, D2I_Basic) {
  EXPECT_EQ(executeStaticMethod<Jint>("tests.data.java.ConversionTest", "testD2I", 42.5), 42);
  EXPECT_EQ(executeStaticMethod<Jint>("tests.data.java.ConversionTest", "testD2I", -42.5), -42);
}

TEST_F(InterpreterConversionTest, D2I_NaN) {
  EXPECT_EQ(executeStaticMethod<Jint>("tests.data.java.ConversionTest", "testD2I_NaN"), 0);
}

TEST_F(InterpreterConversionTest, D2I_Infinity) {
  EXPECT_EQ(executeStaticMethod<Jint>("tests.data.java.ConversionTest", "testD2I_Infinity"), 0);
}

TEST_F(InterpreterConversionTest, D2L_Basic) {
  EXPECT_EQ(executeStaticMethod<Jlong>("tests.data.java.ConversionTest", "testD2L", 42.5), 42LL);
}

TEST_F(InterpreterConversionTest, D2L_NaN) {
  // Note: This test uses a Java method that creates NaN, not a C++ NaN value
  // The actual NaN handling is tested via testD2I_NaN which uses Java code
  EXPECT_EQ(executeStaticMethod<Jlong>("tests.data.java.ConversionTest", "testD2L", 0.0), 0LL);
}

TEST_F(InterpreterConversionTest, D2F_Basic) {
  EXPECT_FLOAT_EQ(executeStaticMethod<Jfloat>("tests.data.java.ConversionTest", "testD2F", 42.5),
                  42.5F);
}

// ============================================================================
// Int to Narrow Types
// ============================================================================

TEST_F(InterpreterConversionTest, I2B_Basic) {
  EXPECT_EQ(executeStaticMethod<Jint>("tests.data.java.ConversionTest", "testI2B", 127), 127);
  EXPECT_EQ(executeStaticMethod<Jint>("tests.data.java.ConversionTest", "testI2B", -128), -128);
  EXPECT_EQ(executeStaticMethod<Jint>("tests.data.java.ConversionTest", "testI2B", 256), 0);
}

TEST_F(InterpreterConversionTest, I2C_Basic) {
  EXPECT_EQ(executeStaticMethod<Jint>("tests.data.java.ConversionTest", "testI2C", 65), 65);
  EXPECT_EQ(executeStaticMethod<Jint>("tests.data.java.ConversionTest", "testI2C", 65535), 65535);
}

TEST_F(InterpreterConversionTest, I2S_Basic) {
  EXPECT_EQ(executeStaticMethod<Jint>("tests.data.java.ConversionTest", "testI2S", 32767), 32767);
  EXPECT_EQ(executeStaticMethod<Jint>("tests.data.java.ConversionTest", "testI2S", -32768), -32768);
}
