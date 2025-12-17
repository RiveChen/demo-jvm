#include <gtest/gtest.h>

#include "common/types.h"
#include "interpreter_test_base.h"

using namespace jvm;

namespace {

class InterpreterControlFlowTest : public InterpreterTestBase {
 public:
  static constexpr const char* kClassName = "tests.data.java.ControlFlowTest";
};

// ============================================================================
// IFEQ Tests - Branch if int value equals 0
// ============================================================================

TEST_F(InterpreterControlFlowTest, IFEQ_True) {
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testIFEQ_True", 0), 1);
}

TEST_F(InterpreterControlFlowTest, IFEQ_False) {
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testIFEQ_False", 5), 1);
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testIFEQ_False", -5), 1);
}

// ============================================================================
// IFNE Tests - Branch if int value not equal to 0
// ============================================================================

TEST_F(InterpreterControlFlowTest, IFNE_True) {
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testIFNE_True", 5), 1);
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testIFNE_True", -5), 1);
}

TEST_F(InterpreterControlFlowTest, IFNE_False) {
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testIFNE_False", 0), 1);
}

// ============================================================================
// IFLT Tests - Branch if int value less than 0
// ============================================================================

TEST_F(InterpreterControlFlowTest, IFLT_True) {
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testIFLT_True", -5), 1);
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testIFLT_True", -1), 1);
}

TEST_F(InterpreterControlFlowTest, IFLT_False) {
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testIFLT_False", 0), 1);
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testIFLT_False", 5), 1);
}

// ============================================================================
// IFGE Tests - Branch if int value greater than or equal to 0
// ============================================================================

TEST_F(InterpreterControlFlowTest, IFGE_True) {
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testIFGE_True", 0), 1);
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testIFGE_True", 5), 1);
}

TEST_F(InterpreterControlFlowTest, IFGE_False) {
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testIFGE_False", -5), 1);
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testIFGE_False", -1), 1);
}

// ============================================================================
// IFGT Tests - Branch if int value greater than 0
// ============================================================================

TEST_F(InterpreterControlFlowTest, IFGT_True) {
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testIFGT_True", 5), 1);
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testIFGT_True", 1), 1);
}

TEST_F(InterpreterControlFlowTest, IFGT_False) {
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testIFGT_False", 0), 1);
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testIFGT_False", -5), 1);
}

// ============================================================================
// IFLE Tests - Branch if int value less than or equal to 0
// ============================================================================

TEST_F(InterpreterControlFlowTest, IFLE_True) {
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testIFLE_True", 0), 1);
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testIFLE_True", -5), 1);
}

TEST_F(InterpreterControlFlowTest, IFLE_False) {
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testIFLE_False", 5), 1);
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testIFLE_False", 1), 1);
}

// ============================================================================
// IF_ICMPEQ Tests - Branch if two int values are equal
// ============================================================================

TEST_F(InterpreterControlFlowTest, IF_ICMPEQ_True) {
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testIF_ICMPEQ_True", 5, 5), 1);
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testIF_ICMPEQ_True", 0, 0), 1);
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testIF_ICMPEQ_True", -10, -10), 1);
}

TEST_F(InterpreterControlFlowTest, IF_ICMPEQ_False) {
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testIF_ICMPEQ_False", 5, 10), 1);
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testIF_ICMPEQ_False", 0, 5), 1);
}

// ============================================================================
// IF_ICMPNE Tests - Branch if two int values are not equal
// ============================================================================

TEST_F(InterpreterControlFlowTest, IF_ICMPNE_True) {
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testIF_ICMPNE_True", 5, 10), 1);
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testIF_ICMPNE_True", 0, 5), 1);
}

TEST_F(InterpreterControlFlowTest, IF_ICMPNE_False) {
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testIF_ICMPNE_False", 5, 5), 1);
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testIF_ICMPNE_False", 0, 0), 1);
}

// ============================================================================
// IF_ICMPLT Tests - Branch if first int value less than second
// ============================================================================

TEST_F(InterpreterControlFlowTest, IF_ICMPLT_True) {
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testIF_ICMPLT_True", 5, 10), 1);
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testIF_ICMPLT_True", -10, -5), 1);
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testIF_ICMPLT_True", -5, 5), 1);
}

TEST_F(InterpreterControlFlowTest, IF_ICMPLT_False) {
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testIF_ICMPLT_False", 10, 5), 1);
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testIF_ICMPLT_False", 5, 5), 1);
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testIF_ICMPLT_False", 5, -5), 1);
}

// ============================================================================
// IF_ICMPGE Tests - Branch if first int value greater than or equal to second
// ============================================================================

TEST_F(InterpreterControlFlowTest, IF_ICMPGE_True) {
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testIF_ICMPGE_True", 10, 5), 1);
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testIF_ICMPGE_True", 5, 5), 1);
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testIF_ICMPGE_True", 5, -5), 1);
}

TEST_F(InterpreterControlFlowTest, IF_ICMPGE_False) {
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testIF_ICMPGE_False", 5, 10), 1);
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testIF_ICMPGE_False", -5, 5), 1);
}

// ============================================================================
// IF_ICMPGT Tests - Branch if first int value greater than second
// ============================================================================

TEST_F(InterpreterControlFlowTest, IF_ICMPGT_True) {
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testIF_ICMPGT_True", 10, 5), 1);
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testIF_ICMPGT_True", 5, -5), 1);
}

TEST_F(InterpreterControlFlowTest, IF_ICMPGT_False) {
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testIF_ICMPGT_False", 5, 10), 1);
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testIF_ICMPGT_False", 5, 5), 1);
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testIF_ICMPGT_False", -5, 5), 1);
}

// ============================================================================
// IF_ICMPLE Tests - Branch if first int value less than or equal to second
// ============================================================================

TEST_F(InterpreterControlFlowTest, IF_ICMPLE_True) {
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testIF_ICMPLE_True", 5, 10), 1);
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testIF_ICMPLE_True", 5, 5), 1);
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testIF_ICMPLE_True", -5, 5), 1);
}

TEST_F(InterpreterControlFlowTest, IF_ICMPLE_False) {
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testIF_ICMPLE_False", 10, 5), 1);
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testIF_ICMPLE_False", 5, -5), 1);
}

// ============================================================================
// IFNULL Tests - Branch if reference value is null
// TODO: Uncomment when object support is implemented
// ============================================================================
/*
TEST_F(InterpreterControlFlowTest, IFNULL_True) {
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testIFNULL_True", static_cast<Jref>(nullptr)),
            1);
}

TEST_F(InterpreterControlFlowTest, IFNULL_False) {
  // Create a dummy object reference (non-null)
  // Note: In a real JVM, this would be a proper object reference
  // For testing, we use a non-null pointer value
  void* dummy_obj = reinterpret_cast<void*>(0x1234);
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testIFNULL_False", static_cast<Jref>(dummy_obj)),
            1);
}
*/

// ============================================================================
// IFNONNULL Tests - Branch if reference value is not null
// TODO: Uncomment when object support is implemented
// ============================================================================
/*
TEST_F(InterpreterControlFlowTest, IFNONNULL_True) {
  // Create a dummy object reference (non-null)
  void* dummy_obj = reinterpret_cast<void*>(0x1234);
  EXPECT_EQ(
    executeStaticMethod<Jint>(kClassName, "testIFNONNULL_True", static_cast<Jref>(dummy_obj)), 1);
}

TEST_F(InterpreterControlFlowTest, IFNONNULL_False) {
  EXPECT_EQ(
    executeStaticMethod<Jint>(kClassName, "testIFNONNULL_False", static_cast<Jref>(nullptr)), 1);
}
*/

// ============================================================================
// IF_ACMPEQ Tests - Branch if two reference values are equal
// TODO: Uncomment when object support is implemented
// ============================================================================
/*
TEST_F(InterpreterControlFlowTest, IF_ACMPEQ_True) {
  void* dummy_obj = reinterpret_cast<void*>(0x1234);
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testIF_ACMPEQ_True",
                                      static_cast<Jref>(dummy_obj), static_cast<Jref>(dummy_obj)),
            1);
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testIF_ACMPEQ_True", static_cast<Jref>(nullptr),
                                      static_cast<Jref>(nullptr)),
            1);
}

TEST_F(InterpreterControlFlowTest, IF_ACMPEQ_False) {
  void* obj1 = reinterpret_cast<void*>(0x1234);
  void* obj2 = reinterpret_cast<void*>(0x5678);
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testIF_ACMPEQ_False", static_cast<Jref>(obj1),
                                      static_cast<Jref>(obj2)),
            1);
}
*/

// ============================================================================
// IF_ACMPNE Tests - Branch if two reference values are not equal
// TODO: Uncomment when object support is implemented
// ============================================================================
/*
TEST_F(InterpreterControlFlowTest, IF_ACMPNE_True) {
  void* obj1 = reinterpret_cast<void*>(0x1234);
  void* obj2 = reinterpret_cast<void*>(0x5678);
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testIF_ACMPNE_True", static_cast<Jref>(obj1),
                                      static_cast<Jref>(obj2)),
            1);
}

TEST_F(InterpreterControlFlowTest, IF_ACMPNE_False) {
  void* dummy_obj = reinterpret_cast<void*>(0x1234);
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testIF_ACMPNE_False",
                                      static_cast<Jref>(dummy_obj), static_cast<Jref>(dummy_obj)),
            1);
}
*/

// ============================================================================
// GOTO Tests - Unconditional branch
// ============================================================================

TEST_F(InterpreterControlFlowTest, GOTO_Positive) {
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testGOTO", 5), 10);
}

TEST_F(InterpreterControlFlowTest, GOTO_Negative) {
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testGOTO", -5), 20);
}

TEST_F(InterpreterControlFlowTest, GOTO_Zero) {
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testGOTO", 0), 20);
}

// ============================================================================
// TABLESWITCH Tests - Table switch statement
// ============================================================================

TEST_F(InterpreterControlFlowTest, TABLESWITCH_Case0) {
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testTABLESWITCH", 0), 100);
}

TEST_F(InterpreterControlFlowTest, TABLESWITCH_Case1) {
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testTABLESWITCH", 1), 200);
}

TEST_F(InterpreterControlFlowTest, TABLESWITCH_Case2) {
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testTABLESWITCH", 2), 300);
}

TEST_F(InterpreterControlFlowTest, TABLESWITCH_Case3) {
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testTABLESWITCH", 3), 400);
}

TEST_F(InterpreterControlFlowTest, TABLESWITCH_Default) {
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testTABLESWITCH", 5), 0);
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testTABLESWITCH", -1), 0);
}

TEST_F(InterpreterControlFlowTest, TABLESWITCH_Default2) {
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testTABLESWITCH_Default", 5), 999);
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testTABLESWITCH_Default", -1), 999);
}

// ============================================================================
// LOOKUPSWITCH Tests - Lookup switch statement
// ============================================================================

TEST_F(InterpreterControlFlowTest, LOOKUPSWITCH_Case10) {
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testLOOKUPSWITCH", 10), 1000);
}

TEST_F(InterpreterControlFlowTest, LOOKUPSWITCH_Case20) {
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testLOOKUPSWITCH", 20), 2000);
}

TEST_F(InterpreterControlFlowTest, LOOKUPSWITCH_Case30) {
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testLOOKUPSWITCH", 30), 3000);
}

TEST_F(InterpreterControlFlowTest, LOOKUPSWITCH_Default) {
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testLOOKUPSWITCH", 5), 0);
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testLOOKUPSWITCH", 15), 0);
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testLOOKUPSWITCH", 25), 0);
}

TEST_F(InterpreterControlFlowTest, LOOKUPSWITCH_Default2) {
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testLOOKUPSWITCH_Default", 5), 9999);
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testLOOKUPSWITCH_Default", 15), 9999);
}

// ============================================================================
// Complex Control Flow Tests
// ============================================================================

TEST_F(InterpreterControlFlowTest, NestedIf_AllPositive) {
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testNestedIf", 5, 5), 1);
}

TEST_F(InterpreterControlFlowTest, NestedIf_FirstPositiveSecondNegative) {
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testNestedIf", 5, -5), 2);
}

TEST_F(InterpreterControlFlowTest, NestedIf_FirstNegativeSecondPositive) {
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testNestedIf", -5, 5), 3);
}

TEST_F(InterpreterControlFlowTest, NestedIf_AllNegative) {
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testNestedIf", -5, -5), 4);
}

TEST_F(InterpreterControlFlowTest, WhileLoop) {
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testWhileLoop", 0), 0);
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testWhileLoop", 1), 1);
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testWhileLoop", 5), 15);  // 5+4+3+2+1
}

// ============================================================================
// Factorial Tests
// ============================================================================

TEST_F(InterpreterControlFlowTest, Factorial_Zero) {
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "factorial", 0), 1);
}

TEST_F(InterpreterControlFlowTest, Factorial_One) {
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "factorial", 1), 1);
}

TEST_F(InterpreterControlFlowTest, Factorial_Two) {
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "factorial", 2), 2);
}

TEST_F(InterpreterControlFlowTest, Factorial_Three) {
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "factorial", 3), 6);
}

TEST_F(InterpreterControlFlowTest, Factorial_Four) {
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "factorial", 4), 24);
}

TEST_F(InterpreterControlFlowTest, Factorial_Five) {
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "factorial", 5), 120);
}

TEST_F(InterpreterControlFlowTest, Factorial_Negative) {
  // For negative input, should return 1 (as per implementation)
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "factorial", -1), 1);
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "factorial", -5), 1);
}

}  // namespace
