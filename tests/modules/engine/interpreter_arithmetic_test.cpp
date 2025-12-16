#include <gtest/gtest.h>

#include <memory>
#include <string>
#include <vector>

#include "class_loader/class_loader.h"
#include "engine/interpreter.h"
#include "runtime/frame.h"
#include "runtime/klass.h"
#include "runtime/method.h"
#include "runtime/method_area.h"
#include "runtime/thread.h"

using namespace jvm;

namespace {

class InterpreterArithmeticTest : public ::testing::Test {
 protected:
  void SetUp() override {
    test_classpath_ = TEST_CLASS_PATH;
    classpath_list_ = {test_classpath_};
    loader_         = std::make_unique<class_loader::ClassLoader>(nullptr, classpath_list_);
    runtime::MethodArea::getInstance().reset();
  }

  void TearDown() override { loader_.reset(); }

  // Helper to execute a static method and get int result
  Jint executeIntMethod(const std::string& method_name, Jint arg1, Jint arg2) {
    auto* klass = loader_->loadClass("tests.data.java.ArithmeticTest");
    if (klass == nullptr) {
      throw std::runtime_error("Failed to load ArithmeticTest class");
    }

    auto* method = klass->findMethod(method_name, "(II)I");
    if (method == nullptr) {
      throw std::runtime_error("Method " + method_name + " not found");
    }

    runtime::Thread     thread;
    engine::Interpreter interpreter;

    // Create a caller frame to receive the return value
    // IRETURN will pop the callee frame and push the return value to caller's stack
    runtime::Frame caller_frame(method);
    caller_frame.setCallerPC(method->getCode().size());
    thread.pushFrame(std::move(caller_frame));
    // Set PC to end of code so interpreter will immediately pop this frame
    thread.setPC(method->getCode().size());

    // Create callee frame (the method we're testing)
    runtime::Frame callee_frame(method);

    // Set up arguments in local variables (static method: args at index 0, 1)
    callee_frame.getLocalVariables().setInt(0, arg1);
    callee_frame.getLocalVariables().setInt(1, arg2);

    // Push callee frame to thread
    thread.pushFrame(std::move(callee_frame));
    // Reset PC to 0 for callee frame
    thread.setPC(0);

    // Execute interpreter
    // When callee returns, it will push return value to caller's stack
    // Interpreter will then pop caller frame (PC beyond code) and stop
    interpreter.interpret(&thread);

    // Get result from operand stack
    if (!thread.isStackEmpty()) {
      auto& current_frame = thread.getCurrentFrame();
      return current_frame.getOperandStack().popInt();
    }
    throw std::runtime_error("Stack is empty after execution");
  }

  // Helper to execute a static method with one int argument
  Jint executeIntMethod1Arg(const std::string& method_name, Jint arg) {
    auto* klass = loader_->loadClass("tests.data.java.ArithmeticTest");
    if (klass == nullptr) {
      throw std::runtime_error("Failed to load ArithmeticTest class");
    }

    auto* method = klass->findMethod(method_name, "(I)I");
    if (method == nullptr) {
      throw std::runtime_error("Method " + method_name + " not found");
    }

    runtime::Thread     thread;
    engine::Interpreter interpreter;

    // Create caller frame to receive return value
    runtime::Frame caller_frame(method);
    caller_frame.setCallerPC(method->getCode().size());
    thread.pushFrame(std::move(caller_frame));
    thread.setPC(method->getCode().size());

    // Create callee frame
    runtime::Frame callee_frame(method);
    callee_frame.getLocalVariables().setInt(0, arg);
    thread.pushFrame(std::move(callee_frame));
    thread.setPC(0);

    interpreter.interpret(&thread);

    if (!thread.isStackEmpty()) {
      auto& current_frame = thread.getCurrentFrame();
      return current_frame.getOperandStack().popInt();
    }
    throw std::runtime_error("Stack is empty after execution");
  }

