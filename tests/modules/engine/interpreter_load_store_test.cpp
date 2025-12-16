#include <gtest/gtest.h>

#include "interpreter_test_base.h"

// ============================================================================
// Integer Load Tests
// ============================================================================

class InterpreterLoadStoreTest : public InterpreterTestBase {};

TEST_F(InterpreterLoadStoreTest, ILOAD_Basic) {
  EXPECT_EQ(executeIntMethod1Arg("tests.data.java.LoadStoreTest", "testILOAD", "(I)I", 42), 42);
  EXPECT_EQ(executeIntMethod1Arg("tests.data.java.LoadStoreTest", "testILOAD", "(I)I", -42), -42);
  EXPECT_EQ(executeIntMethod1Arg("tests.data.java.LoadStoreTest", "testILOAD", "(I)I", 0), 0);
}

TEST_F(InterpreterLoadStoreTest, ILOAD_0) {
  EXPECT_EQ(executeIntMethod1Arg("tests.data.java.LoadStoreTest", "testILOAD_0", "(I)I", 100), 100);
}

TEST_F(InterpreterLoadStoreTest, ILOAD_1) {
  auto* klass = loader_->loadClass("tests.data.java.LoadStoreTest");
  ASSERT_NE(klass, nullptr);

  auto* method = klass->findMethod("testILOAD_1", "(II)I");
  ASSERT_NE(method, nullptr);

  runtime::Thread     thread;
  engine::Interpreter interpreter;

  runtime::Frame caller_frame(method);
  caller_frame.setCallerPC(method->getCode().size());
  thread.pushFrame(std::move(caller_frame));
  thread.setPC(method->getCode().size());

  runtime::Frame callee_frame(method);
  callee_frame.getLocalVariables().setInt(0, 10);
  callee_frame.getLocalVariables().setInt(1, 20);
  thread.pushFrame(std::move(callee_frame));
  thread.setPC(0);

  interpreter.interpret(&thread);

  if (!thread.isStackEmpty()) {
    auto& current_frame = thread.getCurrentFrame();
    EXPECT_EQ(current_frame.getOperandStack().popInt(), 20);
  } else {
    FAIL() << "Stack is empty after execution";
  }
}

TEST_F(InterpreterLoadStoreTest, ILOAD_2) {
  auto* klass = loader_->loadClass("tests.data.java.LoadStoreTest");
  ASSERT_NE(klass, nullptr);

  auto* method = klass->findMethod("testILOAD_2", "(III)I");
  ASSERT_NE(method, nullptr);

  runtime::Thread     thread;
  engine::Interpreter interpreter;

  runtime::Frame caller_frame(method);
  caller_frame.setCallerPC(method->getCode().size());
  thread.pushFrame(std::move(caller_frame));
  thread.setPC(method->getCode().size());

  runtime::Frame callee_frame(method);
  callee_frame.getLocalVariables().setInt(0, 10);
  callee_frame.getLocalVariables().setInt(1, 20);
  callee_frame.getLocalVariables().setInt(2, 30);
  thread.pushFrame(std::move(callee_frame));
  thread.setPC(0);

  interpreter.interpret(&thread);

  if (!thread.isStackEmpty()) {
    auto& current_frame = thread.getCurrentFrame();
    EXPECT_EQ(current_frame.getOperandStack().popInt(), 30);
  } else {
    FAIL() << "Stack is empty after execution";
  }
}

TEST_F(InterpreterLoadStoreTest, ILOAD_3) {
  auto* klass = loader_->loadClass("tests.data.java.LoadStoreTest");
  ASSERT_NE(klass, nullptr);

  auto* method = klass->findMethod("testILOAD_3", "(IIII)I");
  ASSERT_NE(method, nullptr);

  runtime::Thread     thread;
  engine::Interpreter interpreter;

  runtime::Frame caller_frame(method);
  caller_frame.setCallerPC(method->getCode().size());
  thread.pushFrame(std::move(caller_frame));
  thread.setPC(method->getCode().size());

  runtime::Frame callee_frame(method);
  callee_frame.getLocalVariables().setInt(0, 10);
  callee_frame.getLocalVariables().setInt(1, 20);
  callee_frame.getLocalVariables().setInt(2, 30);
  callee_frame.getLocalVariables().setInt(3, 40);
  thread.pushFrame(std::move(callee_frame));
  thread.setPC(0);

  interpreter.interpret(&thread);

  if (!thread.isStackEmpty()) {
    auto& current_frame = thread.getCurrentFrame();
    EXPECT_EQ(current_frame.getOperandStack().popInt(), 40);
  } else {
    FAIL() << "Stack is empty after execution";
  }
}

