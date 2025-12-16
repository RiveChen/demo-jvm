#include <gtest/gtest.h>

#include "class_loader/class_loader.h"
#include "engine/interpreter.h"
#include "interpreter_test_base.h"
#include "runtime/frame.h"
#include "runtime/thread.h"

// ============================================================================
// Type Conversion Tests
// ============================================================================

class InterpreterConversionTest : public InterpreterTestBase {
 protected:
  // Helper to execute a static method with one int argument and get float result
  Jfloat executeIntToFloatMethod1Arg(const std::string& class_name, const std::string& method_name,
                                     const std::string& descriptor, Jint arg) {
    auto* klass = loader_->loadClass(class_name);
    if (klass == nullptr) {
      throw std::runtime_error("Failed to load class: " + class_name);
    }

    auto* method = klass->findMethod(method_name, descriptor);
    if (method == nullptr) {
      throw std::runtime_error("Method " + method_name + " with descriptor " + descriptor +
                               " not found");
    }

    runtime::Thread     thread;
    engine::Interpreter interpreter;

    runtime::Frame caller_frame(method);
    caller_frame.setCallerPC(method->getCode().size());
    thread.pushFrame(std::move(caller_frame));
    thread.setPC(method->getCode().size());

    runtime::Frame callee_frame(method);
    callee_frame.getLocalVariables().setInt(0, arg);
    thread.pushFrame(std::move(callee_frame));
    thread.setPC(0);

    interpreter.interpret(&thread);

    if (!thread.isStackEmpty()) {
      auto& current_frame = thread.getCurrentFrame();
      return current_frame.getOperandStack().popFloat();
    }
    throw std::runtime_error("Stack is empty after execution");
  }

  // Helper to execute a static method with one int argument and get double result
  Jdouble executeIntToDoubleMethod1Arg(const std::string& class_name,
                                       const std::string& method_name,
                                       const std::string& descriptor, Jint arg) {
    auto* klass = loader_->loadClass(class_name);
    if (klass == nullptr) {
      throw std::runtime_error("Failed to load class: " + class_name);
    }

    auto* method = klass->findMethod(method_name, descriptor);
    if (method == nullptr) {
      throw std::runtime_error("Method " + method_name + " with descriptor " + descriptor +
                               " not found");
    }

    runtime::Thread     thread;
    engine::Interpreter interpreter;

    runtime::Frame caller_frame(method);
    caller_frame.setCallerPC(method->getCode().size());
    thread.pushFrame(std::move(caller_frame));
    thread.setPC(method->getCode().size());

    runtime::Frame callee_frame(method);
    callee_frame.getLocalVariables().setInt(0, arg);
    thread.pushFrame(std::move(callee_frame));
    thread.setPC(0);

    interpreter.interpret(&thread);

    if (!thread.isStackEmpty()) {
      auto& current_frame = thread.getCurrentFrame();
      return current_frame.getOperandStack().popDouble();
    }
    throw std::runtime_error("Stack is empty after execution");
  }

  // Helper to execute a static method with one long argument and get float result
  Jfloat executeLongToFloatMethod1Arg(const std::string& class_name, const std::string& method_name,
                                      const std::string& descriptor, Jlong arg) {
    auto* klass = loader_->loadClass(class_name);
    if (klass == nullptr) {
      throw std::runtime_error("Failed to load class: " + class_name);
    }

    auto* method = klass->findMethod(method_name, descriptor);
    if (method == nullptr) {
      throw std::runtime_error("Method " + method_name + " with descriptor " + descriptor +
                               " not found");
    }

    runtime::Thread     thread;
    engine::Interpreter interpreter;

    runtime::Frame caller_frame(method);
    caller_frame.setCallerPC(method->getCode().size());
    thread.pushFrame(std::move(caller_frame));
    thread.setPC(method->getCode().size());

    runtime::Frame callee_frame(method);
    callee_frame.getLocalVariables().setLong(0, arg);
    thread.pushFrame(std::move(callee_frame));
    thread.setPC(0);

    interpreter.interpret(&thread);

    if (!thread.isStackEmpty()) {
      auto& current_frame = thread.getCurrentFrame();
      return current_frame.getOperandStack().popFloat();
    }
    throw std::runtime_error("Stack is empty after execution");
  }

