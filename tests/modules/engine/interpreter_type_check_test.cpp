#include "interpreter_test_base.hpp"
#include "oops/klass.hpp"
#include "oops/method.hpp"
#include "runtime/frame.hpp"
#include "runtime/thread.hpp"
#include "utilities/types.hpp"

#include <gtest/gtest.h>

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
  Jref result =
    executeStaticMethod<Jref>(kClassName, "checkcastAnimalToDog", "()Ltests/data/java/Dog;");
  EXPECT_NE(result, nullptr);
}

TEST_F(InterpreterTypeCheckTest, CheckcastNull) {
  Jref result = executeStaticMethod<Jref>(kClassName, "checkcastNull", "()Ltests/data/java/Dog;");
  EXPECT_EQ(result, nullptr);
}

TEST_F(InterpreterTypeCheckTest, CheckcastFail) {
  EXPECT_THROW(executeStaticMethod<Jref>(kClassName, "checkcastFail", "()Ltests/data/java/Dog;"),
               std::runtime_error);
}

}  // namespace