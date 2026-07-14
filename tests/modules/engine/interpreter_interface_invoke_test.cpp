#include <gtest/gtest.h>

#include "interpreter_test_base.hpp"
#include "utilities/types.hpp"

using namespace jvm;

namespace {

class InterpreterInterfaceInvokeTest : public InterpreterTestBase {
 public:
  static constexpr const char* kClassName = "tests.data.java.InterfaceInvokeTest";
};

// ============================================================================
// INVOKEINTERFACE Tests
// ============================================================================

TEST_F(InterpreterInterfaceInvokeTest, InvokeInterfaceGreetHello) {
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "invokeInterfaceGreet"), 42);
}

TEST_F(InterpreterInterfaceInvokeTest, InvokeInterfaceGreetHi) {
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "invokeInterfaceGreetHi"), 99);
}

}  // namespace