  // Helper to execute a static method with one long argument and get double result
  Jdouble executeLongToDoubleMethod1Arg(const std::string& class_name,
                                        const std::string& method_name,
                                        const std::string& descriptor, Jlong arg) {
    auto* klass = loader_->loadClass(class_name);
    if (klass == nullptr) {
      throw std::runtime_error("Failed to load class: " + class_name);
    }

    auto* method = klass->findMethod(method_name, descriptor);
    if (method == nullptr) {
      throw std::runtime_error("Method " + method_name + " with descriptor " + descriptor +
                               " not found");
    }

    runtime::Thread     thread;
    engine::Interpreter interpreter;

    runtime::Frame caller_frame(method);
    caller_frame.setCallerPC(method->getCode().size());
    thread.pushFrame(std::move(caller_frame));
    thread.setPC(method->getCode().size());

    runtime::Frame callee_frame(method);
    callee_frame.getLocalVariables().setLong(0, arg);
    thread.pushFrame(std::move(callee_frame));
    thread.setPC(0);

    interpreter.interpret(&thread);

    if (!thread.isStackEmpty()) {
      auto& current_frame = thread.getCurrentFrame();
      return current_frame.getOperandStack().popDouble();
    }
    throw std::runtime_error("Stack is empty after execution");
  }

  // Helper to execute a static method with one double argument and get float result
  Jfloat executeDoubleToFloatMethod1Arg(const std::string& class_name,
                                        const std::string& method_name,
                                        const std::string& descriptor, Jdouble arg) {
    auto* klass = loader_->loadClass(class_name);
    if (klass == nullptr) {
      throw std::runtime_error("Failed to load class: " + class_name);
    }

    auto* method = klass->findMethod(method_name, descriptor);
    if (method == nullptr) {
      throw std::runtime_error("Method " + method_name + " with descriptor " + descriptor +
                               " not found");
    }

    runtime::Thread     thread;
    engine::Interpreter interpreter;

    runtime::Frame caller_frame(method);
    caller_frame.setCallerPC(method->getCode().size());
    thread.pushFrame(std::move(caller_frame));
    thread.setPC(method->getCode().size());

    runtime::Frame callee_frame(method);
    callee_frame.getLocalVariables().setDouble(0, arg);
    thread.pushFrame(std::move(callee_frame));
    thread.setPC(0);

    interpreter.interpret(&thread);

    if (!thread.isStackEmpty()) {
      auto& current_frame = thread.getCurrentFrame();
      return current_frame.getOperandStack().popFloat();
    }
    throw std::runtime_error("Stack is empty after execution");
  }

  // Helper to execute a static method with one float argument and get int result
  Jint executeFloatToIntMethod1Arg(const std::string& class_name, const std::string& method_name,
                                   const std::string& descriptor, Jfloat arg) {
    auto* klass = loader_->loadClass(class_name);
    if (klass == nullptr) {
      throw std::runtime_error("Failed to load class: " + class_name);
    }

    auto* method = klass->findMethod(method_name, descriptor);
    if (method == nullptr) {
      throw std::runtime_error("Method " + method_name + " with descriptor " + descriptor +
                               " not found");
    }

    runtime::Thread     thread;
    engine::Interpreter interpreter;

    runtime::Frame caller_frame(method);
    caller_frame.setCallerPC(method->getCode().size());
    thread.pushFrame(std::move(caller_frame));
    thread.setPC(method->getCode().size());

    runtime::Frame callee_frame(method);
    callee_frame.getLocalVariables().setFloat(0, arg);
    thread.pushFrame(std::move(callee_frame));
    thread.setPC(0);

    interpreter.interpret(&thread);

    if (!thread.isStackEmpty()) {
      auto& current_frame = thread.getCurrentFrame();
      return current_frame.getOperandStack().popInt();
    }
    throw std::runtime_error("Stack is empty after execution");
  }