  // Helper to execute a static method and get long result
  Jlong executeLongMethod(const std::string& method_name, Jlong arg1, Jlong arg2) {
    auto* klass = loader_->loadClass("tests.data.java.ArithmeticTest");
    if (klass == nullptr) {
      throw std::runtime_error("Failed to load ArithmeticTest class");
    }

    auto* method = klass->findMethod(method_name, "(JJ)J");
    if (method == nullptr) {
      throw std::runtime_error("Method " + method_name + " not found");
    }

    runtime::Thread     thread;
    engine::Interpreter interpreter;

    // Create caller frame to receive return value
    runtime::Frame caller_frame(method);
    caller_frame.setCallerPC(method->getCode().size());
    thread.pushFrame(std::move(caller_frame));
    thread.setPC(method->getCode().size());

    // Create callee frame
    runtime::Frame callee_frame(method);
    callee_frame.getLocalVariables().setLong(0, arg1);
    callee_frame.getLocalVariables().setLong(2, arg2);  // long takes 2 slots
    thread.pushFrame(std::move(callee_frame));
    thread.setPC(0);

    interpreter.interpret(&thread);

    if (!thread.isStackEmpty()) {
      auto& current_frame = thread.getCurrentFrame();
      return current_frame.getOperandStack().popLong();
    }
    throw std::runtime_error("Stack is empty after execution");
  }

  // Helper to execute a static method with one long argument
  Jlong executeLongMethod1Arg(const std::string& method_name, Jlong arg) {
    auto* klass = loader_->loadClass("tests.data.java.ArithmeticTest");
    if (klass == nullptr) {
      throw std::runtime_error("Failed to load ArithmeticTest class");
    }

    auto* method = klass->findMethod(method_name, "(J)J");
    if (method == nullptr) {
      throw std::runtime_error("Method " + method_name + " not found");
    }

    runtime::Thread     thread;
    engine::Interpreter interpreter;

    // Create caller frame to receive return value
    runtime::Frame caller_frame(method);
    caller_frame.setCallerPC(method->getCode().size());
    thread.pushFrame(std::move(caller_frame));
    thread.setPC(method->getCode().size());

    // Create callee frame
    runtime::Frame callee_frame(method);
    callee_frame.getLocalVariables().setLong(0, arg);
    thread.pushFrame(std::move(callee_frame));
    thread.setPC(0);

    interpreter.interpret(&thread);

    if (!thread.isStackEmpty()) {
      auto& current_frame = thread.getCurrentFrame();
      return current_frame.getOperandStack().popLong();
    }
    throw std::runtime_error("Stack is empty after execution");
  }

  // Helper to execute a static method and get float result
  Jfloat executeFloatMethod(const std::string& method_name, Jfloat arg1, Jfloat arg2) {
    auto* klass = loader_->loadClass("tests.data.java.ArithmeticTest");
    if (klass == nullptr) {
      throw std::runtime_error("Failed to load ArithmeticTest class");
    }

    auto* method = klass->findMethod(method_name, "(FF)F");
    if (method == nullptr) {
      throw std::runtime_error("Method " + method_name + " not found");
    }

    runtime::Thread     thread;
    engine::Interpreter interpreter;

    // Create caller frame to receive return value
    runtime::Frame caller_frame(method);
    caller_frame.setCallerPC(method->getCode().size());
    thread.pushFrame(std::move(caller_frame));
    thread.setPC(method->getCode().size());

    // Create callee frame
    runtime::Frame callee_frame(method);
    callee_frame.getLocalVariables().setFloat(0, arg1);
    callee_frame.getLocalVariables().setFloat(1, arg2);
    thread.pushFrame(std::move(callee_frame));
    thread.setPC(0);

    interpreter.interpret(&thread);

    if (!thread.isStackEmpty()) {
      auto& current_frame = thread.getCurrentFrame();
      return current_frame.getOperandStack().popFloat();
    }
    throw std::runtime_error("Stack is empty after execution");
  }

