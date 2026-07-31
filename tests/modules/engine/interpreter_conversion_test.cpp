#include "interpreter_test_base.hpp"

#include <gtest/gtest.h>
#include <limits>

// ============================================================================
// Type Conversion Tests
// ============================================================================

class InterpreterConversionTest : public InterpreterTestBase {
 public:
  static constexpr const char* kClassName = "tests.data.java.ConversionTest";
};

// ============================================================================
// Int to Other Types
// ============================================================================

TEST_F(InterpreterConversionTest, I2L_Basic) {
  EXPECT_EQ(executeStaticMethod<Jlong>(kClassName, "testI2L", 42), 42LL);
  EXPECT_EQ(executeStaticMethod<Jlong>(kClassName, "testI2L", -42), -42LL);
}

TEST_F(InterpreterConversionTest, I2F_Basic) {
  EXPECT_FLOAT_EQ(executeStaticMethod<Jfloat>(kClassName, "testI2F", 42), 42.0F);
  EXPECT_FLOAT_EQ(executeStaticMethod<Jfloat>(kClassName, "testI2F", -42), -42.0F);
}

TEST_F(InterpreterConversionTest, I2D_Basic) {
  EXPECT_DOUBLE_EQ(executeStaticMethod<Jdouble>(kClassName, "testI2D", 42), 42.0);
  EXPECT_DOUBLE_EQ(executeStaticMethod<Jdouble>(kClassName, "testI2D", -42), -42.0);
}

// ============================================================================
// Long to Other Types
// ============================================================================

TEST_F(InterpreterConversionTest, L2I_Basic) {
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testL2I", 42LL), 42);
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testL2I", -42LL), -42);
}

TEST_F(InterpreterConversionTest, L2F_Basic) {
  EXPECT_FLOAT_EQ(executeStaticMethod<Jfloat>(kClassName, "testL2F", 42LL), 42.0F);
}

TEST_F(InterpreterConversionTest, L2D_Basic) {
  EXPECT_DOUBLE_EQ(executeStaticMethod<Jdouble>(kClassName, "testL2D", 42LL), 42.0);
}

// ============================================================================
// Float to Other Types
// ============================================================================

TEST_F(InterpreterConversionTest, F2I_Basic) {
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testF2I", 42.5F), 42);
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testF2I", -42.5F), -42);
}

TEST_F(InterpreterConversionTest, F2I_NaN) {
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testF2I_NaN"), 0);
}

TEST_F(InterpreterConversionTest, F2I_InfinitySaturates) {
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testF2I", std::numeric_limits<Jfloat>::infinity()),
            std::numeric_limits<Jint>::max());
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testF2I", -std::numeric_limits<Jfloat>::infinity()),
            std::numeric_limits<Jint>::min());
}

TEST_F(InterpreterConversionTest, F2I_PositiveOutOfRangeFiniteValueSaturates) {
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testF2I", std::numeric_limits<Jfloat>::max()),
            std::numeric_limits<Jint>::max());
}

TEST_F(InterpreterConversionTest, F2I_NegativeOutOfRangeFiniteValueSaturates) {
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testF2I", std::numeric_limits<Jfloat>::lowest()),
            std::numeric_limits<Jint>::min());
}

TEST_F(InterpreterConversionTest, F2L_Basic) {
  EXPECT_EQ(executeStaticMethod<Jlong>(kClassName, "testF2L", 42.5F), 42LL);
}

TEST_F(InterpreterConversionTest, F2L_NaN) {
  EXPECT_EQ(executeStaticMethod<Jlong>(kClassName, "testF2L", std::numeric_limits<Jfloat>::quiet_NaN()), 0LL);
}

TEST_F(InterpreterConversionTest, F2L_InfinitySaturates) {
  EXPECT_EQ(executeStaticMethod<Jlong>(kClassName, "testF2L", std::numeric_limits<Jfloat>::infinity()),
            std::numeric_limits<Jlong>::max());
  EXPECT_EQ(executeStaticMethod<Jlong>(kClassName, "testF2L", -std::numeric_limits<Jfloat>::infinity()),
            std::numeric_limits<Jlong>::min());
}

