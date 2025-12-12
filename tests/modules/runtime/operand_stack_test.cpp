#include "runtime/operand_stack.h"

#include <gtest/gtest.h>

namespace jvm::runtime {

TEST(OperandStackTest, PushAndPop) {
  OperandStack stack;
  stack.pushInt(1);
  EXPECT_EQ(stack.popInt(), 1);
}

TEST(OperandStackTest, EmptyStack) {
  OperandStack stack;
  EXPECT_THROW(stack.popInt(), std::runtime_error);
}

}  // namespace jvm::runtime