  // Helper to execute a static method with one float argument
  Jfloat executeFloatMethod1Arg(const std::string& method_name, Jfloat arg) {
    auto* klass = loader_->loadClass("tests.data.java.ArithmeticTest");
    if (klass == nullptr) {
      throw std::runtime_error("Failed to load ArithmeticTest class");
    }

    auto* method = klass->findMethod(method_name, "(F)F");
    if (method == nullptr) {
      throw std::runtime_error("Method " + method_name + " not found");
    }

    runtime::Thread     thread;
    engine::Interpreter interpreter;

    // Create caller frame to receive return value
    runtime::Frame caller_frame(method);
    caller_frame.setCallerPC(method->getCode().size());
    thread.pushFrame(std::move(caller_frame));
    thread.setPC(method->getCode().size());

    // Create callee frame
    runtime::Frame callee_frame(method);
    callee_frame.getLocalVariables().setFloat(0, arg);
    thread.pushFrame(std::move(callee_frame));
    thread.setPC(0);

    interpreter.interpret(&thread);

    if (!thread.isStackEmpty()) {
      auto& current_frame = thread.getCurrentFrame();
      return current_frame.getOperandStack().popFloat();
    }
    throw std::runtime_error("Stack is empty after execution");
  }

  // Helper to execute a static method and get double result
  Jdouble executeDoubleMethod(const std::string& method_name, Jdouble arg1, Jdouble arg2) {
    auto* klass = loader_->loadClass("tests.data.java.ArithmeticTest");
    if (klass == nullptr) {
      throw std::runtime_error("Failed to load ArithmeticTest class");
    }

    auto* method = klass->findMethod(method_name, "(DD)D");
    if (method == nullptr) {
      throw std::runtime_error("Method " + method_name + " not found");
    }

    runtime::Thread     thread;
    engine::Interpreter interpreter;

    // Create caller frame to receive return value
    runtime::Frame caller_frame(method);
    caller_frame.setCallerPC(method->getCode().size());
    thread.pushFrame(std::move(caller_frame));
    thread.setPC(method->getCode().size());

    // Create callee frame
    runtime::Frame callee_frame(method);
    callee_frame.getLocalVariables().setDouble(0, arg1);
    callee_frame.getLocalVariables().setDouble(2, arg2);  // double takes 2 slots
    thread.pushFrame(std::move(callee_frame));
    thread.setPC(0);

    interpreter.interpret(&thread);

    if (!thread.isStackEmpty()) {
      auto& current_frame = thread.getCurrentFrame();
      return current_frame.getOperandStack().popDouble();
    }
    throw std::runtime_error("Stack is empty after execution");
  }

  // Helper to execute a static method with one double argument
  Jdouble executeDoubleMethod1Arg(const std::string& method_name, Jdouble arg) {
    auto* klass = loader_->loadClass("tests.data.java.ArithmeticTest");
    if (klass == nullptr) {
      throw std::runtime_error("Failed to load ArithmeticTest class");
    }

    auto* method = klass->findMethod(method_name, "(D)D");
    if (method == nullptr) {
      throw std::runtime_error("Method " + method_name + " not found");
    }

    runtime::Thread     thread;
    engine::Interpreter interpreter;

    // Create caller frame to receive return value
    runtime::Frame caller_frame(method);
    caller_frame.setCallerPC(method->getCode().size());
    thread.pushFrame(std::move(caller_frame));
    thread.setPC(method->getCode().size());

    // Create callee frame
    runtime::Frame callee_frame(method);
    callee_frame.getLocalVariables().setDouble(0, arg);
    thread.pushFrame(std::move(callee_frame));
    thread.setPC(0);

    interpreter.interpret(&thread);

    if (!thread.isStackEmpty()) {
      auto& current_frame = thread.getCurrentFrame();
      return current_frame.getOperandStack().popDouble();
    }
    throw std::runtime_error("Stack is empty after execution");
  }

  std::string                                test_classpath_;
  std::vector<std::string>                   classpath_list_;
  std::unique_ptr<class_loader::ClassLoader> loader_;
};

}  // namespace

// ============================================================================
// Integer Arithmetic Tests
// ============================================================================

TEST_F(InterpreterArithmeticTest, IADD_Basic) {
  EXPECT_EQ(executeIntMethod("testIADD", 10, 20), 30);
  EXPECT_EQ(executeIntMethod("testIADD", -5, 10), 5);
  EXPECT_EQ(executeIntMethod("testIADD", 0, 0), 0);
}

