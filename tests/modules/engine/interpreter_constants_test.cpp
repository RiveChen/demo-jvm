#include <gtest/gtest.h>

#include "interpreter_test_base.h"

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

// // ============================================================================
// // BIPUSH Tests
// // ============================================================================

// Note: BIPUSH and SIPUSH tests are disabled because Java compilers
// typically use LDC instructions instead of BIPUSH/SIPUSH for constant values.
// These instructions are more commonly used in hand-written bytecode.
// To properly test BIPUSH/SIPUSH, we would need to write bytecode directly.

// TEST_F(InterpreterConstantsTest, BIPUSH_127) {
//   EXPECT_EQ(executeIntMethod0Arg(kClassName, "testBIPUSH_127", "()I"), 127);
// }

// TEST_F(InterpreterConstantsTest, BIPUSH_Neg128) {
//   EXPECT_EQ(executeIntMethod0Arg(kClassName, "testBIPUSH_Neg128", "()I"),
//   -128);
// }

// TEST_F(InterpreterConstantsTest, BIPUSH_0) {
//   EXPECT_EQ(executeIntMethod0Arg(kClassName, "testBIPUSH_0", "()I"), 0);
// }

// // ============================================================================
// // SIPUSH Tests
// // ============================================================================

// TEST_F(InterpreterConstantsTest, SIPUSH_32767) {
//   EXPECT_EQ(executeIntMethod0Arg(kClassName, "testSIPUSH_32767", "()I"),
//   32767);
// }

// TEST_F(InterpreterConstantsTest, SIPUSH_Neg32768) {
//   EXPECT_EQ(executeIntMethod0Arg(kClassName, "testSIPUSH_Neg32768", "()I"),
//             -32768);
// }

// TEST_F(InterpreterConstantsTest, SIPUSH_100) {
//   EXPECT_EQ(executeIntMethod0Arg(kClassName, "testSIPUSH_100", "()I"), 100);
// }

// ============================================================================
// ACONST_NULL Tests
// ============================================================================

TEST_F(InterpreterConstantsTest, ACONST_NULL) {
  auto* klass = loader_->loadClass(kClassName);
  ASSERT_NE(klass, nullptr);

  auto* method = klass->findMethod("testACONST_NULL", "()Ljava/lang/Object;");
  ASSERT_NE(method, nullptr);

  runtime::Thread     thread;
  engine::Interpreter interpreter;

  runtime::Frame caller_frame(method);
  caller_frame.setCallerPC(method->getCode().size());
  thread.pushFrame(std::move(caller_frame));
  thread.setPC(method->getCode().size());

  runtime::Frame callee_frame(method);
  thread.pushFrame(std::move(callee_frame));
  thread.setPC(0);

  interpreter.interpret(&thread);

  if (!thread.isStackEmpty()) {
    auto& current_frame = thread.getCurrentFrame();
    auto  ref           = current_frame.getOperandStack().popRef();
    EXPECT_EQ(ref, nullptr);
  } else {
    FAIL() << "Stack is empty after execution";
  }
}
