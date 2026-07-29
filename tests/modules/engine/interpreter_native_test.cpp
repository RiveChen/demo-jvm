#include "engine/native_registry.hpp"
#include "interpreter_test_base.hpp"
#include "runtime/operand_stack.hpp"
#include "utilities/types.hpp"

#include <gtest/gtest.h>

using namespace jvm;

namespace {

// Native impl bound for the test: pop the int arg, push arg + 100.
// Proves the native ran, popped its argument, and pushed a return value.
void nativeEchoPlus100(runtime::OperandStack& s) {
  Jint x = s.popInt();
  s.pushInt(x + 100);
}

class InterpreterNativeTest : public InterpreterTestBase {
 public:
  static constexpr const char* kClassName = "tests.data.java.NativeTest";

  void SetUp() override {
    InterpreterTestBase::SetUp();
    // key = ownerClassName(slash form) + "." + method + "." + descriptor
    engine::NativeRegistry::getSingleton().bind("tests/data/java/NativeTest.echo.(I)I",
                                                &nativeEchoPlus100);
  }
};

}  // namespace

// INVOKESTATIC of an ACC_NATIVE method must dispatch to the registered C++ fn
// (callEcho(x) -> invokestatic echo -> native -> x + 100).
TEST_F(InterpreterNativeTest, InvokeStaticNativeDispatches) {
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "callEcho", 42), 142);
}