TEST_F(InterpreterArithmeticTest, ISUB_Basic) {
  EXPECT_EQ(executeIntMethod("testISUB", 20, 10), 10);
  EXPECT_EQ(executeIntMethod("testISUB", 10, 20), -10);
  EXPECT_EQ(executeIntMethod("testISUB", 0, 0), 0);
}

TEST_F(InterpreterArithmeticTest, IMUL_Basic) {
  EXPECT_EQ(executeIntMethod("testIMUL", 5, 6), 30);
  EXPECT_EQ(executeIntMethod("testIMUL", -5, 6), -30);
  EXPECT_EQ(executeIntMethod("testIMUL", 0, 100), 0);
}

TEST_F(InterpreterArithmeticTest, IDIV_Basic) {
  EXPECT_EQ(executeIntMethod("testIDIV", 20, 5), 4);
  EXPECT_EQ(executeIntMethod("testIDIV", 21, 5), 4);  // Integer division truncates
  EXPECT_EQ(executeIntMethod("testIDIV", -20, 5), -4);
}

TEST_F(InterpreterArithmeticTest, IDIV_DivideByZero) {
  EXPECT_THROW(executeIntMethod("testIDIV", 10, 0), std::runtime_error);
}

TEST_F(InterpreterArithmeticTest, IREM_Basic) {
  EXPECT_EQ(executeIntMethod("testIREM", 20, 5), 0);
  EXPECT_EQ(executeIntMethod("testIREM", 21, 5), 1);
  EXPECT_EQ(executeIntMethod("testIREM", -21, 5), -1);
}

TEST_F(InterpreterArithmeticTest, IREM_DivideByZero) {
  EXPECT_THROW(executeIntMethod("testIREM", 10, 0), std::runtime_error);
}

TEST_F(InterpreterArithmeticTest, INEG_Basic) {
  EXPECT_EQ(executeIntMethod1Arg("testINEG", 10), -10);
  EXPECT_EQ(executeIntMethod1Arg("testINEG", -10), 10);
  EXPECT_EQ(executeIntMethod1Arg("testINEG", 0), 0);
}

// ============================================================================
// Long Arithmetic Tests
// ============================================================================

TEST_F(InterpreterArithmeticTest, LADD_Basic) {
  EXPECT_EQ(executeLongMethod("testLADD", 10L, 20L), 30L);
  EXPECT_EQ(executeLongMethod("testLADD", -5L, 10L), 5L);
}

TEST_F(InterpreterArithmeticTest, LSUB_Basic) {
  EXPECT_EQ(executeLongMethod("testLSUB", 20L, 10L), 10L);
  EXPECT_EQ(executeLongMethod("testLSUB", 10L, 20L), -10L);
}

TEST_F(InterpreterArithmeticTest, LMUL_Basic) {
  EXPECT_EQ(executeLongMethod("testLMUL", 5L, 6L), 30L);
  EXPECT_EQ(executeLongMethod("testLMUL", -5L, 6L), -30L);
}

TEST_F(InterpreterArithmeticTest, LDIV_Basic) {
  EXPECT_EQ(executeLongMethod("testLDIV", 20L, 5L), 4L);
  EXPECT_EQ(executeLongMethod("testLDIV", 21L, 5L), 4L);
}

TEST_F(InterpreterArithmeticTest, LDIV_DivideByZero) {
  EXPECT_THROW(executeLongMethod("testLDIV", 10L, 0L), std::runtime_error);
}

TEST_F(InterpreterArithmeticTest, LREM_Basic) {
  EXPECT_EQ(executeLongMethod("testLREM", 20L, 5L), 0L);
  EXPECT_EQ(executeLongMethod("testLREM", 21L, 5L), 1L);
}

TEST_F(InterpreterArithmeticTest, LREM_DivideByZero) {
  EXPECT_THROW(executeLongMethod("testLREM", 10L, 0L), std::runtime_error);
}

TEST_F(InterpreterArithmeticTest, LNEG_Basic) {
  EXPECT_EQ(executeLongMethod1Arg("testLNEG", 10L), -10L);
  EXPECT_EQ(executeLongMethod1Arg("testLNEG", -10L), 10L);
}

