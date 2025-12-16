#include <gtest/gtest.h>

#include "common/types.h"
#include "interpreter_test_base.h"

using namespace jvm;

namespace {

class InterpreterArithmeticTest : public InterpreterTestBase {
 public:
  static constexpr const char* kClassName = "tests.data.java.ArithmeticTest";
};

// ============================================================================
// Integer Arithmetic Tests
// ============================================================================

TEST_F(InterpreterArithmeticTest, IADD_Basic) {
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testIADD", 10, 20), 30);
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testIADD", -5, 10), 5);
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testIADD", 0, 0), 0);
}

TEST_F(InterpreterArithmeticTest, ISUB_Basic) {
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testISUB", 20, 10), 10);
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testISUB", 10, 20), -10);
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testISUB", 0, 0), 0);
}

TEST_F(InterpreterArithmeticTest, IMUL_Basic) {
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testIMUL", 5, 6), 30);
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testIMUL", -5, 6), -30);
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testIMUL", 0, 100), 0);
}

TEST_F(InterpreterArithmeticTest, IDIV_Basic) {
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testIDIV", 20, 5), 4);
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testIDIV", 21, 5),
            4);  // Integer division truncates
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testIDIV", -20, 5), -4);
}

TEST_F(InterpreterArithmeticTest, IDIV_DivideByZero) {
  EXPECT_THROW(executeStaticMethod<Jint>(kClassName, "testIDIV", 10, 0), std::runtime_error);
}

TEST_F(InterpreterArithmeticTest, IREM_Basic) {
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testIREM", 20, 5), 0);
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testIREM", 21, 5), 1);
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testIREM", -21, 5), -1);
}

TEST_F(InterpreterArithmeticTest, IREM_DivideByZero) {
  EXPECT_THROW(executeStaticMethod<Jint>(kClassName, "testIREM", 10, 0), std::runtime_error);
}

TEST_F(InterpreterArithmeticTest, INEG_Basic) {
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testINEG", 10), -10);
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testINEG", -10), 10);
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testINEG", 0), 0);
}

// ============================================================================
// Long Arithmetic Tests
// ============================================================================

TEST_F(InterpreterArithmeticTest, LADD_Basic) {
  EXPECT_EQ(executeStaticMethod<Jlong>(kClassName, "testLADD", 10LL, 20LL), 30LL);
  EXPECT_EQ(executeStaticMethod<Jlong>(kClassName, "testLADD", -5LL, 10LL), 5LL);
}

TEST_F(InterpreterArithmeticTest, LSUB_Basic) {
  EXPECT_EQ(executeStaticMethod<Jlong>(kClassName, "testLSUB", 20LL, 10LL), 10LL);
  EXPECT_EQ(executeStaticMethod<Jlong>(kClassName, "testLSUB", 10LL, 20LL), -10LL);
}

TEST_F(InterpreterArithmeticTest, LMUL_Basic) {
  EXPECT_EQ(executeStaticMethod<Jlong>(kClassName, "testLMUL", 5LL, 6LL), 30LL);
  EXPECT_EQ(executeStaticMethod<Jlong>(kClassName, "testLMUL", -5LL, 6LL), -30LL);
}

TEST_F(InterpreterArithmeticTest, LDIV_Basic) {
  EXPECT_EQ(executeStaticMethod<Jlong>(kClassName, "testLDIV", 20LL, 5LL), 4LL);
  EXPECT_EQ(executeStaticMethod<Jlong>(kClassName, "testLDIV", 21LL, 5LL), 4LL);
}

TEST_F(InterpreterArithmeticTest, LDIV_DivideByZero) {
  EXPECT_THROW(executeStaticMethod<Jlong>(kClassName, "testLDIV", 10LL, 0LL), std::runtime_error);
}

TEST_F(InterpreterArithmeticTest, LREM_Basic) {
  EXPECT_EQ(executeStaticMethod<Jlong>(kClassName, "testLREM", 20LL, 5LL), 0LL);
  EXPECT_EQ(executeStaticMethod<Jlong>(kClassName, "testLREM", 21LL, 5LL), 1LL);
}