// ============================================================================
// Integer Store Tests
// ============================================================================

TEST_F(InterpreterLoadStoreTest, ISTORE_Basic) {
  EXPECT_EQ(executeIntMethod1Arg("tests.data.java.LoadStoreTest", "testISTORE", "(I)I", 42), 42);
}

TEST_F(InterpreterLoadStoreTest, ISTORE_0) {
  EXPECT_EQ(executeIntMethod1Arg("tests.data.java.LoadStoreTest", "testISTORE_0", "(I)I", 100),
            100);
}

TEST_F(InterpreterLoadStoreTest, ISTORE_1) {
  auto* klass = loader_->loadClass("tests.data.java.LoadStoreTest");
  ASSERT_NE(klass, nullptr);

  auto* method = klass->findMethod("testISTORE_1", "(II)I");
  ASSERT_NE(method, nullptr);

  runtime::Thread     thread;
  engine::Interpreter interpreter;

  runtime::Frame caller_frame(method);
  caller_frame.setCallerPC(method->getCode().size());
  thread.pushFrame(std::move(caller_frame));
  thread.setPC(method->getCode().size());

  runtime::Frame callee_frame(method);
  callee_frame.getLocalVariables().setInt(0, 10);
  callee_frame.getLocalVariables().setInt(1, 20);
  thread.pushFrame(std::move(callee_frame));
  thread.setPC(0);

  interpreter.interpret(&thread);

  if (!thread.isStackEmpty()) {
    auto& current_frame = thread.getCurrentFrame();
    EXPECT_EQ(current_frame.getOperandStack().popInt(), 20);
  } else {
    FAIL() << "Stack is empty after execution";
  }
}

// ============================================================================
// Long Load/Store Tests
// ============================================================================

TEST_F(InterpreterLoadStoreTest, LLOAD_Basic) {
  EXPECT_EQ(executeLongMethod1Arg("tests.data.java.LoadStoreTest", "testLLOAD", "(J)J", 42L), 42L);
}

TEST_F(InterpreterLoadStoreTest, LLOAD_0) {
  EXPECT_EQ(executeLongMethod1Arg("tests.data.java.LoadStoreTest", "testLLOAD_0", "(J)J", 100L),
            100L);
}

TEST_F(InterpreterLoadStoreTest, LSTORE_Basic) {
  EXPECT_EQ(executeLongMethod1Arg("tests.data.java.LoadStoreTest", "testLSTORE", "(J)J", 42L), 42L);
}

TEST_F(InterpreterLoadStoreTest, LSTORE_0) {
  EXPECT_EQ(executeLongMethod1Arg("tests.data.java.LoadStoreTest", "testLSTORE_0", "(J)J", 100L),
            100L);
}

// ============================================================================
// Float Load/Store Tests
// ============================================================================

TEST_F(InterpreterLoadStoreTest, FLOAD_Basic) {
  EXPECT_FLOAT_EQ(
    executeFloatMethod1Arg("tests.data.java.LoadStoreTest", "testFLOAD", "(F)F", 42.5F), 42.5F);
}

TEST_F(InterpreterLoadStoreTest, FLOAD_0) {
  EXPECT_FLOAT_EQ(
    executeFloatMethod1Arg("tests.data.java.LoadStoreTest", "testFLOAD_0", "(F)F", 100.0F), 100.0F);
}

TEST_F(InterpreterLoadStoreTest, FSTORE_Basic) {
  EXPECT_FLOAT_EQ(
    executeFloatMethod1Arg("tests.data.java.LoadStoreTest", "testFSTORE", "(F)F", 42.5F), 42.5F);
}

TEST_F(InterpreterLoadStoreTest, FSTORE_0) {
  EXPECT_FLOAT_EQ(
    executeFloatMethod1Arg("tests.data.java.LoadStoreTest", "testFSTORE_0", "(F)F", 100.0F),
    100.0F);
}

// ============================================================================
// Double Load/Store Tests
// ============================================================================

TEST_F(InterpreterLoadStoreTest, DLOAD_Basic) {
  EXPECT_DOUBLE_EQ(
    executeDoubleMethod1Arg("tests.data.java.LoadStoreTest", "testDLOAD", "(D)D", 42.5), 42.5);
}

