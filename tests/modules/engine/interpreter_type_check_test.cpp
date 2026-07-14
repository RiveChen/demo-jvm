#include <gtest/gtest.h>

#include "interpreter_test_base.hpp"
#include "oops/klass.hpp"
#include "oops/method.hpp"
#include "runtime/frame.hpp"
#include "runtime/thread.hpp"
#include "utilities/types.hpp"

using namespace jvm;

namespace {

class InterpreterTypeCheckTest : public InterpreterTestBase {
 public:
  static constexpr const char* kClassName = "tests.data.java.TypeCheckTest";
};

// ============================================================================
// INSTANCEOF Tests
// ============================================================================

TEST_F(InterpreterTypeCheckTest, InstanceOfDogIsAnimal) {
  EXPECT_EQ(executeStaticMethod<Jboolean>(kClassName, "instanceOfDogIsAnimal"), 1);
}

TEST_F(InterpreterTypeCheckTest, InstanceOfDogIsDog) {
  EXPECT_EQ(executeStaticMethod<Jboolean>(kClassName, "instanceOfDogIsDog"), 1);
}

TEST_F(InterpreterTypeCheckTest, InstanceOfAnimalIsNotDog) {
  EXPECT_EQ(executeStaticMethod<Jboolean>(kClassName, "instanceOfAnimalIsNotDog"), 0);
}

TEST_F(InterpreterTypeCheckTest, InstanceOfNull) {
  EXPECT_EQ(executeStaticMethod<Jboolean>(kClassName, "instanceOfNull"), 0);
}

// ============================================================================
// CHECKCAST Tests
// ============================================================================

TEST_F(InterpreterTypeCheckTest, CheckcastSuccess) {
  auto* klass  = loader_->loadClass(kClassName);
  auto* method = klass->findMethod("checkcastAnimalToDog", "()Ltests/data/java/Dog;");
  ASSERT_NE(method, nullptr);

  runtime::Thread     thread;
  engine::Interpreter interpreter;

  runtime::Frame caller(method);
  caller.setPC(method->getCode().size());
  thread.pushFrame(std::move(caller));

  runtime::Frame callee(method);
  thread.pushFrame(std::move(callee));

  interpreter.interpret(&thread);

  auto* result = thread.getCurrentFrame().getOperandStack().popRef();
  EXPECT_NE(result, nullptr);
}

TEST_F(InterpreterTypeCheckTest, CheckcastNull) {
  auto* klass  = loader_->loadClass(kClassName);
  auto* method = klass->findMethod("checkcastNull", "()Ltests/data/java/Dog;");
  ASSERT_NE(method, nullptr);

  runtime::Thread     thread;
  engine::Interpreter interpreter;

  runtime::Frame caller(method);
  caller.setPC(method->getCode().size());
  thread.pushFrame(std::move(caller));

  runtime::Frame callee(method);
  thread.pushFrame(std::move(callee));

  interpreter.interpret(&thread);

  auto* result = thread.getCurrentFrame().getOperandStack().popRef();
  EXPECT_EQ(result, nullptr);
}

TEST_F(InterpreterTypeCheckTest, CheckcastFail) {
  auto* klass  = loader_->loadClass(kClassName);
  auto* method = klass->findMethod("checkcastFail", "()Ltests/data/java/Dog;");
  ASSERT_NE(method, nullptr);

  runtime::Thread     thread;
  engine::Interpreter interpreter;

  runtime::Frame caller(method);
  caller.setPC(method->getCode().size());
  thread.pushFrame(std::move(caller));

  runtime::Frame callee(method);
  thread.pushFrame(std::move(callee));

  EXPECT_THROW(interpreter.interpret(&thread), std::runtime_error);
}

}  // namespace