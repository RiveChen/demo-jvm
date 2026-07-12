#include <gtest/gtest.h>

#include "interpreter_test_base.h"
#include "utilities/types.h"

using namespace jvm;

namespace {

class InterpreterObjectTest : public InterpreterTestBase {
 public:
  static constexpr const char* kClassName = "tests.data.java.ObjectTest";
};

// End-to-end: NEW + INVOKESPECIAL(<init>, Object.<init> no-op) + PUTFIELD + GETFIELD.

TEST_F(InterpreterObjectTest, NewPutGetIntField) {
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testIntField", 42), 42);
}

TEST_F(InterpreterObjectTest, NewPutGetLongField) {
  const Jlong v = 0x1122334455667788LL;  // 64-bit value through a category-2 field
  EXPECT_EQ(executeStaticMethod<Jlong>(kClassName, "testLongField", v), v);
}

TEST_F(InterpreterObjectTest, NewPutGetDoubleField) {
  EXPECT_DOUBLE_EQ(executeStaticMethod<Jdouble>(kClassName, "testDoubleField", Jdouble{2.5}), 2.5);
}

TEST_F(InterpreterObjectTest, TwoInstancesIndependent) {
  // if the two objects aliased, this would be 3*1000+3 = 3003
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "testTwoObjects", 7, 3), 7003);
}

}  // namespace