TEST_F(InterpreterLoadStoreTest, DLOAD_0) {
  EXPECT_DOUBLE_EQ(
    executeDoubleMethod1Arg("tests.data.java.LoadStoreTest", "testDLOAD_0", "(D)D", 100.0), 100.0);
}

TEST_F(InterpreterLoadStoreTest, DSTORE_Basic) {
  EXPECT_DOUBLE_EQ(
    executeDoubleMethod1Arg("tests.data.java.LoadStoreTest", "testDSTORE", "(D)D", 42.5), 42.5);
}

TEST_F(InterpreterLoadStoreTest, DSTORE_0) {
  EXPECT_DOUBLE_EQ(
    executeDoubleMethod1Arg("tests.data.java.LoadStoreTest", "testDSTORE_0", "(D)D", 100.0), 100.0);
}

// ============================================================================
// Reference Load/Store Tests
// ============================================================================

TEST_F(InterpreterLoadStoreTest, ALOAD_Basic) {
  auto* klass = loader_->loadClass("tests.data.java.LoadStoreTest");
  ASSERT_NE(klass, nullptr);

  auto* method = klass->findMethod("testALOAD", "(Ljava/lang/Object;)Ljava/lang/Object;");
  ASSERT_NE(method, nullptr);

  runtime::Thread     thread;
  engine::Interpreter interpreter;

  runtime::Frame caller_frame(method);
  caller_frame.setCallerPC(method->getCode().size());
  thread.pushFrame(std::move(caller_frame));
  thread.setPC(method->getCode().size());

  runtime::Frame callee_frame(method);
  callee_frame.getLocalVariables().setRef(0, nullptr);
  thread.pushFrame(std::move(callee_frame));
  thread.setPC(0);

  interpreter.interpret(&thread);

  if (!thread.isStackEmpty()) {
    auto& current_frame = thread.getCurrentFrame();
    EXPECT_EQ(current_frame.getOperandStack().popRef(), nullptr);
  } else {
    FAIL() << "Stack is empty after execution";
  }
}

TEST_F(InterpreterLoadStoreTest, ALOAD_0) {
  auto* klass = loader_->loadClass("tests.data.java.LoadStoreTest");
  ASSERT_NE(klass, nullptr);

  auto* method = klass->findMethod("testALOAD_0", "(Ljava/lang/Object;)Ljava/lang/Object;");
  ASSERT_NE(method, nullptr);

  runtime::Thread     thread;
  engine::Interpreter interpreter;

  runtime::Frame caller_frame(method);
  caller_frame.setCallerPC(method->getCode().size());
  thread.pushFrame(std::move(caller_frame));
  thread.setPC(method->getCode().size());

  runtime::Frame callee_frame(method);
  callee_frame.getLocalVariables().setRef(0, nullptr);
  thread.pushFrame(std::move(callee_frame));
  thread.setPC(0);

  interpreter.interpret(&thread);

  if (!thread.isStackEmpty()) {
    auto& current_frame = thread.getCurrentFrame();
    EXPECT_EQ(current_frame.getOperandStack().popRef(), nullptr);
  } else {
    FAIL() << "Stack is empty after execution";
  }
}

TEST_F(InterpreterLoadStoreTest, ASTORE_Basic) {
  auto* klass = loader_->loadClass("tests.data.java.LoadStoreTest");
  ASSERT_NE(klass, nullptr);

  auto* method = klass->findMethod("testASTORE", "(Ljava/lang/Object;)Ljava/lang/Object;");
  ASSERT_NE(method, nullptr);

  runtime::Thread     thread;
  engine::Interpreter interpreter;

  runtime::Frame caller_frame(method);
  caller_frame.setCallerPC(method->getCode().size());
  thread.pushFrame(std::move(caller_frame));
  thread.setPC(method->getCode().size());

  runtime::Frame callee_frame(method);
  callee_frame.getLocalVariables().setRef(0, nullptr);
  thread.pushFrame(std::move(callee_frame));
  thread.setPC(0);

  interpreter.interpret(&thread);

  if (!thread.isStackEmpty()) {
    auto& current_frame = thread.getCurrentFrame();
    EXPECT_EQ(current_frame.getOperandStack().popRef(), nullptr);
  } else {
    FAIL() << "Stack is empty after execution";
  }
}
