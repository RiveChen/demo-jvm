#include "engine/opcode.hpp"
#include "interpreter_test_base.hpp"

#include <gtest/gtest.h>

// ============================================================================
// Integer Constants Tests
// ============================================================================

class InterpreterConstantsTest : public InterpreterTestBase {
 public:
  static constexpr const char* kClassName = "tests.data.java.ConstantTest";
};

TEST_F(InterpreterConstantsTest, ICONST_M1) {
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testICONST_M1"), -1);
}

TEST_F(InterpreterConstantsTest, ICONST_0) {
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testICONST_0"), 0);
}

TEST_F(InterpreterConstantsTest, ICONST_1) {
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testICONST_1"), 1);
}

TEST_F(InterpreterConstantsTest, ICONST_2) {
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testICONST_2"), 2);
}

TEST_F(InterpreterConstantsTest, ICONST_3) {
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testICONST_3"), 3);
}

TEST_F(InterpreterConstantsTest, ICONST_4) {
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testICONST_4"), 4);
}

TEST_F(InterpreterConstantsTest, ICONST_5) {
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testICONST_5"), 5);
}

// ============================================================================
// Long Constants Tests
// ============================================================================

TEST_F(InterpreterConstantsTest, LCONST_0) {
  EXPECT_EQ(executeStaticMethod<Jlong>(kClassName, "testLCONST_0"), 0LL);
}

TEST_F(InterpreterConstantsTest, LCONST_1) {
  EXPECT_EQ(executeStaticMethod<Jlong>(kClassName, "testLCONST_1"), 1LL);
}

// ============================================================================
// Float Constants Tests
// ============================================================================

TEST_F(InterpreterConstantsTest, FCONST_0) {
  EXPECT_FLOAT_EQ(executeStaticMethod<Jfloat>(kClassName, "testFCONST_0"), 0.0F);
}

TEST_F(InterpreterConstantsTest, FCONST_1) {
  EXPECT_FLOAT_EQ(executeStaticMethod<Jfloat>(kClassName, "testFCONST_1"), 1.0F);
}

TEST_F(InterpreterConstantsTest, FCONST_2) {
  EXPECT_FLOAT_EQ(executeStaticMethod<Jfloat>(kClassName, "testFCONST_2"), 2.0F);
}

// ============================================================================
// Double Constants Tests
// ============================================================================

TEST_F(InterpreterConstantsTest, DCONST_0) {
  EXPECT_DOUBLE_EQ(executeStaticMethod<Jdouble>(kClassName, "testDCONST_0"), 0.0);
}

TEST_F(InterpreterConstantsTest, DCONST_1) {
  EXPECT_DOUBLE_EQ(executeStaticMethod<Jdouble>(kClassName, "testDCONST_1"), 1.0);
}

// ============================================================================
// BIPUSH Tests
// ============================================================================

TEST_F(InterpreterConstantsTest, BIPUSH_PushesPositiveBoundary) {
  auto* klass = loader_->loadClass(kClassName);
  ASSERT_NE(klass, nullptr);
  auto* method = klass->findMethod("testBIPUSH_127", "()I");
  ASSERT_NE(method, nullptr);
  ASSERT_FALSE(method->getCode().empty());
  ASSERT_EQ(method->getCode().front(), engine::BIPUSH);

  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testBIPUSH_127"), 127);
}

TEST_F(InterpreterConstantsTest, BIPUSH_SignExtendsNegativeBoundary) {
  auto* klass = loader_->loadClass(kClassName);
  ASSERT_NE(klass, nullptr);
  auto* method = klass->findMethod("testBIPUSH_Neg128", "()I");
  ASSERT_NE(method, nullptr);
  ASSERT_FALSE(method->getCode().empty());
  ASSERT_EQ(method->getCode().front(), engine::BIPUSH);

  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testBIPUSH_Neg128"), -128);
}

// ============================================================================
// SIPUSH Tests
// ============================================================================

TEST_F(InterpreterConstantsTest, SIPUSH_SignExtendsNegativeImmediate) {
  auto* klass = loader_->loadClass(kClassName);
  ASSERT_NE(klass, nullptr);
  auto* method = klass->findMethod("testSIPUSH_Neg32768", "()I");
  ASSERT_NE(method, nullptr);
  ASSERT_FALSE(method->getCode().empty());
  ASSERT_EQ(method->getCode().front(), engine::SIPUSH);

  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testSIPUSH_Neg32768"), -32768);
}

// ============================================================================
// ACONST_NULL Tests
// ============================================================================

TEST_F(InterpreterConstantsTest, ACONST_NULL) {
  Jref result = executeStaticMethod<Jref>(kClassName, "testACONST_NULL", "()Ljava/lang/Object;");
  EXPECT_EQ(result, nullptr);
}