TEST_F(InterpreterConversionTest, F2L_PositiveOutOfRangeFiniteValueSaturates) {
  EXPECT_EQ(executeStaticMethod<Jlong>(kClassName, "testF2L", std::numeric_limits<Jfloat>::max()),
            std::numeric_limits<Jlong>::max());
}

TEST_F(InterpreterConversionTest, F2L_NegativeOutOfRangeFiniteValueSaturates) {
  EXPECT_EQ(executeStaticMethod<Jlong>(kClassName, "testF2L", std::numeric_limits<Jfloat>::lowest()),
            std::numeric_limits<Jlong>::min());
}

TEST_F(InterpreterConversionTest, F2D_Basic) {
  EXPECT_DOUBLE_EQ(executeStaticMethod<Jdouble>(kClassName, "testF2D", 42.5F), 42.5);
}

// ============================================================================
// Double to Other Types
// ============================================================================

TEST_F(InterpreterConversionTest, D2I_Basic) {
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testD2I", 42.5), 42);
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testD2I", -42.5), -42);
}

TEST_F(InterpreterConversionTest, D2I_NaN) {
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testD2I_NaN"), 0);
}

TEST_F(InterpreterConversionTest, D2I_InfinitySaturates) {
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testD2I", std::numeric_limits<Jdouble>::infinity()),
            std::numeric_limits<Jint>::max());
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testD2I", -std::numeric_limits<Jdouble>::infinity()),
            std::numeric_limits<Jint>::min());
}

TEST_F(InterpreterConversionTest, D2I_PositiveOutOfRangeFiniteValueSaturates) {
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testD2I", std::numeric_limits<Jdouble>::max()),
            std::numeric_limits<Jint>::max());
}

TEST_F(InterpreterConversionTest, D2I_NegativeOutOfRangeFiniteValueSaturates) {
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testD2I", std::numeric_limits<Jdouble>::lowest()),
            std::numeric_limits<Jint>::min());
}

TEST_F(InterpreterConversionTest, D2L_Basic) {
  EXPECT_EQ(executeStaticMethod<Jlong>(kClassName, "testD2L", 42.5), 42LL);
}

TEST_F(InterpreterConversionTest, D2L_NaN) {
  EXPECT_EQ(executeStaticMethod<Jlong>(kClassName, "testD2L", std::numeric_limits<Jdouble>::quiet_NaN()), 0LL);
}

TEST_F(InterpreterConversionTest, D2L_InfinitySaturates) {
  EXPECT_EQ(executeStaticMethod<Jlong>(kClassName, "testD2L", std::numeric_limits<Jdouble>::infinity()),
            std::numeric_limits<Jlong>::max());
  EXPECT_EQ(executeStaticMethod<Jlong>(kClassName, "testD2L", -std::numeric_limits<Jdouble>::infinity()),
            std::numeric_limits<Jlong>::min());
}

TEST_F(InterpreterConversionTest, D2L_PositiveOutOfRangeFiniteValueSaturates) {
  EXPECT_EQ(executeStaticMethod<Jlong>(kClassName, "testD2L", std::numeric_limits<Jdouble>::max()),
            std::numeric_limits<Jlong>::max());
}

TEST_F(InterpreterConversionTest, D2L_NegativeOutOfRangeFiniteValueSaturates) {
  EXPECT_EQ(executeStaticMethod<Jlong>(kClassName, "testD2L", std::numeric_limits<Jdouble>::lowest()),
            std::numeric_limits<Jlong>::min());
}

TEST_F(InterpreterConversionTest, D2F_Basic) {
  EXPECT_FLOAT_EQ(executeStaticMethod<Jfloat>(kClassName, "testD2F", 42.5), 42.5F);
}

// ============================================================================
// Int to Narrow Types
// ============================================================================

TEST_F(InterpreterConversionTest, I2B_Basic) {
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testI2B", 127), 127);
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testI2B", -128), -128);
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testI2B", 256), 0);
}

TEST_F(InterpreterConversionTest, I2C_Basic) {
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testI2C", 65), 65);
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testI2C", 65535), 65535);
}

TEST_F(InterpreterConversionTest, I2S_Basic) {
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testI2S", 32767), 32767);
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testI2S", -32768), -32768);
}
