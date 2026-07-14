#include <gtest/gtest.h>

#include <iostream>
#include <sstream>

#include "engine/interpreter.hpp"
#include "interpreter_test_base.hpp"
#include "oops/klass.hpp"
#include "oops/method.hpp"
#include "oops/string_pool.hpp"
#include "runtime/frame.hpp"
#include "runtime/thread.hpp"

using namespace jvm;

namespace {

class InterpreterStubInterceptTest : public InterpreterTestBase {
 public:
  static constexpr const char* kClassName = "tests.data.java.StubInterceptTest";
};

// ============================================================
//  PrintStream.print/println family — test via stdout capture
// ============================================================

TEST_F(InterpreterStubInterceptTest, PrintlnString) {
  executeStaticMethod<void>(kClassName, "printlnString");
  // No crash is the main win; output is tested below in main.
  SUCCEED();
}

TEST_F(InterpreterStubInterceptTest, PrintlnInt) {
  auto* klass  = loader_->loadClass(kClassName);
  auto* method = klass->findMethod("printlnInt", "()V");
  ASSERT_NE(method, nullptr);

  runtime::Thread     thread;
  engine::Interpreter interpreter;
  runtime::Frame      frame(method);
  thread.pushFrame(std::move(frame));

  std::ostringstream capture;
  auto*              old = std::cout.rdbuf(capture.rdbuf());
  interpreter.interpret(&thread);
  std::cout.rdbuf(old);

  EXPECT_EQ(capture.str(), "42\n");
}

TEST_F(InterpreterStubInterceptTest, PrintlnLong) {
  auto* klass  = loader_->loadClass(kClassName);
  auto* method = klass->findMethod("printlnLong", "()V");
  runtime::Thread     thread;
  engine::Interpreter interpreter;
  runtime::Frame      frame(method);
  thread.pushFrame(std::move(frame));

  std::ostringstream capture;
  auto*              old = std::cout.rdbuf(capture.rdbuf());
  interpreter.interpret(&thread);
  std::cout.rdbuf(old);

  EXPECT_EQ(capture.str(), "1234567890123\n");
}

TEST_F(InterpreterStubInterceptTest, PrintlnFloat) {
  auto* klass  = loader_->loadClass(kClassName);
  auto* method = klass->findMethod("printlnFloat", "()V");
  runtime::Thread     thread;
  engine::Interpreter interpreter;
  runtime::Frame      frame(method);
  thread.pushFrame(std::move(frame));

  std::ostringstream capture;
  auto*              old = std::cout.rdbuf(capture.rdbuf());
  interpreter.interpret(&thread);
  std::cout.rdbuf(old);

  EXPECT_EQ(capture.str(), "3.14\n");
}

TEST_F(InterpreterStubInterceptTest, PrintlnDouble) {
  auto* klass  = loader_->loadClass(kClassName);
  auto* method = klass->findMethod("printlnDouble", "()V");
  runtime::Thread     thread;
  engine::Interpreter interpreter;
  runtime::Frame      frame(method);
  thread.pushFrame(std::move(frame));

  std::ostringstream capture;
  auto*              old = std::cout.rdbuf(capture.rdbuf());
  interpreter.interpret(&thread);
  std::cout.rdbuf(old);

  EXPECT_EQ(capture.str(), "2.71828\n");
}

TEST_F(InterpreterStubInterceptTest, PrintlnBoolean) {
  auto* klass  = loader_->loadClass(kClassName);
  auto* method = klass->findMethod("printlnBoolean", "()V");
  runtime::Thread     thread;
  engine::Interpreter interpreter;
  runtime::Frame      frame(method);
  thread.pushFrame(std::move(frame));

  std::ostringstream capture;
  auto*              old = std::cout.rdbuf(capture.rdbuf());
  interpreter.interpret(&thread);
  std::cout.rdbuf(old);

  EXPECT_EQ(capture.str(), "true\nfalse\n");
}

TEST_F(InterpreterStubInterceptTest, PrintlnChar) {
  auto* klass  = loader_->loadClass(kClassName);
  auto* method = klass->findMethod("printlnChar", "()V");
  runtime::Thread     thread;
  engine::Interpreter interpreter;
  runtime::Frame      frame(method);
  thread.pushFrame(std::move(frame));

  std::ostringstream capture;
  auto*              old = std::cout.rdbuf(capture.rdbuf());
  interpreter.interpret(&thread);
  std::cout.rdbuf(old);

  EXPECT_EQ(capture.str(), "A\n");
}

TEST_F(InterpreterStubInterceptTest, PrintConcatenated) {
  auto* klass  = loader_->loadClass(kClassName);
  auto* method = klass->findMethod("printConcatenated", "()V");
  runtime::Thread     thread;
  engine::Interpreter interpreter;
  runtime::Frame      frame(method);
  thread.pushFrame(std::move(frame));

  std::ostringstream capture;
  auto*              old = std::cout.rdbuf(capture.rdbuf());
  interpreter.interpret(&thread);
  std::cout.rdbuf(old);

  EXPECT_EQ(capture.str(), "value=99");
}

TEST_F(InterpreterStubInterceptTest, PrintlnErr) {
  auto* klass  = loader_->loadClass(kClassName);
  auto* method = klass->findMethod("printlnErr", "()V");
  runtime::Thread     thread;
  engine::Interpreter interpreter;
  runtime::Frame      frame(method);
  thread.pushFrame(std::move(frame));

  std::ostringstream capture;
  auto*              old = std::cerr.rdbuf(capture.rdbuf());
  interpreter.interpret(&thread);
  std::cerr.rdbuf(old);

  EXPECT_EQ(capture.str(), "err msg\n");
}

// ============================================================
//  String methods — test via executeStaticMethod
// ============================================================

TEST_F(InterpreterStubInterceptTest, StringLength) {
  // We need to pass a string argument. Since executeStaticMethod
  // uses typed descriptor traits, we have to set up the frame manually
  // and push the interned string as argument.
  auto* klass  = loader_->loadClass(kClassName);
  auto* method = klass->findMethod("stringLength", "(Ljava/lang/String;)I");
  ASSERT_NE(method, nullptr);

  runtime::Thread     thread;
  engine::Interpreter interpreter;

  // Caller frame
  runtime::Frame caller(method);
  caller.setPC(method->getCode().size());
  thread.pushFrame(std::move(caller));

  // Callee frame with argument
  runtime::Frame callee(method);
  const auto* str = oops::StringPool::getSingleton().intern("hello");
  callee.getLocalVariables().setRef(0, const_cast<std::string*>(str));
  thread.pushFrame(std::move(callee));

  interpreter.interpret(&thread);

  Jint result = thread.getCurrentFrame().getOperandStack().popInt();
  EXPECT_EQ(result, 5);
}

TEST_F(InterpreterStubInterceptTest, StringCharAt) {
  auto* klass  = loader_->loadClass(kClassName);
  auto* method = klass->findMethod("stringCharAt", "(Ljava/lang/String;)C");

  runtime::Thread     thread;
  engine::Interpreter interpreter;

  runtime::Frame caller(method);
  caller.setPC(method->getCode().size());
  thread.pushFrame(std::move(caller));

  runtime::Frame callee(method);
  const auto* str = oops::StringPool::getSingleton().intern("hello");
  callee.getLocalVariables().setRef(0, const_cast<std::string*>(str));
  thread.pushFrame(std::move(callee));

  interpreter.interpret(&thread);

  Jint result = thread.getCurrentFrame().getOperandStack().popInt();
  EXPECT_EQ(result, static_cast<Jint>('e'));  // s.charAt(1) = 'e'
}

TEST_F(InterpreterStubInterceptTest, StringIsEmpty) {
  auto* klass  = loader_->loadClass(kClassName);
  auto* method = klass->findMethod("stringIsEmpty", "(Ljava/lang/String;)Z");

  runtime::Thread     thread;
  engine::Interpreter interpreter;

  runtime::Frame caller(method);
  caller.setPC(method->getCode().size());
  thread.pushFrame(std::move(caller));

  runtime::Frame callee(method);
  const auto* str = oops::StringPool::getSingleton().intern("");
  callee.getLocalVariables().setRef(0, const_cast<std::string*>(str));
  thread.pushFrame(std::move(callee));

  interpreter.interpret(&thread);

  Jint result = thread.getCurrentFrame().getOperandStack().popInt();
  EXPECT_EQ(result, 1);  // true
}

TEST_F(InterpreterStubInterceptTest, StringEquals) {
  auto* klass  = loader_->loadClass(kClassName);
  auto* method = klass->findMethod("stringEquals",
                                   "(Ljava/lang/String;Ljava/lang/String;)Z");

  runtime::Thread     thread;
  engine::Interpreter interpreter;

  runtime::Frame caller(method);
  caller.setPC(method->getCode().size());
  thread.pushFrame(std::move(caller));

  runtime::Frame callee(method);
  const auto* a = oops::StringPool::getSingleton().intern("hello");
  const auto* b = oops::StringPool::getSingleton().intern("hello");
  callee.getLocalVariables().setRef(0, const_cast<std::string*>(a));
  callee.getLocalVariables().setRef(1, const_cast<std::string*>(b));
  thread.pushFrame(std::move(callee));

  interpreter.interpret(&thread);

  Jint result = thread.getCurrentFrame().getOperandStack().popInt();
  EXPECT_EQ(result, 1);  // true
}

TEST_F(InterpreterStubInterceptTest, StringEqualsFalse) {
  auto* klass  = loader_->loadClass(kClassName);
  auto* method = klass->findMethod("stringEquals",
                                   "(Ljava/lang/String;Ljava/lang/String;)Z");

  runtime::Thread     thread;
  engine::Interpreter interpreter;

  runtime::Frame caller(method);
  caller.setPC(method->getCode().size());
  thread.pushFrame(std::move(caller));

  runtime::Frame callee(method);
  const auto* a = oops::StringPool::getSingleton().intern("hello");
  const auto* b = oops::StringPool::getSingleton().intern("world");
  callee.getLocalVariables().setRef(0, const_cast<std::string*>(a));
  callee.getLocalVariables().setRef(1, const_cast<std::string*>(b));
  thread.pushFrame(std::move(callee));

  interpreter.interpret(&thread);

  Jint result = thread.getCurrentFrame().getOperandStack().popInt();
  EXPECT_EQ(result, 0);  // false
}

TEST_F(InterpreterStubInterceptTest, StringHashCode) {
  auto* klass  = loader_->loadClass(kClassName);
  auto* method = klass->findMethod("stringHashCode", "(Ljava/lang/String;)I");

  runtime::Thread     thread;
  engine::Interpreter interpreter;

  runtime::Frame caller(method);
  caller.setPC(method->getCode().size());
  thread.pushFrame(std::move(caller));

  runtime::Frame callee(method);
  const auto* str = oops::StringPool::getSingleton().intern("abc");
  callee.getLocalVariables().setRef(0, const_cast<std::string*>(str));
  thread.pushFrame(std::move(callee));

  interpreter.interpret(&thread);

  Jint result = thread.getCurrentFrame().getOperandStack().popInt();
  // Java hashCode of "abc" = a*31^2 + b*31 + c = 97*961 + 98*31 + 99 = 96354
  EXPECT_EQ(result, 96354);
}

// ============================================================
//  Static utility methods
// ============================================================

TEST_F(InterpreterStubInterceptTest, SystemCurrentTimeMillis) {
  auto* klass  = loader_->loadClass(kClassName);
  // IMPORTANT: We test via execution of a method that calls
  // System.currentTimeMillis and returns. Since it returns a value
  // greater than 0, we can verify it's working.
  auto* method = klass->findMethod("systemCurrentTimeMillis", "()J");

  runtime::Thread     thread;
  engine::Interpreter interpreter;

  runtime::Frame caller(method);
  caller.setPC(method->getCode().size());
  thread.pushFrame(std::move(caller));

  runtime::Frame callee(method);
  thread.pushFrame(std::move(callee));

  interpreter.interpret(&thread);

  Jlong result = thread.getCurrentFrame().getOperandStack().popLong();
  EXPECT_GT(result, 1'600'000'000'000L);  // >= ~2020 in epoch ms
}

TEST_F(InterpreterStubInterceptTest, ParseInt) {
  auto* klass  = loader_->loadClass(kClassName);
  auto* method = klass->findMethod("parseInt", "(Ljava/lang/String;)I");

  runtime::Thread     thread;
  engine::Interpreter interpreter;

  runtime::Frame caller(method);
  caller.setPC(method->getCode().size());
  thread.pushFrame(std::move(caller));

  runtime::Frame callee(method);
  const auto* str = oops::StringPool::getSingleton().intern("42");
  callee.getLocalVariables().setRef(0, const_cast<std::string*>(str));
  thread.pushFrame(std::move(callee));

  interpreter.interpret(&thread);

  Jint result = thread.getCurrentFrame().getOperandStack().popInt();
  EXPECT_EQ(result, 42);
}

TEST_F(InterpreterStubInterceptTest, IntToString) {
  auto* klass  = loader_->loadClass(kClassName);
  auto* method = klass->findMethod("intToString", "(I)Ljava/lang/String;");

  runtime::Thread     thread;
  engine::Interpreter interpreter;

  runtime::Frame caller(method);
  caller.setPC(method->getCode().size());
  thread.pushFrame(std::move(caller));

  runtime::Frame callee(method);
  callee.getLocalVariables().setInt(0, 123);
  thread.pushFrame(std::move(callee));

  interpreter.interpret(&thread);

  auto* result = static_cast<std::string*>(thread.getCurrentFrame().getOperandStack().popRef());
  ASSERT_NE(result, nullptr);
  EXPECT_EQ(*result, "123");
}

TEST_F(InterpreterStubInterceptTest, MathAbsInt) {
  auto result = executeStaticMethod<Jint>(kClassName, "mathAbsInt");
  EXPECT_EQ(result, 7);
}

TEST_F(InterpreterStubInterceptTest, MathAbsLong) {
  auto result = executeStaticMethod<Jlong>(kClassName, "mathAbsLong");
  EXPECT_EQ(result, 1234567890123L);
}

TEST_F(InterpreterStubInterceptTest, MathMax) {
  auto result = executeStaticMethod<Jint>(kClassName, "mathMax");
  EXPECT_EQ(result, 20);
}

TEST_F(InterpreterStubInterceptTest, MathMin) {
  auto result = executeStaticMethod<Jint>(kClassName, "mathMin");
  EXPECT_EQ(result, 10);
}

TEST_F(InterpreterStubInterceptTest, MathSqrt) {
  auto result = executeStaticMethod<Jdouble>(kClassName, "mathSqrt");
  EXPECT_DOUBLE_EQ(result, 3.0);
}

}  // namespace