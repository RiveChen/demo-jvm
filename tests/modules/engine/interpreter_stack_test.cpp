#include <gtest/gtest.h>

#include "interpreter_test_base.h"

// ============================================================================
// Stack Operation Tests
// ============================================================================

class InterpreterStackTest : public InterpreterTestBase {};

TEST_F(InterpreterStackTest, POP_Basic) {
  EXPECT_EQ(executeStaticMethod<Jint>("tests.data.java.StackTest", "testPOP", 10, 20), 30);
}

TEST_F(InterpreterStackTest, POP2_Basic) {
  EXPECT_EQ(executeStaticMethod<Jlong>("tests.data.java.StackTest", "testPOP2", 10LL, 20LL), 30LL);
}

TEST_F(InterpreterStackTest, DUP_Basic) {
  EXPECT_EQ(executeStaticMethod<Jint>("tests.data.java.StackTest", "testDUP", 5), 10);
}

TEST_F(InterpreterStackTest, DUP_X1_Basic) {
  EXPECT_EQ(executeStaticMethod<Jint>("tests.data.java.StackTest", "testDUP_X1", 5, 10), 20);
}

TEST_F(InterpreterStackTest, DUP_X2_Basic) {
  EXPECT_EQ(executeStaticMethod<Jint>("tests.data.java.StackTest", "testDUP_X2", 5, 10, 15),
            35);  // 5 + 10 + 15 + 5 = 35
}

TEST_F(InterpreterStackTest, DUP2_Basic) {
  EXPECT_EQ(executeStaticMethod<Jlong>("tests.data.java.StackTest", "testDUP2", 5LL), 10LL);
}

TEST_F(InterpreterStackTest, SWAP_Basic) {
  EXPECT_EQ(executeStaticMethod<Jint>("tests.data.java.StackTest", "testSWAP", 10, 20), 30);
}
