#include <gtest/gtest.h>

#include <iostream>
#include <sstream>

#include "engine/interpreter.hpp"
#include "interpreter_test_base.hpp"
#include "oops/klass.hpp"
#include "oops/method.hpp"
#include "runtime/frame.hpp"
#include "runtime/thread.hpp"

using namespace jvm;

namespace {

class InterpreterHelloWorldTest : public InterpreterTestBase {
 public:
  static constexpr const char* kClassName = "tests.data.java.HelloWorld";
};

}  // namespace

// End-to-end stubbed hello world: drive the real main([Ljava/lang/String;)V
// (like main.cpp) and capture stdout.
//   getstatic System.out  (intercept -> sentinel)
//   ldc "Hello, World!"    (M2 intern -> stable std::string*)
//   invokevirtual PrintStream.println (intercept -> std::cout << str << '\n')
TEST_F(InterpreterHelloWorldTest, MainPrintsHelloWorld) {
  auto* klass = loader_->loadClass(kClassName);
  ASSERT_NE(klass, nullptr);
  auto* method = klass->findMethod("main", "([Ljava/lang/String;)V");
  ASSERT_NE(method, nullptr);

  jvm::runtime::Thread     thread;
  jvm::engine::Interpreter interpreter;
  jvm::runtime::Frame      frame(method);
  frame.getLocalVariables().setRef(0, nullptr);  // args = null (unused)
  thread.pushFrame(std::move(frame));

  std::ostringstream capture;
  std::streambuf*    old = std::cout.rdbuf(capture.rdbuf());
  interpreter.interpret(&thread);
  std::cout.rdbuf(old);

  EXPECT_EQ(capture.str(), "Hello, World!\n");
}
