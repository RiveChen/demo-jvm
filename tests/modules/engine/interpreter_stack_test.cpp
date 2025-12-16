#include <gtest/gtest.h>

#include "interpreter_test_base.h"

// ============================================================================
// Stack Operation Tests
// ============================================================================

class InterpreterStackTest : public InterpreterTestBase {};

TEST_F(InterpreterStackTest, POP_Basic) {
  EXPECT_EQ(executeIntMethod("tests.data.java.StackTest", "testPOP", "(II)I", 10, 20), 30);
}

TEST_F(InterpreterStackTest, POP2_Basic) {
  EXPECT_EQ(executeLongMethod("tests.data.java.StackTest", "testPOP2", "(JJ)J", 10L, 20L), 30L);
}

TEST_F(InterpreterStackTest, DUP_Basic) {
  EXPECT_EQ(executeIntMethod1Arg("tests.data.java.StackTest", "testDUP", "(I)I", 5), 10);
}

TEST_F(InterpreterStackTest, DUP_X1_Basic) {
  EXPECT_EQ(executeIntMethod("tests.data.java.StackTest", "testDUP_X1", "(II)I", 5, 10), 20);
}

TEST_F(InterpreterStackTest, DUP_X2_Basic) {
  auto* klass = loader_->loadClass("tests.data.java.StackTest");
  ASSERT_NE(klass, nullptr);

  auto* method = klass->findMethod("testDUP_X2", "(III)I");
  ASSERT_NE(method, nullptr);

  runtime::Thread     thread;
  engine::Interpreter interpreter;

  runtime::Frame caller_frame(method);
  thread.pushFrame(std::move(caller_frame));
  thread.setPC(method->getCode().size());

  runtime::Frame callee_frame(method);
  callee_frame.getLocalVariables().setInt(0, 5);
  callee_frame.getLocalVariables().setInt(1, 10);
  callee_frame.getLocalVariables().setInt(2, 15);
  thread.pushFrame(std::move(callee_frame));
  thread.setPC(0);

  interpreter.interpret(&thread);

  if (!thread.isStackEmpty()) {
    auto& current_frame = thread.getCurrentFrame();
    EXPECT_EQ(current_frame.getOperandStack().popInt(), 35);  // 5 + 10 + 15 + 5 = 35
  } else {
    FAIL() << "Stack is empty after execution";
  }
}

TEST_F(InterpreterStackTest, DUP2_Basic) {
  EXPECT_EQ(executeLongMethod1Arg("tests.data.java.StackTest", "testDUP2", "(J)J", 5L), 10L);
}

TEST_F(InterpreterStackTest, SWAP_Basic) {
  EXPECT_EQ(executeIntMethod("tests.data.java.StackTest", "testSWAP", "(II)I", 10, 20), 30);
}
