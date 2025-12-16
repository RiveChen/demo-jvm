#include <gtest/gtest.h>

#include "interpreter_test_base.h"

// ============================================================================
// Integer Load Tests
// ============================================================================

class InterpreterLoadStoreTest : public InterpreterTestBase {
 public:
  static constexpr const char* kClassName = "tests.data.java.LoadStoreTest";
};

TEST_F(InterpreterLoadStoreTest, ILOAD_Basic) {
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testILOAD", 42), 42);
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testILOAD", -42), -42);
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testILOAD", 0), 0);
}

TEST_F(InterpreterLoadStoreTest, ILOAD_0) {
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testILOAD_0", 100), 100);
}

TEST_F(InterpreterLoadStoreTest, ILOAD_1) {
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testILOAD_1", 10, 20), 20);
}

TEST_F(InterpreterLoadStoreTest, ILOAD_2) {
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testILOAD_2", 10, 20, 30), 30);
}

TEST_F(InterpreterLoadStoreTest, ILOAD_3) {
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testILOAD_3", 10, 20, 30, 40), 40);
}

// ============================================================================
// Integer Store Tests
// ============================================================================

TEST_F(InterpreterLoadStoreTest, ISTORE_Basic) {
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testISTORE", 42), 42);
}

TEST_F(InterpreterLoadStoreTest, ISTORE_0) {
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testISTORE_0", 100), 100);
}

TEST_F(InterpreterLoadStoreTest, ISTORE_1) {
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testISTORE_1", 10, 20), 20);
}

// ============================================================================
// Long Load/Store Tests
// ============================================================================

TEST_F(InterpreterLoadStoreTest, LLOAD_Basic) {
  EXPECT_EQ(executeStaticMethod<Jlong>(kClassName, "testLLOAD", 42LL), 42LL);
}

TEST_F(InterpreterLoadStoreTest, LLOAD_0) {
  EXPECT_EQ(executeStaticMethod<Jlong>(kClassName, "testLLOAD_0", 100LL), 100LL);
}

TEST_F(InterpreterLoadStoreTest, LSTORE_Basic) {
  EXPECT_EQ(executeStaticMethod<Jlong>(kClassName, "testLSTORE", 42LL), 42LL);
}

TEST_F(InterpreterLoadStoreTest, LSTORE_0) {
  EXPECT_EQ(executeStaticMethod<Jlong>(kClassName, "testLSTORE_0", 100LL), 100LL);
}

// ============================================================================
// Float Load/Store Tests
// ============================================================================

TEST_F(InterpreterLoadStoreTest, FLOAD_Basic) {
  EXPECT_FLOAT_EQ(executeStaticMethod<Jfloat>(kClassName, "testFLOAD", 42.5F), 42.5F);
}

TEST_F(InterpreterLoadStoreTest, FLOAD_0) {
  EXPECT_FLOAT_EQ(executeStaticMethod<Jfloat>(kClassName, "testFLOAD_0", 100.0F), 100.0F);
}

TEST_F(InterpreterLoadStoreTest, FSTORE_Basic) {
  EXPECT_FLOAT_EQ(executeStaticMethod<Jfloat>(kClassName, "testFSTORE", 42.5F), 42.5F);
}

TEST_F(InterpreterLoadStoreTest, FSTORE_0) {
  EXPECT_FLOAT_EQ(executeStaticMethod<Jfloat>(kClassName, "testFSTORE_0", 100.0F), 100.0F);
}

// ============================================================================
// Double Load/Store Tests
// ============================================================================

TEST_F(InterpreterLoadStoreTest, DLOAD_Basic) {
  EXPECT_DOUBLE_EQ(executeStaticMethod<Jdouble>(kClassName, "testDLOAD", 42.5), 42.5);
}

TEST_F(InterpreterLoadStoreTest, DLOAD_0) {
  EXPECT_DOUBLE_EQ(executeStaticMethod<Jdouble>(kClassName, "testDLOAD_0", 100.0), 100.0);
}

TEST_F(InterpreterLoadStoreTest, DSTORE_Basic) {
  EXPECT_DOUBLE_EQ(executeStaticMethod<Jdouble>(kClassName, "testDSTORE", 42.5), 42.5);
}

TEST_F(InterpreterLoadStoreTest, DSTORE_0) {
  EXPECT_DOUBLE_EQ(executeStaticMethod<Jdouble>(kClassName, "testDSTORE_0", 100.0), 100.0);
}

// ============================================================================
// Reference Load/Store Tests
// ============================================================================

TEST_F(InterpreterLoadStoreTest, ALOAD_Basic) {
  auto* klass = loader_->loadClass(kClassName);
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
  auto* klass = loader_->loadClass(kClassName);
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
  auto* klass = loader_->loadClass(kClassName);
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
