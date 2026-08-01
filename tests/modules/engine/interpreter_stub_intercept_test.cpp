#include "engine/interpreter.hpp"
#include "interpreter_test_base.hpp"
#include "oops/klass.hpp"
#include "oops/method.hpp"
#include "oops/string_pool.hpp"
#include "runtime/frame.hpp"
#include "runtime/thread.hpp"

#include <gtest/gtest.h>
#include <iostream>
#include <sstream>

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
  std::ostringstream capture;
  auto*              old = std::cout.rdbuf(capture.rdbuf());
  executeStaticMethod<void>(kClassName, "printlnInt");
  std::cout.rdbuf(old);

  EXPECT_EQ(capture.str(), "42\n");
}

TEST_F(InterpreterStubInterceptTest, PrintlnLong) {
  std::ostringstream capture;
  auto*              old = std::cout.rdbuf(capture.rdbuf());
  executeStaticMethod<void>(kClassName, "printlnLong");
  std::cout.rdbuf(old);

  EXPECT_EQ(capture.str(), "1234567890123\n");
}

TEST_F(InterpreterStubInterceptTest, PrintlnFloat) {
  std::ostringstream capture;
  auto*              old = std::cout.rdbuf(capture.rdbuf());
  executeStaticMethod<void>(kClassName, "printlnFloat");
  std::cout.rdbuf(old);

  EXPECT_EQ(capture.str(), "3.14\n");
}

TEST_F(InterpreterStubInterceptTest, PrintlnDouble) {
  std::ostringstream capture;
  auto*              old = std::cout.rdbuf(capture.rdbuf());
  executeStaticMethod<void>(kClassName, "printlnDouble");
  std::cout.rdbuf(old);

  EXPECT_EQ(capture.str(), "2.71828\n");
}

TEST_F(InterpreterStubInterceptTest, PrintlnBoolean) {
  std::ostringstream capture;
  auto*              old = std::cout.rdbuf(capture.rdbuf());
  executeStaticMethod<void>(kClassName, "printlnBoolean");
  std::cout.rdbuf(old);

  EXPECT_EQ(capture.str(), "true\nfalse\n");
}

TEST_F(InterpreterStubInterceptTest, PrintlnChar) {
  std::ostringstream capture;
  auto*              old = std::cout.rdbuf(capture.rdbuf());
  executeStaticMethod<void>(kClassName, "printlnChar");
  std::cout.rdbuf(old);

  EXPECT_EQ(capture.str(), "A\n");
}

TEST_F(InterpreterStubInterceptTest, PrintConcatenated) {
  std::ostringstream capture;
  auto*              old = std::cout.rdbuf(capture.rdbuf());
  executeStaticMethod<void>(kClassName, "printConcatenated");
  std::cout.rdbuf(old);

  EXPECT_EQ(capture.str(), "value=99");
}

TEST_F(InterpreterStubInterceptTest, PrintlnErr) {
  // Pre-load the class so the loader's INFO log line (which goes to stderr)
  // is emitted before we redirect stderr for the duration of the call.
  (void)loader_->loadClass(kClassName);

  std::ostringstream capture;
  auto*              old = std::cerr.rdbuf(capture.rdbuf());
  executeStaticMethod<void>(kClassName, "printlnErr");
  std::cerr.rdbuf(old);

  EXPECT_EQ(capture.str(), "err msg\n");
}

// ============================================================
//  String methods — test via executeStaticMethod with exact
//  descriptors (Jref is opaque and cannot encode Ljava/lang/String;).
// ============================================================

namespace {
Jref internString(const char* s) {
  const auto* str = oops::StringPool::getSingleton().intern(s);
  return const_cast<std::string*>(str);
}
}  // namespace

TEST_F(InterpreterStubInterceptTest, StringLength) {
  Jint result = executeStaticMethod<Jint>(kClassName, "stringLength", "(Ljava/lang/String;)I",
                                          internString("hello"));
  EXPECT_EQ(result, 5);
}

TEST_F(InterpreterStubInterceptTest, StringCharAt) {
  Jint result = executeStaticMethod<Jchar>(kClassName, "stringCharAt", "(Ljava/lang/String;)C",
                                           internString("hello"));
  EXPECT_EQ(result, static_cast<Jint>('e'));  // s.charAt(1) = 'e'
}

TEST_F(InterpreterStubInterceptTest, StringIsEmpty) {
  Jint result = executeStaticMethod<Jint>(kClassName, "stringIsEmpty", "(Ljava/lang/String;)Z",
                                          internString(""));
  EXPECT_EQ(result, 1);  // true
}

TEST_F(InterpreterStubInterceptTest, StringEquals) {
  Jint result =
    executeStaticMethod<Jint>(kClassName, "stringEquals", "(Ljava/lang/String;Ljava/lang/String;)Z",
                              internString("hello"), internString("hello"));
  EXPECT_EQ(result, 1);  // true
}

TEST_F(InterpreterStubInterceptTest, StringEqualsFalse) {
  Jint result =
    executeStaticMethod<Jint>(kClassName, "stringEquals", "(Ljava/lang/String;Ljava/lang/String;)Z",
                              internString("hello"), internString("world"));
  EXPECT_EQ(result, 0);  // false
}

TEST_F(InterpreterStubInterceptTest, StringHashCode) {
  Jint result = executeStaticMethod<Jint>(kClassName, "stringHashCode", "(Ljava/lang/String;)I",
                                          internString("abc"));
  // Java hashCode of "abc" = a*31^2 + b*31 + c = 97*961 + 98*31 + 99 = 96354
  EXPECT_EQ(result, 96354);
}

// ============================================================
//  Static utility methods
// ============================================================

TEST_F(InterpreterStubInterceptTest, SystemCurrentTimeMillis) {
  // Executes a method that calls System.currentTimeMillis and returns it.
  // Since it returns a value greater than 0, we can verify it's working.
  Jlong result = executeStaticMethod<Jlong>(kClassName, "systemCurrentTimeMillis", "()J");
  EXPECT_GT(result, 1'600'000'000'000L);  // >= ~2020 in epoch ms
}

TEST_F(InterpreterStubInterceptTest, ParseInt) {
  Jint result =
    executeStaticMethod<Jint>(kClassName, "parseInt", "(Ljava/lang/String;)I", internString("42"));
  EXPECT_EQ(result, 42);
}

TEST_F(InterpreterStubInterceptTest, IntToString) {
  Jref result = executeStaticMethod<Jref>(kClassName, "intToString", "(I)Ljava/lang/String;", 123);
  ASSERT_NE(result, nullptr);
  auto* str = static_cast<std::string*>(result);
  EXPECT_EQ(*str, "123");
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