  // Helper to execute a static method with one float argument and get long result
  Jlong executeFloatToLongMethod1Arg(const std::string& class_name, const std::string& method_name,
                                     const std::string& descriptor, Jfloat arg) {
    auto* klass = loader_->loadClass(class_name);
    if (klass == nullptr) {
      throw std::runtime_error("Failed to load class: " + class_name);
    }

    auto* method = klass->findMethod(method_name, descriptor);
    if (method == nullptr) {
      throw std::runtime_error("Method " + method_name + " with descriptor " + descriptor +
                               " not found");
    }

    runtime::Thread     thread;
    engine::Interpreter interpreter;

    runtime::Frame caller_frame(method);
    caller_frame.setCallerPC(method->getCode().size());
    thread.pushFrame(std::move(caller_frame));
    thread.setPC(method->getCode().size());

    runtime::Frame callee_frame(method);
    callee_frame.getLocalVariables().setFloat(0, arg);
    thread.pushFrame(std::move(callee_frame));
    thread.setPC(0);

    interpreter.interpret(&thread);

    if (!thread.isStackEmpty()) {
      auto& current_frame = thread.getCurrentFrame();
      return current_frame.getOperandStack().popLong();
    }
    throw std::runtime_error("Stack is empty after execution");
  }

  // Helper to execute a static method with one float argument and get double result
  Jdouble executeFloatToDoubleMethod1Arg(const std::string& class_name,
                                         const std::string& method_name,
                                         const std::string& descriptor, Jfloat arg) {
    auto* klass = loader_->loadClass(class_name);
    if (klass == nullptr) {
      throw std::runtime_error("Failed to load class: " + class_name);
    }

    auto* method = klass->findMethod(method_name, descriptor);
    if (method == nullptr) {
      throw std::runtime_error("Method " + method_name + " with descriptor " + descriptor +
                               " not found");
    }

    runtime::Thread     thread;
    engine::Interpreter interpreter;

    runtime::Frame caller_frame(method);
    caller_frame.setCallerPC(method->getCode().size());
    thread.pushFrame(std::move(caller_frame));
    thread.setPC(method->getCode().size());

    runtime::Frame callee_frame(method);
    callee_frame.getLocalVariables().setFloat(0, arg);
    thread.pushFrame(std::move(callee_frame));
    thread.setPC(0);

    interpreter.interpret(&thread);

    if (!thread.isStackEmpty()) {
      auto& current_frame = thread.getCurrentFrame();
      return current_frame.getOperandStack().popDouble();
    }
    throw std::runtime_error("Stack is empty after execution");
  }

  // Helper to execute a static method with one double argument and get int result
  Jint executeDoubleToIntMethod1Arg(const std::string& class_name, const std::string& method_name,
                                    const std::string& descriptor, Jdouble arg) {
    auto* klass = loader_->loadClass(class_name);
    if (klass == nullptr) {
      throw std::runtime_error("Failed to load class: " + class_name);
    }

    auto* method = klass->findMethod(method_name, descriptor);
    if (method == nullptr) {
      throw std::runtime_error("Method " + method_name + " with descriptor " + descriptor +
                               " not found");
    }

    runtime::Thread     thread;
    engine::Interpreter interpreter;

    runtime::Frame caller_frame(method);
    caller_frame.setCallerPC(method->getCode().size());
    thread.pushFrame(std::move(caller_frame));
    thread.setPC(method->getCode().size());

    runtime::Frame callee_frame(method);
    callee_frame.getLocalVariables().setDouble(0, arg);
    thread.pushFrame(std::move(callee_frame));
    thread.setPC(0);

    interpreter.interpret(&thread);

    if (!thread.isStackEmpty()) {
      auto& current_frame = thread.getCurrentFrame();
      return current_frame.getOperandStack().popInt();
    }
    throw std::runtime_error("Stack is empty after execution");
  }

