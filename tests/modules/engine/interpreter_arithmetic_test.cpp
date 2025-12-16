#include <gtest/gtest.h>

#include "common/types.h"
#include "interpreter_test_base.h"

using namespace jvm;

namespace {

class InterpreterArithmeticTest : public InterpreterTestBase {
 protected:
  void SetUp() override { InterpreterTestBase::SetUp(); }

  void TearDown() override { InterpreterTestBase::TearDown(); }
};

// ============================================================================
// Integer Arithmetic Tests
// ============================================================================

TEST_F(InterpreterArithmeticTest, IADD_Basic) {
  EXPECT_EQ(executeStaticMethod<Jint>("tests.data.java.ArithmeticTest", "testIADD", 10, 20), 30);
  EXPECT_EQ(executeStaticMethod<Jint>("tests.data.java.ArithmeticTest", "testIADD", -5, 10), 5);
  EXPECT_EQ(executeStaticMethod<Jint>("tests.data.java.ArithmeticTest", "testIADD", 0, 0), 0);
}

TEST_F(InterpreterArithmeticTest, ISUB_Basic) {
  EXPECT_EQ(executeStaticMethod<Jint>("tests.data.java.ArithmeticTest", "testISUB", 20, 10), 10);
  EXPECT_EQ(executeStaticMethod<Jint>("tests.data.java.ArithmeticTest", "testISUB", 10, 20), -10);
  EXPECT_EQ(executeStaticMethod<Jint>("tests.data.java.ArithmeticTest", "testISUB", 0, 0), 0);
}

TEST_F(InterpreterArithmeticTest, IMUL_Basic) {
  EXPECT_EQ(executeStaticMethod<Jint>("tests.data.java.ArithmeticTest", "testIMUL", 5, 6), 30);
  EXPECT_EQ(executeStaticMethod<Jint>("tests.data.java.ArithmeticTest", "testIMUL", -5, 6), -30);
  EXPECT_EQ(executeStaticMethod<Jint>("tests.data.java.ArithmeticTest", "testIMUL", 0, 100), 0);
}

TEST_F(InterpreterArithmeticTest, IDIV_Basic) {
  EXPECT_EQ(executeStaticMethod<Jint>("tests.data.java.ArithmeticTest", "testIDIV", 20, 5), 4);
  EXPECT_EQ(executeStaticMethod<Jint>("tests.data.java.ArithmeticTest", "testIDIV", 21, 5),
            4);  // Integer division truncates
  EXPECT_EQ(executeStaticMethod<Jint>("tests.data.java.ArithmeticTest", "testIDIV", -20, 5), -4);
}

TEST_F(InterpreterArithmeticTest, IDIV_DivideByZero) {
  EXPECT_THROW(executeStaticMethod<Jint>("tests.data.java.ArithmeticTest", "testIDIV", 10, 0),
               std::runtime_error);
}

TEST_F(InterpreterArithmeticTest, IREM_Basic) {
  EXPECT_EQ(executeStaticMethod<Jint>("tests.data.java.ArithmeticTest", "testIREM", 20, 5), 0);
  EXPECT_EQ(executeStaticMethod<Jint>("tests.data.java.ArithmeticTest", "testIREM", 21, 5), 1);
  EXPECT_EQ(executeStaticMethod<Jint>("tests.data.java.ArithmeticTest", "testIREM", -21, 5), -1);
}

TEST_F(InterpreterArithmeticTest, IREM_DivideByZero) {
  EXPECT_THROW(executeStaticMethod<Jint>("tests.data.java.ArithmeticTest", "testIREM", 10, 0),
               std::runtime_error);
}

TEST_F(InterpreterArithmeticTest, INEG_Basic) {
  EXPECT_EQ(executeStaticMethod<Jint>("tests.data.java.ArithmeticTest", "testINEG", 10), -10);
  EXPECT_EQ(executeStaticMethod<Jint>("tests.data.java.ArithmeticTest", "testINEG", -10), 10);
  EXPECT_EQ(executeStaticMethod<Jint>("tests.data.java.ArithmeticTest", "testINEG", 0), 0);
}

// ============================================================================
// Long Arithmetic Tests
// ============================================================================

TEST_F(InterpreterArithmeticTest, LADD_Basic) {
  EXPECT_EQ(executeStaticMethod<Jlong>("tests.data.java.ArithmeticTest", "testLADD", 10LL, 20LL),
            30LL);
  EXPECT_EQ(executeStaticMethod<Jlong>("tests.data.java.ArithmeticTest", "testLADD", -5LL, 10LL),
            5LL);
}

TEST_F(InterpreterArithmeticTest, LSUB_Basic) {
  EXPECT_EQ(executeStaticMethod<Jlong>("tests.data.java.ArithmeticTest", "testLSUB", 20LL, 10LL),
            10LL);
  EXPECT_EQ(executeStaticMethod<Jlong>("tests.data.java.ArithmeticTest", "testLSUB", 10LL, 20LL),
            -10LL);
}

TEST_F(InterpreterArithmeticTest, LMUL_Basic) {
  EXPECT_EQ(executeStaticMethod<Jlong>("tests.data.java.ArithmeticTest", "testLMUL", 5LL, 6LL),
            30LL);
  EXPECT_EQ(executeStaticMethod<Jlong>("tests.data.java.ArithmeticTest", "testLMUL", -5LL, 6LL),
            -30LL);
}

TEST_F(InterpreterArithmeticTest, LDIV_Basic) {
  EXPECT_EQ(executeStaticMethod<Jlong>("tests.data.java.ArithmeticTest", "testLDIV", 20LL, 5LL),
            4LL);
  EXPECT_EQ(executeStaticMethod<Jlong>("tests.data.java.ArithmeticTest", "testLDIV", 21LL, 5LL),
            4LL);
}

TEST_F(InterpreterArithmeticTest, LDIV_DivideByZero) {
  EXPECT_THROW(executeStaticMethod<Jlong>("tests.data.java.ArithmeticTest", "testLDIV", 10LL, 0LL),
               std::runtime_error);
}

TEST_F(InterpreterArithmeticTest, LREM_Basic) {
  EXPECT_EQ(executeStaticMethod<Jlong>("tests.data.java.ArithmeticTest", "testLREM", 20LL, 5LL),
            0LL);
  EXPECT_EQ(executeStaticMethod<Jlong>("tests.data.java.ArithmeticTest", "testLREM", 21LL, 5LL),
            1LL);
}

TEST_F(InterpreterArithmeticTest, LREM_DivideByZero) {
  EXPECT_THROW(executeStaticMethod<Jlong>("tests.data.java.ArithmeticTest", "testLREM", 10LL, 0LL),
               std::runtime_error);
}

TEST_F(InterpreterArithmeticTest, LNEG_Basic) {
  EXPECT_EQ(executeStaticMethod<Jlong>("tests.data.java.ArithmeticTest", "testLNEG", 10LL), -10LL);
  EXPECT_EQ(executeStaticMethod<Jlong>("tests.data.java.ArithmeticTest", "testLNEG", -10LL), 10LL);
}

// ============================================================================
// Float Arithmetic Tests
// ============================================================================

TEST_F(InterpreterArithmeticTest, FADD_Basic) {
  EXPECT_FLOAT_EQ(
    executeStaticMethod<Jfloat>("tests.data.java.ArithmeticTest", "testFADD", 10.5F, 20.5F), 31.0F);
  EXPECT_FLOAT_EQ(
    executeStaticMethod<Jfloat>("tests.data.java.ArithmeticTest", "testFADD", -5.5F, 10.5F), 5.0F);
}

TEST_F(InterpreterArithmeticTest, FSUB_Basic) {
  EXPECT_FLOAT_EQ(
    executeStaticMethod<Jfloat>("tests.data.java.ArithmeticTest", "testFSUB", 20.5F, 10.5F), 10.0F);
  EXPECT_FLOAT_EQ(
    executeStaticMethod<Jfloat>("tests.data.java.ArithmeticTest", "testFSUB", 10.5F, 20.5F),
    -10.0F);
}

TEST_F(InterpreterArithmeticTest, FMUL_Basic) {
  EXPECT_FLOAT_EQ(
    executeStaticMethod<Jfloat>("tests.data.java.ArithmeticTest", "testFMUL", 5.0F, 6.0F), 30.0F);
  EXPECT_FLOAT_EQ(
    executeStaticMethod<Jfloat>("tests.data.java.ArithmeticTest", "testFMUL", -5.0F, 6.0F), -30.0F);
}

TEST_F(InterpreterArithmeticTest, FDIV_Basic) {
  EXPECT_FLOAT_EQ(
    executeStaticMethod<Jfloat>("tests.data.java.ArithmeticTest", "testFDIV", 20.0F, 5.0F), 4.0F);
  EXPECT_FLOAT_EQ(
    executeStaticMethod<Jfloat>("tests.data.java.ArithmeticTest", "testFDIV", 21.0F, 5.0F), 4.2F);
}

TEST_F(InterpreterArithmeticTest, FREM_Basic) {
  EXPECT_FLOAT_EQ(
    executeStaticMethod<Jfloat>("tests.data.java.ArithmeticTest", "testFREM", 20.5F, 5.0F), 0.5F);
  EXPECT_FLOAT_EQ(
    executeStaticMethod<Jfloat>("tests.data.java.ArithmeticTest", "testFREM", 21.0F, 5.0F), 1.0F);
}

TEST_F(InterpreterArithmeticTest, FNEG_Basic) {
  EXPECT_FLOAT_EQ(executeStaticMethod<Jfloat>("tests.data.java.ArithmeticTest", "testFNEG", 10.5F),
                  -10.5F);
  EXPECT_FLOAT_EQ(executeStaticMethod<Jfloat>("tests.data.java.ArithmeticTest", "testFNEG", -10.5F),
                  10.5F);
}

// ============================================================================
// Double Arithmetic Tests
// ============================================================================

TEST_F(InterpreterArithmeticTest, DADD_Basic) {
  EXPECT_DOUBLE_EQ(
    executeStaticMethod<Jdouble>("tests.data.java.ArithmeticTest", "testDADD", 10.5, 20.5), 31.0);
  EXPECT_DOUBLE_EQ(
    executeStaticMethod<Jdouble>("tests.data.java.ArithmeticTest", "testDADD", -5.5, 10.5), 5.0);
}

TEST_F(InterpreterArithmeticTest, DSUB_Basic) {
  EXPECT_DOUBLE_EQ(
    executeStaticMethod<Jdouble>("tests.data.java.ArithmeticTest", "testDSUB", 20.5, 10.5), 10.0);
  EXPECT_DOUBLE_EQ(
    executeStaticMethod<Jdouble>("tests.data.java.ArithmeticTest", "testDSUB", 10.5, 20.5), -10.0);
}

TEST_F(InterpreterArithmeticTest, DMUL_Basic) {
  EXPECT_DOUBLE_EQ(
    executeStaticMethod<Jdouble>("tests.data.java.ArithmeticTest", "testDMUL", 5.0, 6.0), 30.0);
  EXPECT_DOUBLE_EQ(
    executeStaticMethod<Jdouble>("tests.data.java.ArithmeticTest", "testDMUL", -5.0, 6.0), -30.0);
}

TEST_F(InterpreterArithmeticTest, DDIV_Basic) {
  EXPECT_DOUBLE_EQ(
    executeStaticMethod<Jdouble>("tests.data.java.ArithmeticTest", "testDDIV", 20.0, 5.0), 4.0);
  EXPECT_DOUBLE_EQ(
    executeStaticMethod<Jdouble>("tests.data.java.ArithmeticTest", "testDDIV", 21.0, 5.0), 4.2);
}

TEST_F(InterpreterArithmeticTest, DREM_Basic) {
  EXPECT_DOUBLE_EQ(
    executeStaticMethod<Jdouble>("tests.data.java.ArithmeticTest", "testDREM", 20.5, 5.0), 0.5);
  EXPECT_DOUBLE_EQ(
    executeStaticMethod<Jdouble>("tests.data.java.ArithmeticTest", "testDREM", 21.0, 5.0), 1.0);
}

TEST_F(InterpreterArithmeticTest, DNEG_Basic) {
  EXPECT_DOUBLE_EQ(executeStaticMethod<Jdouble>("tests.data.java.ArithmeticTest", "testDNEG", 10.5),
                   -10.5);
  EXPECT_DOUBLE_EQ(
    executeStaticMethod<Jdouble>("tests.data.java.ArithmeticTest", "testDNEG", -10.5), 10.5);
}

// ============================================================================
// Integer Bitwise Operation Tests
// ============================================================================

TEST_F(InterpreterArithmeticTest, IAND_Basic) {
  EXPECT_EQ(executeStaticMethod<Jint>("tests.data.java.ArithmeticTest", "testIAND", 0x0F, 0xF0),
            0x00);
  EXPECT_EQ(executeStaticMethod<Jint>("tests.data.java.ArithmeticTest", "testIAND", 0xFF, 0x0F),
            0x0F);
  EXPECT_EQ(executeStaticMethod<Jint>("tests.data.java.ArithmeticTest", "testIAND", 0xAA, 0x55),
            0x00);
  EXPECT_EQ(executeStaticMethod<Jint>("tests.data.java.ArithmeticTest", "testIAND", 0xFF, 0xFF),
            0xFF);
  EXPECT_EQ(executeStaticMethod<Jint>("tests.data.java.ArithmeticTest", "testIAND", 0, 0xFFFF), 0);
}

TEST_F(InterpreterArithmeticTest, IOR_Basic) {
  EXPECT_EQ(executeStaticMethod<Jint>("tests.data.java.ArithmeticTest", "testIOR", 0x0F, 0xF0),
            0xFF);
  EXPECT_EQ(executeStaticMethod<Jint>("tests.data.java.ArithmeticTest", "testIOR", 0xAA, 0x55),
            0xFF);
  EXPECT_EQ(executeStaticMethod<Jint>("tests.data.java.ArithmeticTest", "testIOR", 0, 0xFF), 0xFF);
  EXPECT_EQ(executeStaticMethod<Jint>("tests.data.java.ArithmeticTest", "testIOR", 0xFF, 0x00),
            0xFF);
}

TEST_F(InterpreterArithmeticTest, IXOR_Basic) {
  EXPECT_EQ(executeStaticMethod<Jint>("tests.data.java.ArithmeticTest", "testIXOR", 0xFF, 0xFF),
            0x00);
  EXPECT_EQ(executeStaticMethod<Jint>("tests.data.java.ArithmeticTest", "testIXOR", 0xAA, 0x55),
            0xFF);
  EXPECT_EQ(executeStaticMethod<Jint>("tests.data.java.ArithmeticTest", "testIXOR", 0x0F, 0xF0),
            0xFF);
  EXPECT_EQ(executeStaticMethod<Jint>("tests.data.java.ArithmeticTest", "testIXOR", 0, 0xFF), 0xFF);
}

TEST_F(InterpreterArithmeticTest, ISHL_Basic) {
  EXPECT_EQ(executeStaticMethod<Jint>("tests.data.java.ArithmeticTest", "testISHL", 1, 1), 2);
  EXPECT_EQ(executeStaticMethod<Jint>("tests.data.java.ArithmeticTest", "testISHL", 1, 3), 8);
  EXPECT_EQ(executeStaticMethod<Jint>("tests.data.java.ArithmeticTest", "testISHL", 5, 2), 20);
  EXPECT_EQ(
    executeStaticMethod<Jint>("tests.data.java.ArithmeticTest", "testISHL", (Jint)0x80000000, 1),
    0);  // Overflow
}

TEST_F(InterpreterArithmeticTest, ISHR_Basic) {
  EXPECT_EQ(executeStaticMethod<Jint>("tests.data.java.ArithmeticTest", "testISHR", 8, 1), 4);
  EXPECT_EQ(executeStaticMethod<Jint>("tests.data.java.ArithmeticTest", "testISHR", 32, 3), 4);
  EXPECT_EQ(executeStaticMethod<Jint>("tests.data.java.ArithmeticTest", "testISHR", -8, 1),
            -4);  // Arithmetic shift preserves sign
  EXPECT_EQ(executeStaticMethod<Jint>("tests.data.java.ArithmeticTest", "testISHR", -32, 3), -4);
}

TEST_F(InterpreterArithmeticTest, IUSHR_Basic) {
  EXPECT_EQ(executeStaticMethod<Jint>("tests.data.java.ArithmeticTest", "testIUSHR", 8, 1), 4);
  EXPECT_EQ(executeStaticMethod<Jint>("tests.data.java.ArithmeticTest", "testIUSHR", 32, 3), 4);
  EXPECT_EQ(executeStaticMethod<Jint>("tests.data.java.ArithmeticTest", "testIUSHR", -8, 1),
            0x7FFFFFFC);  // Logical shift, no sign extension
  EXPECT_EQ(executeStaticMethod<Jint>("tests.data.java.ArithmeticTest", "testIUSHR", -1, 31),
            1);  // Logical shift right 31 bits
}

// ============================================================================
// Long Bitwise Operation Tests
// ============================================================================

TEST_F(InterpreterArithmeticTest, LAND_Basic) {
  EXPECT_EQ(
    executeStaticMethod<Jlong>("tests.data.java.ArithmeticTest", "testLAND", 0x0FLL, 0xF0LL),
    0x00LL);
  EXPECT_EQ(
    executeStaticMethod<Jlong>("tests.data.java.ArithmeticTest", "testLAND", 0xFFLL, 0x0FLL),
    0x0FLL);
  EXPECT_EQ(
    executeStaticMethod<Jlong>("tests.data.java.ArithmeticTest", "testLAND", 0xAALL, 0x55LL),
    0x00LL);
  EXPECT_EQ(executeStaticMethod<Jlong>("tests.data.java.ArithmeticTest", "testLAND", 0xFFFFFFFFLL,
                                       0xFFFFFFFFLL),
            0xFFFFFFFFLL);
}

TEST_F(InterpreterArithmeticTest, LOR_Basic) {
  EXPECT_EQ(executeStaticMethod<Jlong>("tests.data.java.ArithmeticTest", "testLOR", 0x0FLL, 0xF0LL),
            0xFFLL);
  EXPECT_EQ(executeStaticMethod<Jlong>("tests.data.java.ArithmeticTest", "testLOR", 0xAALL, 0x55LL),
            0xFFLL);
  EXPECT_EQ(executeStaticMethod<Jlong>("tests.data.java.ArithmeticTest", "testLOR", 0LL, 0xFFLL),
            0xFFLL);
  EXPECT_EQ(executeStaticMethod<Jlong>("tests.data.java.ArithmeticTest", "testLOR", 0xFFLL, 0x00LL),
            0xFFLL);
}

TEST_F(InterpreterArithmeticTest, LXOR_Basic) {
  EXPECT_EQ(
    executeStaticMethod<Jlong>("tests.data.java.ArithmeticTest", "testLXOR", 0xFFLL, 0xFFLL),
    0x00LL);
  EXPECT_EQ(
    executeStaticMethod<Jlong>("tests.data.java.ArithmeticTest", "testLXOR", 0xAALL, 0x55LL),
    0xFFLL);
  EXPECT_EQ(
    executeStaticMethod<Jlong>("tests.data.java.ArithmeticTest", "testLXOR", 0x0FLL, 0xF0LL),
    0xFFLL);
  EXPECT_EQ(executeStaticMethod<Jlong>("tests.data.java.ArithmeticTest", "testLXOR", 0LL, 0xFFLL),
            0xFFLL);
}

TEST_F(InterpreterArithmeticTest, LSHL_Basic) {
  EXPECT_EQ(executeStaticMethod<Jlong>("tests.data.java.ArithmeticTest", "testLSHL", 1LL, 1), 2LL);
  EXPECT_EQ(executeStaticMethod<Jlong>("tests.data.java.ArithmeticTest", "testLSHL", 1LL, 3), 8LL);
  EXPECT_EQ(executeStaticMethod<Jlong>("tests.data.java.ArithmeticTest", "testLSHL", 5LL, 2), 20LL);
  EXPECT_EQ(executeStaticMethod<Jlong>("tests.data.java.ArithmeticTest", "testLSHL",
                                       0x4000000000000000LL, 1),
            0x8000000000000000LL);
}

TEST_F(InterpreterArithmeticTest, LSHR_Basic) {
  EXPECT_EQ(executeStaticMethod<Jlong>("tests.data.java.ArithmeticTest", "testLSHR", 8LL, 1), 4LL);
  EXPECT_EQ(executeStaticMethod<Jlong>("tests.data.java.ArithmeticTest", "testLSHR", 32LL, 3), 4LL);
  EXPECT_EQ(executeStaticMethod<Jlong>("tests.data.java.ArithmeticTest", "testLSHR", -8LL, 1),
            -4LL);  // Arithmetic shift preserves sign
  EXPECT_EQ(executeStaticMethod<Jlong>("tests.data.java.ArithmeticTest", "testLSHR", -32LL, 3),
            -4LL);
}

TEST_F(InterpreterArithmeticTest, LUSHR_Basic) {
  EXPECT_EQ(executeStaticMethod<Jlong>("tests.data.java.ArithmeticTest", "testLUSHR", 8LL, 1), 4LL);
  EXPECT_EQ(executeStaticMethod<Jlong>("tests.data.java.ArithmeticTest", "testLUSHR", 32LL, 3),
            4LL);
  EXPECT_EQ(executeStaticMethod<Jlong>("tests.data.java.ArithmeticTest", "testLUSHR", -8LL, 1),
            0x7FFFFFFFFFFFFFFCLL);  // Logical shift
  EXPECT_EQ(executeStaticMethod<Jlong>("tests.data.java.ArithmeticTest", "testLUSHR", -1LL, 63),
            1LL);  // Logical shift right 63 bits
}

}  // namespace