TEST_F(InterpreterArithmeticTest, LREM_DivideByZero) {
  EXPECT_THROW(executeStaticMethod<Jlong>(kClassName, "testLREM", 10LL, 0LL), std::runtime_error);
}

TEST_F(InterpreterArithmeticTest, LNEG_Basic) {
  EXPECT_EQ(executeStaticMethod<Jlong>(kClassName, "testLNEG", 10LL), -10LL);
  EXPECT_EQ(executeStaticMethod<Jlong>(kClassName, "testLNEG", -10LL), 10LL);
}

// ============================================================================
// Float Arithmetic Tests
// ============================================================================

TEST_F(InterpreterArithmeticTest, FADD_Basic) {
  EXPECT_FLOAT_EQ(executeStaticMethod<Jfloat>(kClassName, "testFADD", 10.5F, 20.5F), 31.0F);
  EXPECT_FLOAT_EQ(executeStaticMethod<Jfloat>(kClassName, "testFADD", -5.5F, 10.5F), 5.0F);
}

TEST_F(InterpreterArithmeticTest, FSUB_Basic) {
  EXPECT_FLOAT_EQ(executeStaticMethod<Jfloat>(kClassName, "testFSUB", 20.5F, 10.5F), 10.0F);
  EXPECT_FLOAT_EQ(executeStaticMethod<Jfloat>(kClassName, "testFSUB", 10.5F, 20.5F), -10.0F);
}

TEST_F(InterpreterArithmeticTest, FMUL_Basic) {
  EXPECT_FLOAT_EQ(executeStaticMethod<Jfloat>(kClassName, "testFMUL", 5.0F, 6.0F), 30.0F);
  EXPECT_FLOAT_EQ(executeStaticMethod<Jfloat>(kClassName, "testFMUL", -5.0F, 6.0F), -30.0F);
}

TEST_F(InterpreterArithmeticTest, FDIV_Basic) {
  EXPECT_FLOAT_EQ(executeStaticMethod<Jfloat>(kClassName, "testFDIV", 20.0F, 5.0F), 4.0F);
  EXPECT_FLOAT_EQ(executeStaticMethod<Jfloat>(kClassName, "testFDIV", 21.0F, 5.0F), 4.2F);
}

TEST_F(InterpreterArithmeticTest, FREM_Basic) {
  EXPECT_FLOAT_EQ(executeStaticMethod<Jfloat>(kClassName, "testFREM", 20.5F, 5.0F), 0.5F);
  EXPECT_FLOAT_EQ(executeStaticMethod<Jfloat>(kClassName, "testFREM", 21.0F, 5.0F), 1.0F);
}

TEST_F(InterpreterArithmeticTest, FNEG_Basic) {
  EXPECT_FLOAT_EQ(executeStaticMethod<Jfloat>(kClassName, "testFNEG", 10.5F), -10.5F);
  EXPECT_FLOAT_EQ(executeStaticMethod<Jfloat>(kClassName, "testFNEG", -10.5F), 10.5F);
}

// ============================================================================
// Double Arithmetic Tests
// ============================================================================

TEST_F(InterpreterArithmeticTest, DADD_Basic) {
  EXPECT_DOUBLE_EQ(executeStaticMethod<Jdouble>(kClassName, "testDADD", 10.5, 20.5), 31.0);
  EXPECT_DOUBLE_EQ(executeStaticMethod<Jdouble>(kClassName, "testDADD", -5.5, 10.5), 5.0);
}

TEST_F(InterpreterArithmeticTest, DSUB_Basic) {
  EXPECT_DOUBLE_EQ(executeStaticMethod<Jdouble>(kClassName, "testDSUB", 20.5, 10.5), 10.0);
  EXPECT_DOUBLE_EQ(executeStaticMethod<Jdouble>(kClassName, "testDSUB", 10.5, 20.5), -10.0);
}