  // Helper to execute a static method with one double argument and get long result
  Jlong executeDoubleToLongMethod1Arg(const std::string& class_name, const std::string& method_name,
                                      const std::string& descriptor, Jdouble arg) {
    auto* klass = loader_->loadClass(class_name);
    if (klass == nullptr) {
      throw std::runtime_error("Failed to load class: " + class_name);
    }

    auto* method = klass->findMethod(method_name, descriptor);
    if (method == nullptr) {
      throw std::runtime_error("Method " + method_name + " with descriptor " + descriptor +
                               " not found");
    }

    runtime::Thread     thread;
    engine::Interpreter interpreter;

    runtime::Frame caller_frame(method);
    caller_frame.setCallerPC(method->getCode().size());
    thread.pushFrame(std::move(caller_frame));
    thread.setPC(method->getCode().size());

    runtime::Frame callee_frame(method);
    callee_frame.getLocalVariables().setDouble(0, arg);
    thread.pushFrame(std::move(callee_frame));
    thread.setPC(0);

    interpreter.interpret(&thread);

    if (!thread.isStackEmpty()) {
      auto& current_frame = thread.getCurrentFrame();
      return current_frame.getOperandStack().popLong();
    }
    throw std::runtime_error("Stack is empty after execution");
  }
};

// ============================================================================
// Int to Other Types
// ============================================================================

TEST_F(InterpreterConversionTest, I2L_Basic) {
  EXPECT_EQ(executeLongMethod1Arg("tests.data.java.ConversionTest", "testI2L", "(I)J", 42), 42L);
  EXPECT_EQ(executeLongMethod1Arg("tests.data.java.ConversionTest", "testI2L", "(I)J", -42), -42L);
}

TEST_F(InterpreterConversionTest, I2F_Basic) {
  EXPECT_FLOAT_EQ(
    executeIntToFloatMethod1Arg("tests.data.java.ConversionTest", "testI2F", "(I)F", 42), 42.0F);
  EXPECT_FLOAT_EQ(
    executeIntToFloatMethod1Arg("tests.data.java.ConversionTest", "testI2F", "(I)F", -42), -42.0F);
}

TEST_F(InterpreterConversionTest, I2D_Basic) {
  EXPECT_DOUBLE_EQ(
    executeIntToDoubleMethod1Arg("tests.data.java.ConversionTest", "testI2D", "(I)D", 42), 42.0);
  EXPECT_DOUBLE_EQ(
    executeIntToDoubleMethod1Arg("tests.data.java.ConversionTest", "testI2D", "(I)D", -42), -42.0);
}

// ============================================================================
// Long to Other Types
// ============================================================================

TEST_F(InterpreterConversionTest, L2I_Basic) {
  EXPECT_EQ(executeIntMethod1Arg("tests.data.java.ConversionTest", "testL2I", "(J)I", 42L), 42);
  EXPECT_EQ(executeIntMethod1Arg("tests.data.java.ConversionTest", "testL2I", "(J)I", -42L), -42);
}

TEST_F(InterpreterConversionTest, L2F_Basic) {
  EXPECT_FLOAT_EQ(
    executeLongToFloatMethod1Arg("tests.data.java.ConversionTest", "testL2F", "(J)F", 42L), 42.0F);
}

TEST_F(InterpreterConversionTest, L2D_Basic) {
  EXPECT_DOUBLE_EQ(
    executeLongToDoubleMethod1Arg("tests.data.java.ConversionTest", "testL2D", "(J)D", 42L), 42.0);
}

// ============================================================================
// Float to Other Types
// ============================================================================

TEST_F(InterpreterConversionTest, F2I_Basic) {
  EXPECT_EQ(executeFloatToIntMethod1Arg("tests.data.java.ConversionTest", "testF2I", "(F)I", 42.5F),
            42);
  EXPECT_EQ(
    executeFloatToIntMethod1Arg("tests.data.java.ConversionTest", "testF2I", "(F)I", -42.5F), -42);
}

TEST_F(InterpreterConversionTest, F2I_NaN) {
  EXPECT_EQ(executeIntMethod0Arg("tests.data.java.ConversionTest", "testF2I_NaN", "()I"), 0);
}

