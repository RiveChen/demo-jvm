#include "interpreter_test_base.hpp"
#include "utilities/types.hpp"

#include <gtest/gtest.h>
#include <limits>

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

TEST_F(InterpreterArithmeticTest, IADD_MaxPlusOneWrapsToMin) {
  EXPECT_EQ(
    executeStaticMethod<Jint>(kClassName, "testIADD", std::numeric_limits<Jint>::max(), Jint{1}),
    std::numeric_limits<Jint>::min());
}

TEST_F(InterpreterArithmeticTest, ISUB_MinMinusOneWrapsToMax) {
  EXPECT_EQ(
    executeStaticMethod<Jint>(kClassName, "testISUB", std::numeric_limits<Jint>::min(), Jint{1}),
    std::numeric_limits<Jint>::max());
}

TEST_F(InterpreterArithmeticTest, IMUL_OverflowWrapsAsTwosComplement) {
  EXPECT_EQ(
    executeStaticMethod<Jint>(kClassName, "testIMUL", std::numeric_limits<Jint>::max(), Jint{2}),
    Jint{-2});
}

TEST_F(InterpreterArithmeticTest, INEG_MinWrapsToMin) {
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testINEG", std::numeric_limits<Jint>::min()),
            std::numeric_limits<Jint>::min());
}

TEST_F(InterpreterArithmeticTest, IDIV_MinByNegativeOneReturnsMin) {
  EXPECT_EQ(
    executeStaticMethod<Jint>(kClassName, "testIDIV", std::numeric_limits<Jint>::min(), Jint{-1}),
    std::numeric_limits<Jint>::min());
}

TEST_F(InterpreterArithmeticTest, IREM_MinByNegativeOneReturnsZero) {
  EXPECT_EQ(
    executeStaticMethod<Jint>(kClassName, "testIREM", std::numeric_limits<Jint>::min(), Jint{-1}),
    Jint{0});
}

// ============================================================================
// Long Arithmetic Tests
// ============================================================================

TEST_F(InterpreterArithmeticTest, LADD_Basic) {
  EXPECT_EQ(executeStaticMethod<Jlong>(kClassName, "testLADD", Jlong{10}, Jlong{20}), Jlong{30});
  EXPECT_EQ(executeStaticMethod<Jlong>(kClassName, "testLADD", Jlong{-5}, Jlong{10}), Jlong{5});
}

TEST_F(InterpreterArithmeticTest, LSUB_Basic) {
  EXPECT_EQ(executeStaticMethod<Jlong>(kClassName, "testLSUB", Jlong{20}, Jlong{10}), Jlong{10});
  EXPECT_EQ(executeStaticMethod<Jlong>(kClassName, "testLSUB", Jlong{10}, Jlong{20}), Jlong{-10});
}

TEST_F(InterpreterArithmeticTest, LMUL_Basic) {
  EXPECT_EQ(executeStaticMethod<Jlong>(kClassName, "testLMUL", Jlong{5}, Jlong{6}), Jlong{30});
  EXPECT_EQ(executeStaticMethod<Jlong>(kClassName, "testLMUL", Jlong{-5}, Jlong{6}), Jlong{-30});
}

TEST_F(InterpreterArithmeticTest, LDIV_Basic) {
  EXPECT_EQ(executeStaticMethod<Jlong>(kClassName, "testLDIV", Jlong{20}, Jlong{5}), Jlong{4});
  EXPECT_EQ(executeStaticMethod<Jlong>(kClassName, "testLDIV", Jlong{21}, Jlong{5}), Jlong{4});
}

TEST_F(InterpreterArithmeticTest, LDIV_DivideByZero) {
  EXPECT_THROW(executeStaticMethod<Jlong>(kClassName, "testLDIV", Jlong{10}, Jlong{0}), std::runtime_error);
}

TEST_F(InterpreterArithmeticTest, LREM_Basic) {
  EXPECT_EQ(executeStaticMethod<Jlong>(kClassName, "testLREM", Jlong{20}, Jlong{5}), Jlong{0});
  EXPECT_EQ(executeStaticMethod<Jlong>(kClassName, "testLREM", Jlong{21}, Jlong{5}), Jlong{1});
}

TEST_F(InterpreterArithmeticTest, LREM_DivideByZero) {
  EXPECT_THROW(executeStaticMethod<Jlong>(kClassName, "testLREM", Jlong{10}, Jlong{0}), std::runtime_error);
}

TEST_F(InterpreterArithmeticTest, LNEG_Basic) {
  EXPECT_EQ(executeStaticMethod<Jlong>(kClassName, "testLNEG", Jlong{10}), Jlong{-10});
  EXPECT_EQ(executeStaticMethod<Jlong>(kClassName, "testLNEG", Jlong{-10}), Jlong{10});
}

TEST_F(InterpreterArithmeticTest, LADD_MaxPlusOneWrapsToMin) {
  EXPECT_EQ(
    executeStaticMethod<Jlong>(kClassName, "testLADD", std::numeric_limits<Jlong>::max(), Jlong{1}),
    std::numeric_limits<Jlong>::min());
}

TEST_F(InterpreterArithmeticTest, LSUB_MinMinusOneWrapsToMax) {
  EXPECT_EQ(
    executeStaticMethod<Jlong>(kClassName, "testLSUB", std::numeric_limits<Jlong>::min(), Jlong{1}),
    std::numeric_limits<Jlong>::max());
}

TEST_F(InterpreterArithmeticTest, LMUL_OverflowWrapsAsTwosComplement) {
  EXPECT_EQ(
    executeStaticMethod<Jlong>(kClassName, "testLMUL", std::numeric_limits<Jlong>::max(), Jlong{2}),
    Jlong{-2});
}