TEST_F(InterpreterArithmeticTest, DMUL_Basic) {
  EXPECT_DOUBLE_EQ(executeStaticMethod<Jdouble>(kClassName, "testDMUL", 5.0, 6.0), 30.0);
  EXPECT_DOUBLE_EQ(executeStaticMethod<Jdouble>(kClassName, "testDMUL", -5.0, 6.0), -30.0);
}

TEST_F(InterpreterArithmeticTest, DDIV_Basic) {
  EXPECT_DOUBLE_EQ(executeStaticMethod<Jdouble>(kClassName, "testDDIV", 20.0, 5.0), 4.0);
  EXPECT_DOUBLE_EQ(executeStaticMethod<Jdouble>(kClassName, "testDDIV", 21.0, 5.0), 4.2);
}

TEST_F(InterpreterArithmeticTest, DREM_Basic) {
  EXPECT_DOUBLE_EQ(executeStaticMethod<Jdouble>(kClassName, "testDREM", 20.5, 5.0), 0.5);
  EXPECT_DOUBLE_EQ(executeStaticMethod<Jdouble>(kClassName, "testDREM", 21.0, 5.0), 1.0);
}

TEST_F(InterpreterArithmeticTest, DNEG_Basic) {
  EXPECT_DOUBLE_EQ(executeStaticMethod<Jdouble>(kClassName, "testDNEG", 10.5), -10.5);
  EXPECT_DOUBLE_EQ(executeStaticMethod<Jdouble>(kClassName, "testDNEG", -10.5), 10.5);
}

// ============================================================================
// Integer Bitwise Operation Tests
// ============================================================================

TEST_F(InterpreterArithmeticTest, IAND_Basic) {
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testIAND", 0x0F, 0xF0), 0x00);
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testIAND", 0xFF, 0x0F), 0x0F);
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testIAND", 0xAA, 0x55), 0x00);
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testIAND", 0xFF, 0xFF), 0xFF);
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testIAND", 0, 0xFFFF), 0);
}

TEST_F(InterpreterArithmeticTest, IOR_Basic) {
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testIOR", 0x0F, 0xF0), 0xFF);
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testIOR", 0xAA, 0x55), 0xFF);
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testIOR", 0, 0xFF), 0xFF);
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testIOR", 0xFF, 0x00), 0xFF);
}

TEST_F(InterpreterArithmeticTest, IXOR_Basic) {
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testIXOR", 0xFF, 0xFF), 0x00);
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testIXOR", 0xAA, 0x55), 0xFF);
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testIXOR", 0x0F, 0xF0), 0xFF);
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testIXOR", 0, 0xFF), 0xFF);
}

TEST_F(InterpreterArithmeticTest, ISHL_Basic) {
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testISHL", 1, 1), 2);
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testISHL", 1, 3), 8);
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testISHL", 5, 2), 20);
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testISHL", (Jint)0x80000000, 1),
            0);  // Overflow
}

TEST_F(InterpreterArithmeticTest, ISHR_Basic) {
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testISHR", 8, 1), 4);
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testISHR", 32, 3), 4);
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testISHR", -8, 1),
            -4);  // Arithmetic shift preserves sign
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testISHR", -32, 3), -4);
}

TEST_F(InterpreterArithmeticTest, IUSHR_Basic) {
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testIUSHR", 8, 1), 4);
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testIUSHR", 32, 3), 4);
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testIUSHR", -8, 1),
            0x7FFFFFFC);  // Logical shift, no sign extension
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testIUSHR", -1, 31),
            1);  // Logical shift right 31 bits
}

// ============================================================================
// Long Bitwise Operation Tests
// ============================================================================