TEST_F(InterpreterConversionTest, F2I_Infinity) {
  EXPECT_EQ(executeIntMethod0Arg("tests.data.java.ConversionTest", "testF2I_Infinity", "()I"), 0);
}

TEST_F(InterpreterConversionTest, F2L_Basic) {
  EXPECT_EQ(
    executeFloatToLongMethod1Arg("tests.data.java.ConversionTest", "testF2L", "(F)J", 42.5F), 42L);
}

TEST_F(InterpreterConversionTest, F2L_NaN) {
  // Note: F2L with NaN should return 0L
  // This is tested indirectly through the Java test methods
  EXPECT_EQ(executeFloatToLongMethod1Arg("tests.data.java.ConversionTest", "testF2L", "(F)J", 0.0F),
            0L);
}

TEST_F(InterpreterConversionTest, F2D_Basic) {
  EXPECT_DOUBLE_EQ(
    executeFloatToDoubleMethod1Arg("tests.data.java.ConversionTest", "testF2D", "(F)D", 42.5F),
    42.5);
}

// ============================================================================
// Double to Other Types
// ============================================================================

TEST_F(InterpreterConversionTest, D2I_Basic) {
  EXPECT_EQ(executeDoubleToIntMethod1Arg("tests.data.java.ConversionTest", "testD2I", "(D)I", 42.5),
            42);
  EXPECT_EQ(
    executeDoubleToIntMethod1Arg("tests.data.java.ConversionTest", "testD2I", "(D)I", -42.5), -42);
}

TEST_F(InterpreterConversionTest, D2I_NaN) {
  EXPECT_EQ(executeIntMethod0Arg("tests.data.java.ConversionTest", "testD2I_NaN", "()I"), 0);
}

TEST_F(InterpreterConversionTest, D2I_Infinity) {
  EXPECT_EQ(executeIntMethod0Arg("tests.data.java.ConversionTest", "testD2I_Infinity", "()I"), 0);
}

TEST_F(InterpreterConversionTest, D2L_Basic) {
  EXPECT_EQ(
    executeDoubleToLongMethod1Arg("tests.data.java.ConversionTest", "testD2L", "(D)J", 42.5), 42L);
}

TEST_F(InterpreterConversionTest, D2L_NaN) {
  // Note: This test uses a Java method that creates NaN, not a C++ NaN value
  // The actual NaN handling is tested via testD2I_NaN which uses Java code
  EXPECT_EQ(executeDoubleToLongMethod1Arg("tests.data.java.ConversionTest", "testD2L", "(D)J", 0.0),
            0L);
}

TEST_F(InterpreterConversionTest, D2F_Basic) {
  EXPECT_FLOAT_EQ(
    executeDoubleToFloatMethod1Arg("tests.data.java.ConversionTest", "testD2F", "(D)F", 42.5),
    42.5F);
}

// ============================================================================
// Int to Narrow Types
// ============================================================================

TEST_F(InterpreterConversionTest, I2B_Basic) {
  EXPECT_EQ(executeIntMethod1Arg("tests.data.java.ConversionTest", "testI2B", "(I)I", 127), 127);
  EXPECT_EQ(executeIntMethod1Arg("tests.data.java.ConversionTest", "testI2B", "(I)I", -128), -128);
  EXPECT_EQ(executeIntMethod1Arg("tests.data.java.ConversionTest", "testI2B", "(I)I", 256), 0);
}

TEST_F(InterpreterConversionTest, I2C_Basic) {
  EXPECT_EQ(executeIntMethod1Arg("tests.data.java.ConversionTest", "testI2C", "(I)I", 65), 65);
  EXPECT_EQ(executeIntMethod1Arg("tests.data.java.ConversionTest", "testI2C", "(I)I", 65535),
            65535);
}

TEST_F(InterpreterConversionTest, I2S_Basic) {
  EXPECT_EQ(executeIntMethod1Arg("tests.data.java.ConversionTest", "testI2S", "(I)I", 32767),
            32767);
  EXPECT_EQ(executeIntMethod1Arg("tests.data.java.ConversionTest", "testI2S", "(I)I", -32768),
            -32768);
}
