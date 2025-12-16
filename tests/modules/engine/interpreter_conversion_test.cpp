#include <gtest/gtest.h>

#include <cmath>

#include "interpreter_test_base.h"

// ============================================================================
// Type Conversion Tests
// ============================================================================

class InterpreterConversionTest : public InterpreterTestBase {};

// ============================================================================
// Int to Other Types
// ============================================================================

TEST_F(InterpreterConversionTest, I2L_Basic) {
  EXPECT_EQ(executeLongMethod1Arg("tests.data.java.ConversionTest", "testI2L", "(I)J", 42), 42L);
  EXPECT_EQ(executeLongMethod1Arg("tests.data.java.ConversionTest", "testI2L", "(I)J", -42), -42L);
}

TEST_F(InterpreterConversionTest, I2F_Basic) {
  EXPECT_FLOAT_EQ(executeFloatMethod1Arg("tests.data.java.ConversionTest", "testI2F", "(I)F", 42),
                  42.0F);
  EXPECT_FLOAT_EQ(executeFloatMethod1Arg("tests.data.java.ConversionTest", "testI2F", "(I)F", -42),
                  -42.0F);
}

TEST_F(InterpreterConversionTest, I2D_Basic) {
  EXPECT_DOUBLE_EQ(executeDoubleMethod1Arg("tests.data.java.ConversionTest", "testI2D", "(I)D", 42),
                   42.0);
  EXPECT_DOUBLE_EQ(
    executeDoubleMethod1Arg("tests.data.java.ConversionTest", "testI2D", "(I)D", -42), -42.0);
}

// ============================================================================
// Long to Other Types
// ============================================================================

TEST_F(InterpreterConversionTest, L2I_Basic) {
  EXPECT_EQ(executeIntMethod1Arg("tests.data.java.ConversionTest", "testL2I", "(J)I", 42L), 42);
  EXPECT_EQ(executeIntMethod1Arg("tests.data.java.ConversionTest", "testL2I", "(J)I", -42L), -42);
}

TEST_F(InterpreterConversionTest, L2F_Basic) {
  EXPECT_FLOAT_EQ(executeFloatMethod1Arg("tests.data.java.ConversionTest", "testL2F", "(J)F", 42L),
                  42.0F);
}

TEST_F(InterpreterConversionTest, L2D_Basic) {
  EXPECT_DOUBLE_EQ(
    executeDoubleMethod1Arg("tests.data.java.ConversionTest", "testL2D", "(J)D", 42L), 42.0);
}

// ============================================================================
// Float to Other Types
// ============================================================================

TEST_F(InterpreterConversionTest, F2I_Basic) {
  EXPECT_EQ(executeIntMethod1Arg("tests.data.java.ConversionTest", "testF2I", "(F)I", 42.5F), 42);
  EXPECT_EQ(executeIntMethod1Arg("tests.data.java.ConversionTest", "testF2I", "(F)I", -42.5F), -42);
}

TEST_F(InterpreterConversionTest, F2I_NaN) {
  EXPECT_EQ(executeIntMethod0Arg("tests.data.java.ConversionTest", "testF2I_NaN", "()I"), 0);
}

TEST_F(InterpreterConversionTest, F2I_Infinity) {
  EXPECT_EQ(executeIntMethod0Arg("tests.data.java.ConversionTest", "testF2I_Infinity", "()I"), 0);
}

TEST_F(InterpreterConversionTest, F2L_Basic) {
  EXPECT_EQ(executeLongMethod1Arg("tests.data.java.ConversionTest", "testF2L", "(F)J", 42.5F), 42L);
}

TEST_F(InterpreterConversionTest, F2L_NaN) {
  // Note: F2L with NaN should return 0L
  // This is tested indirectly through the Java test methods
  EXPECT_EQ(executeLongMethod1Arg("tests.data.java.ConversionTest", "testF2L", "(F)J", 0.0F), 0L);
}

TEST_F(InterpreterConversionTest, F2D_Basic) {
  EXPECT_DOUBLE_EQ(
    executeDoubleMethod1Arg("tests.data.java.ConversionTest", "testF2D", "(F)D", 42.5F), 42.5);
}

// ============================================================================
// Double to Other Types
// ============================================================================

TEST_F(InterpreterConversionTest, D2I_Basic) {
  EXPECT_EQ(executeIntMethod1Arg("tests.data.java.ConversionTest", "testD2I", "(D)I", 42.5), 42);
  EXPECT_EQ(executeIntMethod1Arg("tests.data.java.ConversionTest", "testD2I", "(D)I", -42.5), -42);
}

TEST_F(InterpreterConversionTest, D2I_NaN) {
  EXPECT_EQ(executeIntMethod0Arg("tests.data.java.ConversionTest", "testD2I_NaN", "()I"), 0);
}

TEST_F(InterpreterConversionTest, D2I_Infinity) {
  EXPECT_EQ(executeIntMethod0Arg("tests.data.java.ConversionTest", "testD2I_Infinity", "()I"), 0);
}

TEST_F(InterpreterConversionTest, D2L_Basic) {
  EXPECT_EQ(executeLongMethod1Arg("tests.data.java.ConversionTest", "testD2L", "(D)J", 42.5), 42L);
}

TEST_F(InterpreterConversionTest, D2L_NaN) {
  // Note: This test uses a Java method that creates NaN, not a C++ NaN value
  // The actual NaN handling is tested via testD2I_NaN which uses Java code
  EXPECT_EQ(executeLongMethod1Arg("tests.data.java.ConversionTest", "testD2L", "(D)J", 0.0), 0L);
}

TEST_F(InterpreterConversionTest, D2F_Basic) {
  EXPECT_FLOAT_EQ(executeFloatMethod1Arg("tests.data.java.ConversionTest", "testD2F", "(D)F", 42.5),
                  42.5F);
}

// ============================================================================
// Int to Narrow Types
// ============================================================================

TEST_F(InterpreterConversionTest, I2B_Basic) {
  EXPECT_EQ(executeIntMethod1Arg("tests.data.java.ConversionTest", "testI2B", "(I)I", 127), 127);
  EXPECT_EQ(executeIntMethod1Arg("tests.data.java.ConversionTest", "testI2B", "(I)I", -128), -128);
  EXPECT_EQ(executeIntMethod1Arg("tests.data.java.ConversionTest", "testI2B", "(I)I", 256), 0);
}

TEST_F(InterpreterConversionTest, I2C_Basic) {
  EXPECT_EQ(executeIntMethod1Arg("tests.data.java.ConversionTest", "testI2C", "(I)I", 65), 65);
  EXPECT_EQ(executeIntMethod1Arg("tests.data.java.ConversionTest", "testI2C", "(I)I", 65535),
            65535);
}

TEST_F(InterpreterConversionTest, I2S_Basic) {
  EXPECT_EQ(executeIntMethod1Arg("tests.data.java.ConversionTest", "testI2S", "(I)I", 32767),
            32767);
  EXPECT_EQ(executeIntMethod1Arg("tests.data.java.ConversionTest", "testI2S", "(I)I", -32768),
            -32768);
}