TEST_F(InterpreterArithmeticTest, LNEG_MinWrapsToMin) {
  EXPECT_EQ(executeStaticMethod<Jlong>(kClassName, "testLNEG", std::numeric_limits<Jlong>::min()),
            std::numeric_limits<Jlong>::min());
}

TEST_F(InterpreterArithmeticTest, LDIV_MinByNegativeOneReturnsMin) {
  EXPECT_EQ(executeStaticMethod<Jlong>(kClassName, "testLDIV", std::numeric_limits<Jlong>::min(),
                                       Jlong{-1}),
            std::numeric_limits<Jlong>::min());
}

TEST_F(InterpreterArithmeticTest, LREM_MinByNegativeOneReturnsZero) {
  EXPECT_EQ(executeStaticMethod<Jlong>(kClassName, "testLREM", std::numeric_limits<Jlong>::min(),
                                       Jlong{-1}),
            Jlong{0});
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
  EXPECT_EQ(executeStaticMethod<Jlong>(kClassName, "testLAND", Jlong{0x0F}, Jlong{0xF0}), Jlong{0x00});
  EXPECT_EQ(executeStaticMethod<Jlong>(kClassName, "testLAND", Jlong{0xFF}, Jlong{0x0F}), Jlong{0x0F});
  EXPECT_EQ(executeStaticMethod<Jlong>(kClassName, "testLAND", Jlong{0xAA}, Jlong{0x55}), Jlong{0x00});
  EXPECT_EQ(executeStaticMethod<Jlong>(kClassName, "testLAND", Jlong{0xFFFFFFFF}, Jlong{0xFFFFFFFF}),
            Jlong{0xFFFFFFFF});
}

TEST_F(InterpreterArithmeticTest, LOR_Basic) {
  EXPECT_EQ(executeStaticMethod<Jlong>(kClassName, "testLOR", Jlong{0x0F}, Jlong{0xF0}), Jlong{0xFF});
  EXPECT_EQ(executeStaticMethod<Jlong>(kClassName, "testLOR", Jlong{0xAA}, Jlong{0x55}), Jlong{0xFF});
  EXPECT_EQ(executeStaticMethod<Jlong>(kClassName, "testLOR", Jlong{0}, Jlong{0xFF}), Jlong{0xFF});
  EXPECT_EQ(executeStaticMethod<Jlong>(kClassName, "testLOR", Jlong{0xFF}, Jlong{0x00}), Jlong{0xFF});
}

TEST_F(InterpreterArithmeticTest, LXOR_Basic) {
  EXPECT_EQ(executeStaticMethod<Jlong>(kClassName, "testLXOR", Jlong{0xFF}, Jlong{0xFF}), Jlong{0x00});
  EXPECT_EQ(executeStaticMethod<Jlong>(kClassName, "testLXOR", Jlong{0xAA}, Jlong{0x55}), Jlong{0xFF});
  EXPECT_EQ(executeStaticMethod<Jlong>(kClassName, "testLXOR", Jlong{0x0F}, Jlong{0xF0}), Jlong{0xFF});
  EXPECT_EQ(executeStaticMethod<Jlong>(kClassName, "testLXOR", Jlong{0}, Jlong{0xFF}), Jlong{0xFF});
}

TEST_F(InterpreterArithmeticTest, LSHL_Basic) {
  EXPECT_EQ(executeStaticMethod<Jlong>(kClassName, "testLSHL", Jlong{1}, 1), Jlong{2});
  EXPECT_EQ(executeStaticMethod<Jlong>(kClassName, "testLSHL", Jlong{1}, 3), Jlong{8});
  EXPECT_EQ(executeStaticMethod<Jlong>(kClassName, "testLSHL", Jlong{5}, 2), Jlong{20});
  EXPECT_EQ(executeStaticMethod<Jlong>(kClassName, "testLSHL", Jlong{0x4000000000000000}, 1),
            std::numeric_limits<Jlong>::min());
}

TEST_F(InterpreterArithmeticTest, LSHR_Basic) {
  EXPECT_EQ(executeStaticMethod<Jlong>(kClassName, "testLSHR", Jlong{8}, 1), Jlong{4});
  EXPECT_EQ(executeStaticMethod<Jlong>(kClassName, "testLSHR", Jlong{32}, 3), Jlong{4});
  EXPECT_EQ(executeStaticMethod<Jlong>(kClassName, "testLSHR", Jlong{-8}, 1),
            Jlong{-4});  // Arithmetic shift preserves sign
  EXPECT_EQ(executeStaticMethod<Jlong>(kClassName, "testLSHR", Jlong{-32}, 3), Jlong{-4});
}

TEST_F(InterpreterArithmeticTest, LUSHR_Basic) {
  EXPECT_EQ(executeStaticMethod<Jlong>(kClassName, "testLUSHR", Jlong{8}, 1), Jlong{4});
  EXPECT_EQ(executeStaticMethod<Jlong>(kClassName, "testLUSHR", Jlong{32}, 3), Jlong{4});
  EXPECT_EQ(executeStaticMethod<Jlong>(kClassName, "testLUSHR", Jlong{-8}, 1),
            Jlong{0x7FFFFFFFFFFFFFFC});  // Logical shift
  EXPECT_EQ(executeStaticMethod<Jlong>(kClassName, "testLUSHR", Jlong{-1}, 63),
            Jlong{1});  // Logical shift right 63 bits
}

}  // namespace
