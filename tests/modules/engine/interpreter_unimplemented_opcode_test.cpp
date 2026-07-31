#include "engine/opcode.hpp"
#include "interpreter_test_base.hpp"

#include <gtest/gtest.h>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

using namespace jvm;

namespace {

struct UnsupportedInstruction {
  const char*     name;
  U1              opcode;
  std::vector<U1> code;
};

class InterpreterUnimplementedOpcodeTest
  : public InterpreterTestBase,
    public ::testing::WithParamInterface<UnsupportedInstruction> {
 protected:
  static constexpr const char* kClassName  = "tests.data.java.OpcodeFailureTest";
  static constexpr const char* kMethodName = "testOpcode";

  std::string interpretPatchedMethod(const std::vector<U1>& code) {
    auto* klass = loader_->loadClass(kClassName);
    if (klass == nullptr) {
      throw std::runtime_error("Class not found");
    }

    auto* method = klass->findMethod(kMethodName, "()V");
    if (method == nullptr) {
      throw std::runtime_error("Method not found");
    }

    // Keep the parsed method metadata/constant pool and replace only its code
    // in this isolated fixture, avoiding a hand-written class-file assembler.
    auto& mutable_code = const_cast<std::vector<U1>&>(method->getCode());
    mutable_code       = code;

    runtime::Thread     thread;
    engine::Interpreter interpreter;

    runtime::Frame caller_frame(method);
    caller_frame.setPC(mutable_code.size());
    thread.pushFrame(std::move(caller_frame));
    thread.pushFrame(runtime::Frame(method));

    try {
      interpreter.interpret(&thread);
    } catch (const std::runtime_error& error) {
      return error.what();
    }
    return {};
  }
};

TEST_P(InterpreterUnimplementedOpcodeTest, FailsWithOpcodeAndLocationContext) {
  const auto& instruction = GetParam();
  const auto  message     = interpretPatchedMethod(instruction.code);

  ASSERT_FALSE(message.empty()) << instruction.name << " continued execution instead of failing";
  EXPECT_NE(message.find(instruction.name), std::string::npos) << message;

  std::ostringstream opcode_hex;
  opcode_hex << "0x" << std::hex << std::setw(2) << std::setfill('0')
             << static_cast<unsigned>(instruction.opcode);
  EXPECT_NE(message.find(opcode_hex.str()), std::string::npos) << message;
  EXPECT_NE(message.find("OpcodeFailureTest"), std::string::npos) << message;
  EXPECT_NE(message.find(kMethodName), std::string::npos) << message;
  EXPECT_NE(message.find("pc=0"), std::string::npos) << message;
}

std::vector<UnsupportedInstruction> unsupportedInstructions() {
  using namespace engine;
  return {
    {"IALOAD", IALOAD, {IALOAD, RETURN}},
    {"LALOAD", LALOAD, {LALOAD, RETURN}},
    {"FALOAD", FALOAD, {FALOAD, RETURN}},
    {"DALOAD", DALOAD, {DALOAD, RETURN}},
    {"AALOAD", AALOAD, {AALOAD, RETURN}},
    {"BALOAD", BALOAD, {BALOAD, RETURN}},
    {"CALOAD", CALOAD, {CALOAD, RETURN}},
    {"SALOAD", SALOAD, {SALOAD, RETURN}},
    {"IASTORE", IASTORE, {IASTORE, RETURN}},
    {"LASTORE", LASTORE, {LASTORE, RETURN}},
    {"FASTORE", FASTORE, {FASTORE, RETURN}},
    {"DASTORE", DASTORE, {DASTORE, RETURN}},
    {"AASTORE", AASTORE, {AASTORE, RETURN}},
    {"BASTORE", BASTORE, {BASTORE, RETURN}},
    {"CASTORE", CASTORE, {CASTORE, RETURN}},
    {"SASTORE", SASTORE, {SASTORE, RETURN}},
    {"JSR", JSR, {JSR, RETURN}},
    {"RET", RET, {RET, RETURN}},
    {"JSR_W", JSR_W, {JSR_W, RETURN}},
    {"INVOKEDYNAMIC", INVOKEDYNAMIC, {INVOKEDYNAMIC, RETURN}},
    {"ATHROW", ATHROW, {ATHROW, RETURN}},
    {"MONITORENTER", MONITORENTER, {MONITORENTER, RETURN}},
    {"MONITOREXIT", MONITOREXIT, {MONITOREXIT, RETURN}},
    {"NEWARRAY", NEWARRAY, {NEWARRAY, RETURN}},
    {"ANEWARRAY", ANEWARRAY, {ANEWARRAY, RETURN}},
    {"ARRAYLENGTH", ARRAYLENGTH, {ARRAYLENGTH, RETURN}},
    {"MULTIANEWARRAY", MULTIANEWARRAY, {MULTIANEWARRAY, RETURN}},
    {"RET", RET, {WIDE, RET, 0x00, 0x00, RETURN}},
  };
}

INSTANTIATE_TEST_SUITE_P(EveryUnsupportedInstruction, InterpreterUnimplementedOpcodeTest,
                         ::testing::ValuesIn(unsupportedInstructions()),
                         [](const ::testing::TestParamInfo<UnsupportedInstruction>& info) {
                           std::string name = info.param.name;
                           if (info.param.code.front() == engine::WIDE) {
                             name = "WIDE_" + name;
                           }
                           return name;
                         });

}  // namespace
