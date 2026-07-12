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

// INVOKEVIRTUAL: instance method reached via dynamic dispatch on the receiver's class.
TEST_F(InterpreterObjectTest, InvokeVirtualInstanceMethod) {
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "callVirtual", 3, 4), 7);
}

// INVOKEVIRTUAL virtual dispatch: Animal a = new Dog(); a.sound() must select the
// override Dog.sound()=2 by the receiver's actual class, not Animal.sound()=1.
TEST_F(InterpreterObjectTest, InvokeVirtualDispatchesToOverride) {
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "callOverride"), 2);
}

// LDC string interning: two LDCs of the same literal yield the same reference.
// (Before interning, LDC pushed a fresh dangling c_str() each time -> would be 0.)
TEST_F(InterpreterObjectTest, LdcInternsStringLiterals) {
  EXPECT_EQ(executeStaticMethod<Jint>(kClassName, "sameStringRef"), 1);
}

}  // namespace