// ============================================================================
// Float Arithmetic Tests
// ============================================================================

TEST_F(InterpreterArithmeticTest, FADD_Basic) {
  EXPECT_FLOAT_EQ(executeFloatMethod("testFADD", 10.5F, 20.5F), 31.0F);
  EXPECT_FLOAT_EQ(executeFloatMethod("testFADD", -5.5F, 10.5F), 5.0F);
}

TEST_F(InterpreterArithmeticTest, FSUB_Basic) {
  EXPECT_FLOAT_EQ(executeFloatMethod("testFSUB", 20.5F, 10.5F), 10.0F);
  EXPECT_FLOAT_EQ(executeFloatMethod("testFSUB", 10.5F, 20.5F), -10.0F);
}

TEST_F(InterpreterArithmeticTest, FMUL_Basic) {
  EXPECT_FLOAT_EQ(executeFloatMethod("testFMUL", 5.0F, 6.0F), 30.0F);
  EXPECT_FLOAT_EQ(executeFloatMethod("testFMUL", -5.0F, 6.0F), -30.0F);
}

TEST_F(InterpreterArithmeticTest, FDIV_Basic) {
  EXPECT_FLOAT_EQ(executeFloatMethod("testFDIV", 20.0F, 5.0F), 4.0F);
  EXPECT_FLOAT_EQ(executeFloatMethod("testFDIV", 21.0F, 5.0F), 4.2F);
}

TEST_F(InterpreterArithmeticTest, FREM_Basic) {
  EXPECT_FLOAT_EQ(executeFloatMethod("testFREM", 20.5F, 5.0F), 0.5F);
  EXPECT_FLOAT_EQ(executeFloatMethod("testFREM", 21.0F, 5.0F), 1.0F);
}

TEST_F(InterpreterArithmeticTest, FNEG_Basic) {
  EXPECT_FLOAT_EQ(executeFloatMethod1Arg("testFNEG", 10.5F), -10.5F);
  EXPECT_FLOAT_EQ(executeFloatMethod1Arg("testFNEG", -10.5F), 10.5F);
}

// ============================================================================
// Double Arithmetic Tests
// ============================================================================

TEST_F(InterpreterArithmeticTest, DADD_Basic) {
  EXPECT_DOUBLE_EQ(executeDoubleMethod("testDADD", 10.5, 20.5), 31.0);
  EXPECT_DOUBLE_EQ(executeDoubleMethod("testDADD", -5.5, 10.5), 5.0);
}

TEST_F(InterpreterArithmeticTest, DSUB_Basic) {
  EXPECT_DOUBLE_EQ(executeDoubleMethod("testDSUB", 20.5, 10.5), 10.0);
  EXPECT_DOUBLE_EQ(executeDoubleMethod("testDSUB", 10.5, 20.5), -10.0);
}

TEST_F(InterpreterArithmeticTest, DMUL_Basic) {
  EXPECT_DOUBLE_EQ(executeDoubleMethod("testDMUL", 5.0, 6.0), 30.0);
  EXPECT_DOUBLE_EQ(executeDoubleMethod("testDMUL", -5.0, 6.0), -30.0);
}

TEST_F(InterpreterArithmeticTest, DDIV_Basic) {
  EXPECT_DOUBLE_EQ(executeDoubleMethod("testDDIV", 20.0, 5.0), 4.0);
  EXPECT_DOUBLE_EQ(executeDoubleMethod("testDDIV", 21.0, 5.0), 4.2);
}

TEST_F(InterpreterArithmeticTest, DREM_Basic) {
  EXPECT_DOUBLE_EQ(executeDoubleMethod("testDREM", 20.5, 5.0), 0.5);
  EXPECT_DOUBLE_EQ(executeDoubleMethod("testDREM", 21.0, 5.0), 1.0);
}

TEST_F(InterpreterArithmeticTest, DNEG_Basic) {
  EXPECT_DOUBLE_EQ(executeDoubleMethod1Arg("testDNEG", 10.5), -10.5);
  EXPECT_DOUBLE_EQ(executeDoubleMethod1Arg("testDNEG", -10.5), 10.5);
}
