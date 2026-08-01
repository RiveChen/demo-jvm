#include "engine/interpreter.hpp"
#include "interpreter_test_base.hpp"
#include "oops/method.hpp"

#include <gtest/gtest.h>
#include <stdexcept>
#include <vector>

// ============================================================================
// Fixed run-protocol tests: a single entry frame returns to the host through
// RunOutcome, and the Java frame stack is empty on completion.
//
// These tests pin the contract:
//   push entry frame -> run -> RunOutcome{Completed(ReturnValue)}
// without any fake caller frame, setPC(code.size()), or residual operand
// stack reads.
// ============================================================================

class InterpreterEntryReturnTest : public InterpreterTestBase {
 public:
  static constexpr const char* kArithmetic = "tests.data.java.ArithmeticTest";
  static constexpr const char* kLoadStore  = "tests.data.java.LoadStoreTest";
  static constexpr const char* kMethodInv  = "tests.data.java.MethodInvocationTest";
  static constexpr const char* kStub       = "tests.data.java.StubInterceptTest";
  static constexpr const char* kOpcodeFail = "tests.data.java.OpcodeFailureTest";
};

// 1. Entry IRETURN returns Int to the host.
TEST_F(InterpreterEntryReturnTest, IntReturnCompletesWithValue) {
  auto* klass  = loader_->loadClass(kArithmetic);
  auto* method = klass->findMethod("testIADD", "(II)I");
  ASSERT_NE(method, nullptr);

  engine::RunOutcome outcome =
    runEntryMethodForTest(method, {engine::VmValue{Jint{10}}, engine::VmValue{Jint{20}}});

  ASSERT_TRUE(outcome.isCompleted());
  ASSERT_FALSE(outcome.isVoid());
  const auto& result = std::get<engine::Completed<engine::ReturnValue>>(outcome.value).result;
  EXPECT_EQ(result.kind, engine::ReturnValue::Int);
  EXPECT_EQ(result.i, 30);
}

// 2. Entry LRETURN returns Long to the host.
TEST_F(InterpreterEntryReturnTest, LongReturnCompletesWithValue) {
  auto* klass  = loader_->loadClass(kArithmetic);
  auto* method = klass->findMethod("testLADD", "(JJ)J");
  ASSERT_NE(method, nullptr);

  engine::RunOutcome outcome =
    runEntryMethodForTest(method, {engine::VmValue{Jlong{5}}, engine::VmValue{Jlong{7}}});

  ASSERT_TRUE(outcome.isCompleted());
  ASSERT_FALSE(outcome.isVoid());
  const auto& result = std::get<engine::Completed<engine::ReturnValue>>(outcome.value).result;
  EXPECT_EQ(result.kind, engine::ReturnValue::Long);
  EXPECT_EQ(result.l, 12);
}

// 3. Entry ARETURN returns Reference to the host.
TEST_F(InterpreterEntryReturnTest, RefReturnCompletesWithValue) {
  auto* klass  = loader_->loadClass(kLoadStore);
  auto* method = klass->findMethod("testALOAD", "(Ljava/lang/Object;)Ljava/lang/Object;");
  ASSERT_NE(method, nullptr);

  engine::RunOutcome outcome =
    runEntryMethodForTest(method, {engine::VmValue{static_cast<Jref>(nullptr)}});

  ASSERT_TRUE(outcome.isCompleted());
  ASSERT_FALSE(outcome.isVoid());
  const auto& result = std::get<engine::Completed<engine::ReturnValue>>(outcome.value).result;
  EXPECT_EQ(result.kind, engine::ReturnValue::Reference);
  EXPECT_EQ(result.r, nullptr);
}

// 4. Entry RETURN (void) completes with a Void return value.
TEST_F(InterpreterEntryReturnTest, VoidReturnCompletes) {
  auto* klass  = loader_->loadClass(kStub);
  auto* method = klass->findMethod("printlnInt", "()V");
  ASSERT_NE(method, nullptr);

  engine::RunOutcome outcome = runEntryMethodForTest(method);

  ASSERT_TRUE(outcome.isCompleted());
  EXPECT_TRUE(outcome.isVoid());
  EXPECT_TRUE(std::holds_alternative<engine::CompletedVoid>(outcome.value));
}

// 5. An entry method that falls off the end of its code is a VM error
//    (fail-fast), not a silent void return.
TEST_F(InterpreterEntryReturnTest, FallingOffCodeEndFailsFast) {
  auto* klass  = loader_->loadClass(kOpcodeFail);
  auto* method = klass->findMethod("testOpcode", "()V");
  ASSERT_NE(method, nullptr);

  // Patch the method so its bytecode is empty: the interpreter must not
  // invent a return; it must fail fast on pc >= code.size().
  auto& mutable_code = const_cast<std::vector<U1>&>(method->getCode());
  mutable_code.clear();

  EXPECT_THROW(runEntryMethodForTest(method), std::runtime_error);
}

// ============================================================================
// Stress the host-facing protocol through the full helper path as well, and
// confirm RunOutcome (not residual frames) is the only source of the value.
// ============================================================================

TEST_F(InterpreterEntryReturnTest, NestedCallsAlsoReturnViaRunOutcome) {
  // testInvokeStaticFactorial (I)I -> INVOKESTATIC recursion terminates with
  // IRETURN; the value must arrive via RunOutcome, not a leftover caller frame.
  auto* klass  = loader_->loadClass(kMethodInv);
  auto* method = klass->findMethod("testInvokeStaticFactorial", "(I)I");
  ASSERT_NE(method, nullptr);

  engine::RunOutcome outcome = runEntryMethodForTest(method, {engine::VmValue{Jint{5}}});

  ASSERT_TRUE(outcome.isCompleted());
  ASSERT_FALSE(outcome.isVoid());
  const auto& result = std::get<engine::Completed<engine::ReturnValue>>(outcome.value).result;
  EXPECT_EQ(result.kind, engine::ReturnValue::Int);
  EXPECT_EQ(result.i, 120);
}