TEST_F(InterpreterArithmeticTest, LAND_Basic) {
  EXPECT_EQ(executeStaticMethod<Jlong>(kClassName, "testLAND", 0x0FLL, 0xF0LL), 0x00LL);
  EXPECT_EQ(executeStaticMethod<Jlong>(kClassName, "testLAND", 0xFFLL, 0x0FLL), 0x0FLL);
  EXPECT_EQ(executeStaticMethod<Jlong>(kClassName, "testLAND", 0xAALL, 0x55LL), 0x00LL);
  EXPECT_EQ(executeStaticMethod<Jlong>(kClassName, "testLAND", 0xFFFFFFFFLL, 0xFFFFFFFFLL),
            0xFFFFFFFFLL);
}

TEST_F(InterpreterArithmeticTest, LOR_Basic) {
  EXPECT_EQ(executeStaticMethod<Jlong>(kClassName, "testLOR", 0x0FLL, 0xF0LL), 0xFFLL);
  EXPECT_EQ(executeStaticMethod<Jlong>(kClassName, "testLOR", 0xAALL, 0x55LL), 0xFFLL);
  EXPECT_EQ(executeStaticMethod<Jlong>(kClassName, "testLOR", 0LL, 0xFFLL), 0xFFLL);
  EXPECT_EQ(executeStaticMethod<Jlong>(kClassName, "testLOR", 0xFFLL, 0x00LL), 0xFFLL);
}

TEST_F(InterpreterArithmeticTest, LXOR_Basic) {
  EXPECT_EQ(executeStaticMethod<Jlong>(kClassName, "testLXOR", 0xFFLL, 0xFFLL), 0x00LL);
  EXPECT_EQ(executeStaticMethod<Jlong>(kClassName, "testLXOR", 0xAALL, 0x55LL), 0xFFLL);
  EXPECT_EQ(executeStaticMethod<Jlong>(kClassName, "testLXOR", 0x0FLL, 0xF0LL), 0xFFLL);
  EXPECT_EQ(executeStaticMethod<Jlong>(kClassName, "testLXOR", 0LL, 0xFFLL), 0xFFLL);
}

TEST_F(InterpreterArithmeticTest, LSHL_Basic) {
  EXPECT_EQ(executeStaticMethod<Jlong>(kClassName, "testLSHL", 1LL, 1), 2LL);
  EXPECT_EQ(executeStaticMethod<Jlong>(kClassName, "testLSHL", 1LL, 3), 8LL);
  EXPECT_EQ(executeStaticMethod<Jlong>(kClassName, "testLSHL", 5LL, 2), 20LL);
  EXPECT_EQ(executeStaticMethod<Jlong>(kClassName, "testLSHL", 0x4000000000000000LL, 1),
            0x8000000000000000LL);
}

TEST_F(InterpreterArithmeticTest, LSHR_Basic) {
  EXPECT_EQ(executeStaticMethod<Jlong>(kClassName, "testLSHR", 8LL, 1), 4LL);
  EXPECT_EQ(executeStaticMethod<Jlong>(kClassName, "testLSHR", 32LL, 3), 4LL);
  EXPECT_EQ(executeStaticMethod<Jlong>(kClassName, "testLSHR", -8LL, 1),
            -4LL);  // Arithmetic shift preserves sign
  EXPECT_EQ(executeStaticMethod<Jlong>(kClassName, "testLSHR", -32LL, 3), -4LL);
}

TEST_F(InterpreterArithmeticTest, LUSHR_Basic) {
  EXPECT_EQ(executeStaticMethod<Jlong>(kClassName, "testLUSHR", 8LL, 1), 4LL);
  EXPECT_EQ(executeStaticMethod<Jlong>(kClassName, "testLUSHR", 32LL, 3), 4LL);
  EXPECT_EQ(executeStaticMethod<Jlong>(kClassName, "testLUSHR", -8LL, 1),
            0x7FFFFFFFFFFFFFFCLL);  // Logical shift
  EXPECT_EQ(executeStaticMethod<Jlong>(kClassName, "testLUSHR", -1LL, 63),
            1LL);  // Logical shift right 63 bits
}

}  // namespace
