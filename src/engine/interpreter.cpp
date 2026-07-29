/**
 * @file interpreter.cpp
 * @brief JVM bytecode interpreter implementation.
 *
 * Implements the instruction dispatch loop for all defined opcodes.
 * Each instruction group (constants, loads, stores, arithmetic, etc.)
 * is handled within the main switch statement.
 *
 * @see JVM Specification §6.5 for the complete instruction set.
 */

#include "interpreter.hpp"

#include "bytecode_reader.hpp"
#include "engine/stub_intercepts.hpp"
#include "memory/heap.hpp"
#include "native_registry.hpp"
#include "oops/klass.hpp"
#include "oops/method.hpp"
#include "oops/object.hpp"
#include "oops/string_pool.hpp"
#include "opcode.hpp"
#include "runtime/frame.hpp"
#include "runtime/thread.hpp"
#include "utilities/descriptor.hpp"
#include "utilities/logger.hpp"
#include "utilities/types.hpp"

#include <cmath>
#include <stdexcept>
#include <vector>

namespace jvm::engine {

// (readability-function-size, hicpp-function-size, readability-function-cognitive-complexity)
// NOLINTNEXTLINE
void Interpreter::interpret(runtime::Thread* thread) {
  // cache pc to avoid fetching it from thread every time
  // for thread-pc, we only use it when the frame is popped or pushed
  size_t pc = thread->getCurrentFrame().getPC();

  while (true) {
    if (thread->isStackEmpty()) {
      return;
    }

    auto&       frame      = thread->getCurrentFrame();
    auto*       method     = frame.getMethod();
    auto&       local_vars = frame.getLocalVariables();
    auto&       op_stack   = frame.getOperandStack();
    auto&       rt_cp      = method->getOwnerKlass()->getRuntimeConstantPool();
    const auto& code       = method->getCode();

    // size_t pc = thread->getPC();
    if (pc >= code.size()) {
      // PC is beyond code length, method has finished executing
      // This typically means the method ended without an explicit RETURN
      // In a real JVM, this would be an error, but for testing we'll just pop the frame
      // If there's a return value on the stack, it means IRETURN/LRETURN/etc
      // just executed - return now so the test helper can read the value
      return;
    }

    // fetch opcode
    BytecodeReader reader(code, pc);          // note we pass pc by ref here
    auto           opcode = reader.readU1();  // note pc incremented by 1 here
    LOG_TRACE("pc=", (pc - 1), " 0x", "0123456789abcdef"[opcode >> 4U],
              "0123456789abcdef"[opcode & 0x0FU], " (", opcode_name(opcode), ")");

    // NOLINTBEGIN(bugprone-branch-clone)
    // NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
    switch (opcode) {
      case NOP:
        break;
      /* #region Push constants */

      // Function: Push constant values onto operand stack
      // Components: op_stack
      case ACONST_NULL:
        op_stack.pushRef(nullptr);
        break;
      case ICONST_M1:
        op_stack.pushInt(-1);
        break;
      case ICONST_0:
        op_stack.pushInt(0);
        break;
      case ICONST_1:
        op_stack.pushInt(1);
        break;
      case ICONST_2:
        op_stack.pushInt(2);
        break;
      case ICONST_3:
        op_stack.pushInt(3);
        break;
      case ICONST_4:
        op_stack.pushInt(4);
        break;
      case ICONST_5:
        op_stack.pushInt(5);
        break;
      case LCONST_0:
        op_stack.pushLong(0L);
        break;
      case LCONST_1:
        op_stack.pushLong(1L);
        break;
      case FCONST_0:
        op_stack.pushFloat(0.0F);
        break;
      case FCONST_1:
        op_stack.pushFloat(1.0F);
        break;
      case FCONST_2:
        op_stack.pushFloat(2.0F);
        break;
      case DCONST_0:
        op_stack.pushDouble(0.0);
        break;
      case DCONST_1:
        op_stack.pushDouble(1.0);
        break;
      /* #endregion Push constants */

      /* #region Push immediate values */

      // Function: Push immediate byte/short values onto operand stack
      // Components: op_stack, thread (PC)
      case BIPUSH: {
        // byte integer push (sign-extend)
        op_stack.pushInt(reader.readSU1());
      } break;
      case SIPUSH: {
        // short integer push
        op_stack.pushInt(reader.readU2());
      } break;
      /* #endregion Push immediate values */

      /* #region Push from constant pool */

      // Function: Load constants from runtime constant pool onto operand stack
      // Components: rt_cp, op_stack, thread (PC)
      case LDC: {
        auto index    = reader.readU1();
        auto constant = rt_cp.getConstant(index);
        if (std::holds_alternative<Jint>(constant)) {
          op_stack.pushInt(std::get<Jint>(constant));
        } else if (std::holds_alternative<Jfloat>(constant)) {
          op_stack.pushFloat(std::get<Jfloat>(constant));
        } else if (std::holds_alternative<std::string>(constant)) {
          const std::string* s =
            oops::StringPool::getSingleton().intern(std::get<std::string>(constant));
          op_stack.pushRef(const_cast<std::string*>(s));
        }
      } break;
      case LDC_W: {
        auto index    = reader.readU2();
        auto constant = rt_cp.getConstant(index);
        if (std::holds_alternative<Jint>(constant)) {
          op_stack.pushInt(std::get<Jint>(constant));
        } else if (std::holds_alternative<Jfloat>(constant)) {
          op_stack.pushFloat(std::get<Jfloat>(constant));
        } else if (std::holds_alternative<std::string>(constant)) {
          const std::string* s =
            oops::StringPool::getSingleton().intern(std::get<std::string>(constant));
          op_stack.pushRef(const_cast<std::string*>(s));
        }
      } break;
      case LDC2_W: {
        auto index    = reader.readU2();
        auto constant = rt_cp.getConstant(index);
        if (std::holds_alternative<Jlong>(constant)) {
          op_stack.pushLong(std::get<Jlong>(constant));
        } else if (std::holds_alternative<Jdouble>(constant)) {
          op_stack.pushDouble(std::get<Jdouble>(constant));
        }
      } break;
      /* #endregion Push from constant pool */

      /* #region Loads */

      // Function: Load values from local variables onto operand stack
      // Components: local_vars, op_stack, thread (PC)
      case ILOAD: {
        auto index = reader.readU1();
        auto value = local_vars.getInt(index);
        op_stack.pushInt(value);
      } break;
      case LLOAD: {
        auto index = reader.readU1();
        auto value = local_vars.getLong(index);
        op_stack.pushLong(value);
      } break;
      case FLOAD: {
        auto index = reader.readU1();
        auto value = local_vars.getFloat(index);
        op_stack.pushFloat(value);
      } break;
      case DLOAD: {
        auto index = reader.readU1();
        auto value = local_vars.getDouble(index);
        op_stack.pushDouble(value);
      } break;
      case ALOAD: {
        auto  index = reader.readU1();
        auto* value = local_vars.getRef(index);
        op_stack.pushRef(value);
      } break;
      case ILOAD_0: {
        auto value = local_vars.getInt(0);
        op_stack.pushInt(value);
      } break;
      case ILOAD_1: {
        auto value = local_vars.getInt(1);
        op_stack.pushInt(value);
      } break;
      case ILOAD_2: {
        auto value = local_vars.getInt(2);
        op_stack.pushInt(value);
      } break;
      case ILOAD_3: {
        auto value = local_vars.getInt(3);
        op_stack.pushInt(value);
      } break;
      case LLOAD_0: {
        auto value = local_vars.getLong(0);
        op_stack.pushLong(value);
      } break;
      case LLOAD_1: {
        auto value = local_vars.getLong(1);
        op_stack.pushLong(value);
      } break;
      case LLOAD_2: {
        auto value = local_vars.getLong(2);
        op_stack.pushLong(value);
      } break;
      case LLOAD_3: {
        auto value = local_vars.getLong(3);
        op_stack.pushLong(value);
      } break;
      case FLOAD_0: {
        auto value = local_vars.getFloat(0);
        op_stack.pushFloat(value);
      } break;
      case FLOAD_1: {
        auto value = local_vars.getFloat(1);
        op_stack.pushFloat(value);
      } break;
      case FLOAD_2: {
        auto value = local_vars.getFloat(2);
        op_stack.pushFloat(value);
      } break;
      case FLOAD_3: {
        auto value = local_vars.getFloat(3);
        op_stack.pushFloat(value);
      } break;
      case DLOAD_0: {
        auto value = local_vars.getDouble(0);
        op_stack.pushDouble(value);
      } break;
      case DLOAD_1: {
        auto value = local_vars.getDouble(1);
        op_stack.pushDouble(value);
      } break;
      case DLOAD_2: {
        auto value = local_vars.getDouble(2);
        op_stack.pushDouble(value);
      } break;
      case DLOAD_3: {
        auto value = local_vars.getDouble(3);
        op_stack.pushDouble(value);
      } break;
      case ALOAD_0: {
        auto* value = local_vars.getRef(0);
        op_stack.pushRef(value);
      } break;
      case ALOAD_1: {
        auto* value = local_vars.getRef(1);
        op_stack.pushRef(value);
      } break;
      case ALOAD_2: {
        auto* value = local_vars.getRef(2);
        op_stack.pushRef(value);
      } break;
      case ALOAD_3: {
        auto* value = local_vars.getRef(3);
        op_stack.pushRef(value);
      } break;
      case IALOAD:
        // TODO: implement iaload
        break;
      case LALOAD:
        // TODO: implement laload
        break;
      case FALOAD:
        // TODO: implement faload
        break;
      case DALOAD:
        // TODO: implement daload
        break;
      case AALOAD:
        // TODO: implement aaload
        break;
      case BALOAD:
        // TODO: implement baload
        break;
      case CALOAD:
        // TODO: implement caload
        break;
      case SALOAD:
        // TODO: implement saload
        break;
      /* #endregion Loads */

      /* #region Stores */

      // Function: Store values from operand stack into local variables
      // Components: op_stack, local_vars, thread (PC)
      case ISTORE: {
        auto index = reader.readU1();
        auto value = op_stack.popInt();
        local_vars.setInt(index, value);
      } break;
      case LSTORE: {
        auto index = reader.readU1();
        auto value = op_stack.popLong();
        local_vars.setLong(index, value);
      } break;
      case FSTORE: {
        auto index = reader.readU1();
        auto value = op_stack.popFloat();
        local_vars.setFloat(index, value);
      } break;
      case DSTORE: {
        auto index = reader.readU1();
        auto value = op_stack.popDouble();
        local_vars.setDouble(index, value);
      } break;
      case ASTORE: {
        auto  index = reader.readU1();
        auto* value = op_stack.popRef();
        local_vars.setRef(index, value);
      } break;
      case ISTORE_0: {
        auto value = op_stack.popInt();
        local_vars.setInt(0, value);
      } break;
      case ISTORE_1: {
        auto value = op_stack.popInt();
        local_vars.setInt(1, value);
      } break;
      case ISTORE_2: {
        auto value = op_stack.popInt();
        local_vars.setInt(2, value);
      } break;
      case ISTORE_3: {
        auto value = op_stack.popInt();
        local_vars.setInt(3, value);
      } break;
      case LSTORE_0: {
        auto value = op_stack.popLong();
        local_vars.setLong(0, value);
      } break;
      case LSTORE_1: {
        auto value = op_stack.popLong();
        local_vars.setLong(1, value);
      } break;
      case LSTORE_2: {
        auto value = op_stack.popLong();
        local_vars.setLong(2, value);
      } break;
      case LSTORE_3: {
        auto value = op_stack.popLong();
        local_vars.setLong(3, value);
      } break;
      case FSTORE_0: {
        auto value = op_stack.popFloat();
        local_vars.setFloat(0, value);
      } break;
      case FSTORE_1: {
        auto value = op_stack.popFloat();
        local_vars.setFloat(1, value);
      } break;
      case FSTORE_2: {
        auto value = op_stack.popFloat();
        local_vars.setFloat(2, value);
      } break;
      case FSTORE_3: {
        auto value = op_stack.popFloat();
        local_vars.setFloat(3, value);
      } break;
      case DSTORE_0: {
        auto value = op_stack.popDouble();
        local_vars.setDouble(0, value);
      } break;
      case DSTORE_1: {
        auto value = op_stack.popDouble();
        local_vars.setDouble(1, value);
      } break;
      case DSTORE_2: {
        auto value = op_stack.popDouble();
        local_vars.setDouble(2, value);
      } break;
      case DSTORE_3: {
        auto value = op_stack.popDouble();
        local_vars.setDouble(3, value);
      } break;
      case ASTORE_0: {
        auto* value = op_stack.popRef();
        local_vars.setRef(0, value);
      } break;
      case ASTORE_1: {
        auto* value = op_stack.popRef();
        local_vars.setRef(1, value);
      } break;
      case ASTORE_2: {
        auto* value = op_stack.popRef();
        local_vars.setRef(2, value);
      } break;
      case ASTORE_3: {
        auto* value = op_stack.popRef();
        local_vars.setRef(3, value);
      } break;
      case IASTORE:
        // TODO: implement iastore
        break;
      case LASTORE:
        // TODO: implement lastore
        break;
      case FASTORE:
        // TODO: implement fastore
        break;
      case DASTORE:
        // TODO: implement dastore
        break;
      case AASTORE:
        // TODO: implement aastore
        break;
      case BASTORE:
        // TODO: implement bastore
        break;
      case CASTORE:
        // TODO: implement castore
        break;
      case SASTORE:
        // TODO: implement sastore
        break;
      /* #endregion Stores */

      /* #region Stack */

      // Function: Manipulate operand stack (pop, dup, swap operations)
      // Components: op_stack
      case POP: {
        op_stack.popSlot();  // Pop one word (int, float, or reference)
      } break;
      case POP2: {
        op_stack.popSlot();  // long & double emplace 2 slots
        op_stack.popSlot();  // Pop two words (long or double)
      } break;
      case DUP: {
        auto value = op_stack.popSlot();  // Pop one word
        op_stack.pushSlot(value);         // Push it back
        op_stack.pushSlot(value);         // Push it again (duplicate)
      } break;
      case DUP_X1: {
        // Duplicate the top value and insert it two slots down
        // Stack: ..., value2, value1 -> ..., value1, value2, value1
        auto value1 = op_stack.popSlot();  // Pop value1 (top)
        auto value2 = op_stack.popSlot();  // Pop value2
        op_stack.pushSlot(value1);         // Push value1 (duplicate)
        op_stack.pushSlot(value2);         // Push value2
        op_stack.pushSlot(value1);         // Push value1 (original)
      } break;
      case DUP_X2: {
        // Duplicate the top value and insert it three slots down
        // Stack: ..., value3, value2, value1 -> ..., value1, value3, value2, value1
        auto value1 = op_stack.popSlot();  // Pop value1 (top)
        auto value2 = op_stack.popSlot();  // Pop value2
        auto value3 = op_stack.popSlot();  // Pop value3
        op_stack.pushSlot(value1);         // Push value1 (duplicate)
        op_stack.pushSlot(value3);         // Push value3
        op_stack.pushSlot(value2);         // Push value2
        op_stack.pushSlot(value1);         // Push value1 (original)
      } break;
      case DUP2: {
        // Duplicate the top two values
        // Stack: ..., value2, value1 -> ..., value2, value1, value2, value1
        auto value1 = op_stack.popSlot();  // Pop value1 (top)
        auto value2 = op_stack.popSlot();  // Pop value2
        op_stack.pushSlot(value2);         // Push value2 (duplicate)
        op_stack.pushSlot(value1);         // Push value1 (duplicate)
        op_stack.pushSlot(value2);         // Push value2 (original)
        op_stack.pushSlot(value1);         // Push value1 (original)
      } break;
      case DUP2_X1: {
        // Duplicate the top two values and insert them three slots down
        // Stack: ..., value3, value2, value1 -> ..., value2, value1, value3, value2, value1
        auto value1 = op_stack.popSlot();  // Pop value1 (top)
        auto value2 = op_stack.popSlot();  // Pop value2
        auto value3 = op_stack.popSlot();  // Pop value3
        op_stack.pushSlot(value2);         // Push value2 (duplicate)
        op_stack.pushSlot(value1);         // Push value1 (duplicate)
        op_stack.pushSlot(value3);         // Push value3
        op_stack.pushSlot(value2);         // Push value2 (original)
        op_stack.pushSlot(value1);         // Push value1 (original)
      } break;
      case DUP2_X2: {
        // Duplicate the top two values and insert them four slots down
        // Stack: ..., value4, value3, value2, value1 -> ..., value2, value1, value4, value3,
        // value2, value1
        auto value1 = op_stack.popSlot();  // Pop value1 (top)
        auto value2 = op_stack.popSlot();  // Pop value2
        auto value3 = op_stack.popSlot();  // Pop value3
        auto value4 = op_stack.popSlot();  // Pop value4
        op_stack.pushSlot(value2);         // Push value2 (duplicate)
        op_stack.pushSlot(value1);         // Push value1 (duplicate)
        op_stack.pushSlot(value4);         // Push value4
        op_stack.pushSlot(value3);         // Push value3
        op_stack.pushSlot(value2);         // Push value2 (original)
        op_stack.pushSlot(value1);         // Push value1 (original)
      } break;
      case SWAP: {
        auto value1 = op_stack.popSlot();  // Pop first word
        auto value2 = op_stack.popSlot();  // Pop second word
        op_stack.pushSlot(value1);         // Push first word
        op_stack.pushSlot(value2);         // Push second word (now on top)
      } break;
      /* #endregion Stack */

      /* #region Arithmetic */

      // Function: Perform arithmetic operations on numeric values (add, subtract, multiply, divide,
      // remainder, negate, shift, bitwise) Components: op_stack
      case IADD: {
        auto value2 = op_stack.popInt();
        auto value1 = op_stack.popInt();
        op_stack.pushInt(value1 + value2);
      } break;
      case LADD: {
        auto value2 = op_stack.popLong();
        auto value1 = op_stack.popLong();
        op_stack.pushLong(value1 + value2);
      } break;
      case FADD: {
        auto value2 = op_stack.popFloat();
        auto value1 = op_stack.popFloat();
        op_stack.pushFloat(value1 + value2);
      } break;
      case DADD: {
        auto value2 = op_stack.popDouble();
        auto value1 = op_stack.popDouble();
        op_stack.pushDouble(value1 + value2);
      } break;
      case ISUB: {
        auto value2 = op_stack.popInt();
        auto value1 = op_stack.popInt();
        op_stack.pushInt(value1 - value2);
      } break;
      case LSUB: {
        auto value2 = op_stack.popLong();
        auto value1 = op_stack.popLong();
        op_stack.pushLong(value1 - value2);
      } break;
      case FSUB: {
        auto value2 = op_stack.popFloat();
        auto value1 = op_stack.popFloat();
        op_stack.pushFloat(value1 - value2);
      } break;
      case DSUB: {
        auto value2 = op_stack.popDouble();
        auto value1 = op_stack.popDouble();
        op_stack.pushDouble(value1 - value2);
      } break;
      case IMUL: {
        auto value2 = op_stack.popInt();
        auto value1 = op_stack.popInt();
        op_stack.pushInt(value1 * value2);
      } break;
      case LMUL: {
        auto value2 = op_stack.popLong();
        auto value1 = op_stack.popLong();
        op_stack.pushLong(value1 * value2);
      } break;
      case FMUL: {
        auto value2 = op_stack.popFloat();
        auto value1 = op_stack.popFloat();
        op_stack.pushFloat(value1 * value2);
      } break;
      case DMUL: {
        auto value2 = op_stack.popDouble();
        auto value1 = op_stack.popDouble();
        op_stack.pushDouble(value1 * value2);
      } break;
      case IDIV: {
        auto value2 = op_stack.popInt();
        auto value1 = op_stack.popInt();
        if (value2 == 0) {
          throw std::runtime_error("ArithmeticException: / by zero");
        }
        op_stack.pushInt(value1 / value2);
      } break;
      case LDIV: {
        auto value2 = op_stack.popLong();
        auto value1 = op_stack.popLong();
        if (value2 == 0) {
          throw std::runtime_error("ArithmeticException: / by zero");
        }
        op_stack.pushLong(value1 / value2);
      } break;
      case FDIV: {
        auto value2 = op_stack.popFloat();
        auto value1 = op_stack.popFloat();
        op_stack.pushFloat(value1 / value2);
      } break;
      case DDIV: {
        auto value2 = op_stack.popDouble();
        auto value1 = op_stack.popDouble();
        op_stack.pushDouble(value1 / value2);
      } break;
      case IREM: {
        auto value2 = op_stack.popInt();
        auto value1 = op_stack.popInt();
        if (value2 == 0) {
          throw std::runtime_error("ArithmeticException: / by zero");
        }
        op_stack.pushInt(value1 % value2);
      } break;
      case LREM: {
        auto value2 = op_stack.popLong();
        auto value1 = op_stack.popLong();
        if (value2 == 0) {
          throw std::runtime_error("ArithmeticException: / by zero");
        }
        op_stack.pushLong(value1 % value2);
      } break;
      case FREM: {
        auto value2 = op_stack.popFloat();
        auto value1 = op_stack.popFloat();
        op_stack.pushFloat(std::fmod(value1, value2));
      } break;
      case DREM: {
        auto value2 = op_stack.popDouble();
        auto value1 = op_stack.popDouble();
        op_stack.pushDouble(std::fmod(value1, value2));
      } break;
      case INEG: {
        auto value = op_stack.popInt();
        op_stack.pushInt(-value);
      } break;
      case LNEG: {
        auto value = op_stack.popLong();
        op_stack.pushLong(-value);
      } break;
      case FNEG: {
        auto value = op_stack.popFloat();
        op_stack.pushFloat(-value);
      } break;
      case DNEG: {
        auto value = op_stack.popDouble();
        op_stack.pushDouble(-value);
      } break;
      case ISHL: {
        // Integer shift left: value1 << (value2 & 0x1f)
        auto shift_count = static_cast<U4>(op_stack.popInt()) & 0x1FU;  // Only use lower 5 bits
        auto value       = static_cast<U4>(op_stack.popInt());
        op_stack.pushInt(static_cast<Jint>(value << shift_count));
      } break;
      case LSHL: {
        // Long shift left: value1 << (value2 & 0x3f)
        auto shift_count = static_cast<U4>(op_stack.popInt()) & 0x3FU;  // Only use lower 6 bits
        auto value       = static_cast<U8>(op_stack.popLong());
        op_stack.pushLong(static_cast<Jlong>(value << shift_count));
      } break;
      case ISHR: {
        // Integer arithmetic shift right: value1 >> (value2 & 0x1f)
        auto shift_count = static_cast<U4>(op_stack.popInt()) & 0x1FU;  // Only use lower 5 bits
        auto value       = op_stack.popInt();
        // NOLINTNEXTLINE(hicpp-signed-bitwise) yes we want to shift the sign bit
        op_stack.pushInt(value >> shift_count);
      } break;
      case LSHR: {
        // Long arithmetic shift right: value1 >> (value2 & 0x3f)
        auto shift_count = static_cast<U4>(op_stack.popInt()) & 0x3FU;  // Only use lower 6 bits
        auto value       = op_stack.popLong();
        // NOLINTNEXTLINE(hicpp-signed-bitwise) yes we want to shift the sign bit
        op_stack.pushLong(value >> shift_count);
      } break;
      case IUSHR: {
        // Integer logical shift right: (unsigned)value1 >>> (value2 & 0x1f)
        auto shift_count = static_cast<U4>(op_stack.popInt()) & 0x1FU;  // Only use lower 5 bits
        auto value       = static_cast<U4>(op_stack.popInt());
        op_stack.pushInt(static_cast<Jint>(value >> shift_count));
      } break;
      case LUSHR: {
        // Long logical shift right: (unsigned)value1 >>> (value2 & 0x3f)
        auto shift_count = static_cast<U4>(op_stack.popInt()) & 0x3FU;  // Only use lower 6 bits
        auto value       = static_cast<U8>(op_stack.popLong());
        op_stack.pushLong(static_cast<Jlong>(value >> shift_count));
      } break;
      case IAND: {
        // Integer bitwise AND
        auto value2 = static_cast<U4>(op_stack.popInt());
        auto value1 = static_cast<U4>(op_stack.popInt());
        op_stack.pushInt(static_cast<Jint>(value1 & value2));
      } break;
      case LAND: {
        // Long bitwise AND
        auto value2 = static_cast<U8>(op_stack.popLong());
        auto value1 = static_cast<U8>(op_stack.popLong());
        op_stack.pushLong(static_cast<Jlong>(value1 & value2));
      } break;
      case IOR: {
        // Integer bitwise OR
        auto value2 = static_cast<U4>(op_stack.popInt());
        auto value1 = static_cast<U4>(op_stack.popInt());
        op_stack.pushInt(static_cast<Jint>(value1 | value2));
      } break;
      case LOR: {
        // Long bitwise OR
        auto value2 = static_cast<U8>(op_stack.popLong());
        auto value1 = static_cast<U8>(op_stack.popLong());
        op_stack.pushLong(static_cast<Jlong>(value1 | value2));
      } break;
      case IXOR: {
        // Integer bitwise XOR
        auto value2 = static_cast<U4>(op_stack.popInt());
        auto value1 = static_cast<U4>(op_stack.popInt());
        op_stack.pushInt(static_cast<Jint>(value1 ^ value2));
      } break;
      case LXOR: {
        // Long bitwise XOR
        auto value2 = static_cast<U8>(op_stack.popLong());
        auto value1 = static_cast<U8>(op_stack.popLong());
        op_stack.pushLong(static_cast<Jlong>(value1 ^ value2));
      } break;
      /* #endregion Arithmetic */

      /* #region IINC */

      // Function: Increment local variable by an immediate value
      // Components: local_vars, thread (PC)
      case IINC: {
        auto index         = reader.readU1();
        auto const_val     = reader.readSU1();
        auto current_value = local_vars.getInt(index);
        local_vars.setInt(index, current_value + const_val);
      } break;
      /* #endregion IINC */

      /* #region Conversions */

      // Function: Convert between different numeric types
      // Components: op_stack
      case I2L: {
        // Convert int to long
        auto value = op_stack.popInt();
        op_stack.pushLong(static_cast<Jlong>(value));
      } break;
      case I2F: {
        // Convert int to float
        auto value = op_stack.popInt();
        op_stack.pushFloat(static_cast<Jfloat>(value));
      } break;
      case I2D: {
        // Convert int to double
        auto value = op_stack.popInt();
        op_stack.pushDouble(static_cast<Jdouble>(value));
      } break;
      case L2I: {
        // Convert long to int (truncate)
        auto value = op_stack.popLong();
        op_stack.pushInt(static_cast<Jint>(value));
      } break;
      case L2F: {
        // Convert long to float
        auto value = op_stack.popLong();
        op_stack.pushFloat(static_cast<Jfloat>(value));
      } break;
      case L2D: {
        // Convert long to double
        auto value = op_stack.popLong();
        op_stack.pushDouble(static_cast<Jdouble>(value));
      } break;
      case F2I: {
        // Convert float to int (truncate towards zero)
        auto value = op_stack.popFloat();
        if (std::isnan(value) || std::isinf(value)) {
          op_stack.pushInt(0);
        } else {
          op_stack.pushInt(static_cast<Jint>(value));
        }
      } break;
      case F2L: {
        // Convert float to long (truncate towards zero)
        auto value = op_stack.popFloat();
        if (std::isnan(value) || std::isinf(value)) {
          op_stack.pushLong(0);
        } else {
          op_stack.pushLong(static_cast<Jlong>(value));
        }
      } break;
      case F2D: {
        // Convert float to double
        auto value = op_stack.popFloat();
        op_stack.pushDouble(static_cast<Jdouble>(value));
      } break;
      case D2I: {
        // Convert double to int (truncate towards zero)
        auto value = op_stack.popDouble();
        if (std::isnan(value) || std::isinf(value)) {
          op_stack.pushInt(0);
        } else {
          op_stack.pushInt(static_cast<Jint>(value));
        }
      } break;
      case D2L: {
        // Convert double to long (truncate towards zero)
        auto value = op_stack.popDouble();
        if (std::isnan(value) || std::isinf(value)) {
          op_stack.pushLong(0);
        } else {
          op_stack.pushLong(static_cast<Jlong>(value));
        }
      } break;
      case D2F: {
        // Convert double to float
        auto value = op_stack.popDouble();
        op_stack.pushFloat(static_cast<Jfloat>(value));
      } break;
      case I2B: {
        // Convert int to byte (sign extend)
        auto value = op_stack.popInt();
        op_stack.pushInt(static_cast<Jint>(static_cast<Jbyte>(value)));
      } break;
      case I2C: {
        // Convert int to char (zero extend)
        auto value = op_stack.popInt();
        op_stack.pushInt(static_cast<Jint>(static_cast<Jchar>(value)));
      } break;
      case I2S: {
        // Convert int to short (sign extend)
        auto value = op_stack.popInt();
        op_stack.pushInt(static_cast<Jint>(static_cast<Jshort>(value)));
      } break;
      /* #endregion Conversions */

      /* #region Comparisons */

      // Function: Compare values and perform conditional branches
      // Components: op_stack, thread (PC)
      case LCMP: {
        // Compare two longs: value1 - value2
        auto value2 = op_stack.popLong();
        auto value1 = op_stack.popLong();
        if (value1 > value2) {
          op_stack.pushInt(1);
        } else if (value1 < value2) {
          op_stack.pushInt(-1);
        } else {
          op_stack.pushInt(0);
        }
      } break;
      case FCMPL: {
        // Compare two floats, return -1 if either is NaN
        auto value2 = op_stack.popFloat();
        auto value1 = op_stack.popFloat();
        if (std::isnan(value1) || std::isnan(value2)) {
          op_stack.pushInt(-1);
        } else if (value1 > value2) {
          op_stack.pushInt(1);
        } else if (value1 < value2) {
          op_stack.pushInt(-1);
        } else {
          op_stack.pushInt(0);
        }
      } break;
      case FCMPG: {
        // Compare two floats, return 1 if either is NaN
        auto value2 = op_stack.popFloat();
        auto value1 = op_stack.popFloat();
        if (std::isnan(value1) || std::isnan(value2)) {
          op_stack.pushInt(1);
        } else if (value1 > value2) {
          op_stack.pushInt(1);
        } else if (value1 < value2) {
          op_stack.pushInt(-1);
        } else {
          op_stack.pushInt(0);
        }
      } break;
      case DCMPL: {
        // Compare two doubles, return -1 if either is NaN
        auto value2 = op_stack.popDouble();
        auto value1 = op_stack.popDouble();
        if (std::isnan(value1) || std::isnan(value2)) {
          op_stack.pushInt(-1);
        } else if (value1 > value2) {
          op_stack.pushInt(1);
        } else if (value1 < value2) {
          op_stack.pushInt(-1);
        } else {
          op_stack.pushInt(0);
        }
      } break;
      case DCMPG: {
        // Compare two doubles, return 1 if either is NaN
        auto value2 = op_stack.popDouble();
        auto value1 = op_stack.popDouble();
        if (std::isnan(value1) || std::isnan(value2)) {
          op_stack.pushInt(1);
        } else if (value1 > value2) {
          op_stack.pushInt(1);
        } else if (value1 < value2) {
          op_stack.pushInt(-1);
        } else {
          op_stack.pushInt(0);
        }
      } break;
      case IFEQ: {
        // Branch if int value equals 0
        auto base_addr     = pc - 1;
        auto value         = op_stack.popInt();
        auto branch_offset = static_cast<size_t>(reader.readSU2());
        if (value == 0) {
          pc = base_addr + branch_offset;
        }
      } break;
      case IFNE: {
        // Branch if int value not equal to 0
        auto base_addr     = pc - 1;
        auto value         = op_stack.popInt();
        auto branch_offset = static_cast<size_t>(reader.readSU2());
        if (value != 0) {
          pc = base_addr + branch_offset;
        }
      } break;
      case IFLT: {
        // Branch if int value less than 0
        auto base_addr     = pc - 1;
        auto value         = op_stack.popInt();
        auto branch_offset = static_cast<size_t>(reader.readSU2());
        if (value < 0) {
          pc = base_addr + branch_offset;
        }
      } break;
      case IFGE: {
        // Branch if int value greater than or equal to 0
        auto base_addr     = pc - 1;
        auto value         = op_stack.popInt();
        auto branch_offset = static_cast<size_t>(reader.readSU2());
        if (value >= 0) {
          pc = base_addr + branch_offset;
        }
      } break;
      case IFGT: {
        // Branch if int value greater than 0
        auto base_addr     = pc - 1;
        auto value         = op_stack.popInt();
        auto branch_offset = static_cast<size_t>(reader.readSU2());
        if (value > 0) {
          pc = base_addr + branch_offset;
        }
      } break;
      case IFLE: {
        // Branch if int value less than or equal to 0
        auto base_addr     = pc - 1;
        auto value         = op_stack.popInt();
        auto branch_offset = static_cast<size_t>(reader.readSU2());
        if (value <= 0) {
          pc = base_addr + branch_offset;
        }
      } break;
      case IF_ICMPEQ: {
        // Branch if two int values are equal
        auto base_addr     = pc - 1;
        auto value2        = op_stack.popInt();
        auto value1        = op_stack.popInt();
        auto branch_offset = static_cast<size_t>(reader.readSU2());
        if (value1 == value2) {
          pc = base_addr + branch_offset;
        }
      } break;
      case IF_ICMPNE: {
        // Branch if two int values are not equal
        auto base_addr     = pc - 1;
        auto value2        = op_stack.popInt();
        auto value1        = op_stack.popInt();
        auto branch_offset = static_cast<size_t>(reader.readSU2());
        if (value1 != value2) {
          pc = base_addr + branch_offset;
        }
      } break;
      case IF_ICMPLT: {
        // Branch if first int value less than second
        auto base_addr     = pc - 1;
        auto value2        = op_stack.popInt();
        auto value1        = op_stack.popInt();
        auto branch_offset = static_cast<size_t>(reader.readSU2());
        if (value1 < value2) {
          pc = base_addr + branch_offset;
        }
      } break;
      case IF_ICMPGE: {
        // Branch if first int value greater than or equal to second
        auto base_addr     = pc - 1;
        auto value2        = op_stack.popInt();
        auto value1        = op_stack.popInt();
        auto branch_offset = static_cast<size_t>(reader.readSU2());
        if (value1 >= value2) {
          pc = base_addr + branch_offset;
        }
      } break;
      case IF_ICMPGT: {
        // Branch if first int value greater than second
        auto base_addr     = pc - 1;
        auto value2        = op_stack.popInt();
        auto value1        = op_stack.popInt();
        auto branch_offset = static_cast<size_t>(reader.readSU2());
        if (value1 > value2) {
          pc = base_addr + branch_offset;
        }
      } break;
      case IF_ICMPLE: {
        // Branch if first int value less than or equal to second
        auto base_addr     = pc - 1;
        auto value2        = op_stack.popInt();
        auto value1        = op_stack.popInt();
        auto branch_offset = static_cast<size_t>(reader.readSU2());
        if (value1 <= value2) {
          pc = base_addr + branch_offset;
        }
      } break;
      case IF_ACMPEQ: {
        // Branch if two reference values are equal
        auto  base_addr     = pc - 1;
        auto* value2        = op_stack.popRef();
        auto* value1        = op_stack.popRef();
        auto  branch_offset = static_cast<size_t>(reader.readSU2());
        if (value1 == value2) {
          pc = base_addr + branch_offset;
        }
      } break;
      case IF_ACMPNE: {
        // Branch if two reference values are not equal
        auto  base_addr     = pc - 1;
        auto* value2        = op_stack.popRef();
        auto* value1        = op_stack.popRef();
        auto  branch_offset = static_cast<size_t>(reader.readSU2());
        if (value1 != value2) {
          pc = base_addr + branch_offset;
        }
      } break;
      case IFNULL: {
        // Branch if reference value is null
        auto  base_addr     = pc - 1;
        auto* value         = op_stack.popRef();
        auto  branch_offset = static_cast<size_t>(reader.readSU2());
        if (value == nullptr) {
          pc = base_addr + branch_offset;
        }
      } break;
      case IFNONNULL: {
        // Branch if reference value is not null
        auto  base_addr     = pc - 1;
        auto* value         = op_stack.popRef();
        auto  branch_offset = static_cast<size_t>(reader.readSU2());
        if (value != nullptr) {
          pc = base_addr + branch_offset;
        }
      } break;
      /* #endregion Comparisons */

      /* #region Control flow */

      // Function: Unconditional branches and switch statements
      // Components: thread (PC)
      case GOTO: {
        auto base_addr     = pc - 1;
        auto branch_offset = static_cast<size_t>(reader.readSU2());
        pc                 = base_addr + branch_offset;
      } break;
      case GOTO_W: {
        auto base_addr     = pc - 1;
        auto branch_offset = static_cast<size_t>(reader.readSU4());
        pc                 = base_addr + branch_offset;
      } break;
      case JSR:
        // not used in Java SE 8
        break;
      case JSR_W:
        // not used in Java SE 8
        break;
      case RET:
        // not used in Java SE 8
        break;
      case TABLESWITCH: {
        auto base_addr = pc - 1;
        // skip padding to make sure the defaultOffset' address in bytecode is always 4-byte aligned
        reader.align4();
        // defaultOffset, signed
        auto default_offset = static_cast<size_t>(reader.readSU4());
        // low, signed
        auto low  = reader.readSU4();
        auto high = reader.readSU4();
        // jump_offsets
        auto range        = static_cast<size_t>(high) - static_cast<size_t>(low) + 1;
        auto jump_offsets = std::vector<Jint>(range);
        for (size_t i = 0; i < jump_offsets.size(); i++) {
          jump_offsets[i] = reader.readSU4();
        }
        // pop index from operand stack
        auto index = op_stack.popInt();
        if (index < low || index > high) {
          pc = base_addr + default_offset;
        } else {
          pc = base_addr + static_cast<size_t>(jump_offsets[static_cast<size_t>(index - low)]);
        }
      } break;
      case LOOKUPSWITCH: {
        auto base_addr = pc - 1;
        // skip padding to make sure 4-byte alignment
        reader.align4();
        auto default_bytes = static_cast<size_t>(reader.readSU4());
        auto npairs_count  = static_cast<size_t>(reader.readSU4());
        auto jump_offsets  = std::vector<std::pair<Jint, Jint>>(npairs_count);
        for (size_t i = 0; i < jump_offsets.size(); i++) {
          jump_offsets[i].first  = reader.readSU4();
          jump_offsets[i].second = reader.readSU4();
        }
        // pop key from operand stack
        auto key   = op_stack.popInt();
        bool found = false;
        for (size_t i = 0; i < jump_offsets.size(); i++) {
          if (key == jump_offsets[i].first) {
            pc    = base_addr + static_cast<size_t>(jump_offsets[i].second);
            found = true;
            break;
          }
        }
        if (!found) {
          pc = base_addr + default_bytes;
        }
      } break;

      /* #endregion Control flow */

      /* #region Returns */

      // Function: Return from method
      // Components: thread, op_stack
      case IRETURN: {
        auto& callee_frame = thread->getCurrentFrame();
        Jint  ret          = callee_frame.getOperandStack().popInt();
        LOG_DEBUG("return from ", callee_frame.getMethod()->getName(), " = ", ret);
        thread->popFrame();
        if (!thread->isStackEmpty()) {
          // push ret into caller frame's operand stack
          auto& caller_frame = thread->getCurrentFrame();
          caller_frame.getOperandStack().pushInt(ret);
          pc = caller_frame.getPC();
        }
      } break;
      case LRETURN: {
        auto& callee_frame = thread->getCurrentFrame();
        Jlong ret          = callee_frame.getOperandStack().popLong();
        LOG_DEBUG("return from ", callee_frame.getMethod()->getName(), " = ", ret);
        thread->popFrame();
        if (!thread->isStackEmpty()) {
          auto& caller_frame = thread->getCurrentFrame();
          caller_frame.getOperandStack().pushLong(ret);
          pc = caller_frame.getPC();
        }
      } break;
      case FRETURN: {
        auto&  callee_frame = thread->getCurrentFrame();
        Jfloat ret          = callee_frame.getOperandStack().popFloat();
        LOG_DEBUG("return from ", callee_frame.getMethod()->getName(), " = ", ret);
        thread->popFrame();
        if (!thread->isStackEmpty()) {
          auto& caller_frame = thread->getCurrentFrame();
          caller_frame.getOperandStack().pushFloat(ret);
          pc = caller_frame.getPC();
        }
      } break;
      case DRETURN: {
        auto&   callee_frame = thread->getCurrentFrame();
        Jdouble ret          = callee_frame.getOperandStack().popDouble();
        LOG_DEBUG("return from ", callee_frame.getMethod()->getName(), " = ", ret);
        thread->popFrame();
        if (!thread->isStackEmpty()) {
          auto& caller_frame = thread->getCurrentFrame();
          caller_frame.getOperandStack().pushDouble(ret);
          pc = caller_frame.getPC();
        }
      } break;
      case ARETURN: {
        auto& callee_frame = thread->getCurrentFrame();
        Jref  ret          = callee_frame.getOperandStack().popRef();
        LOG_DEBUG("return from ", callee_frame.getMethod()->getName(), " ref=", ret);
        thread->popFrame();
        if (!thread->isStackEmpty()) {
          auto& caller_frame = thread->getCurrentFrame();
          caller_frame.getOperandStack().pushRef(ret);
          pc = caller_frame.getPC();
        }
      } break;
      case RETURN: {
        LOG_DEBUG("return from ", method->getName());
        // If the returned method was <clinit>, mark the class as fully initialized
        if (method->getName() == "<clinit>") {
          method->getOwnerKlass()->markFullyInitialized();
        }
        thread->popFrame();
        if (!thread->isStackEmpty()) {
          pc = thread->getCurrentFrame().getPC();
        } else {
          return;
        }
      } break;
      /* #endregion Returns */

      /* #region Fields */

      // Function: Access static and instance fields
      // Components: rt_cp, op_stack, thread (PC)
      case GETSTATIC: {
        auto index = reader.readU2();

        if (tryStubIntercept(rt_cp, index, op_stack)) {
          break;
        }

        auto* field = rt_cp.resolveField(index);
        auto* klass = field->getOwnerKlass();
        if (klass->getState() == oops::InstanceKlass::Linked) {
          thread->getCurrentFrame().setPC(pc - 3);
          klass->initialize(thread);
          pc = 0;
          break;
        }
        auto& slot      = klass->getStaticSlot(field->getSlotIndex());
        auto  signature = field->getSignature();
        descriptor::isCategory2(signature) ? op_stack.pushWide(slot) : op_stack.pushSlot(slot);
      } break;
      case PUTSTATIC: {
        auto  index = reader.readU2();
        auto* field = rt_cp.resolveField(index);
        auto* klass = field->getOwnerKlass();
        if (klass->getState() == oops::InstanceKlass::Linked) {
          thread->getCurrentFrame().setPC(pc - 3);
          klass->initialize(thread);
          pc = 0;
          break;
        }
        auto& slot      = klass->getStaticSlot(field->getSlotIndex());
        auto  signature = field->getSignature();
        slot = descriptor::isCategory2(signature) ? op_stack.popWide() : op_stack.popSlot();
      } break;
      case GETFIELD: {
        auto  index = reader.readU2();
        auto* field = rt_cp.resolveField(index);
        auto* obj   = static_cast<oops::InstanceOopDesc*>(op_stack.popRef());
        if (obj == nullptr) {
          throw std::runtime_error("NPE");
        }
        auto& slot      = obj->fieldSlot(field->getSlotIndex());
        auto  signature = field->getSignature();
        descriptor::isCategory2(signature) ? op_stack.pushWide(slot) : op_stack.pushSlot(slot);
      } break;
      case PUTFIELD: {
        auto  index     = reader.readU2();
        auto* field     = rt_cp.resolveField(index);
        auto  signature = field->getSignature();
        // stack: ..., objectref, value  (value on top) -> pop value first, then objectref
        Slot  value = descriptor::isCategory2(signature) ? op_stack.popWide() : op_stack.popSlot();
        auto* obj   = static_cast<oops::InstanceOopDesc*>(op_stack.popRef());
        if (obj == nullptr) {
          throw std::runtime_error("NPE");
        }
        obj->fieldSlot(field->getSlotIndex()) = value;
      } break;
      /* #endregion Fields */

      /* #region Methods */

      // Function: Invoke methods
      // Components: rt_cp, thread (PC), op_stack
      case INVOKEVIRTUAL: {
        auto index = reader.readU2();

        if (tryStubIntercept(rt_cp, index, op_stack)) {
          LOG_DEBUG("Stub intercepted INVOKEVIRTUAL at index=", index);
          break;
        }

        auto* resolved = rt_cp.resolveMethod(index);
        LOG_DEBUG("INVOKEVIRTUAL ", resolved->getOwnerKlass()->getName(), ".", resolved->getName(),
                  ".", resolved->getDescriptor());
        auto* recv = static_cast<oops::InstanceOopDesc*>(
          op_stack.peekRef(resolved->getSignature().arg_slot_count));

        if (recv == nullptr) {
          throw std::runtime_error("NPE");
        }
        oops::InstanceKlass* recv_klass = recv->getKlass();

        oops::Method* actual =
          recv_klass->findMethod(resolved->getName(), resolved->getDescriptor());
        if (actual == nullptr) {
          throw std::runtime_error("NPE");
        }

        const auto&    signature = actual->getSignature();
        runtime::Frame next_frame(actual);
        auto&          current_op_stack = op_stack;  // current frame's operand stack
        auto&          next_local_vars  = next_frame.getLocalVariables();

        U2 pos = signature.arg_slot_count + 1;
        for (int p = static_cast<int>(signature.params.size()) - 1; p >= 0; --p) {
          auto idx = static_cast<size_t>(p);
          if (descriptor::isCategory2(signature.params[idx])) {
            pos -= 2;
            next_local_vars.setWide(pos, current_op_stack.popWide());
          } else {
            pos -= 1;
            next_local_vars.setSlot(pos, current_op_stack.popSlot());
          }
        }
        next_local_vars.setSlot(0, op_stack.popSlot());

        thread->getCurrentFrame().setPC(pc);
        thread->pushFrame(std::move(next_frame));
        pc = 0;
      } break;
      case INVOKESPECIAL: {
        auto index = reader.readU2();

        if (tryStubIntercept(rt_cp, index, op_stack)) {
          break;
        }

        auto* new_method = rt_cp.resolveMethod(index);
        LOG_DEBUG("INVOKESPECIAL ", new_method->getOwnerKlass()->getName(), ".",
                  new_method->getName(), ".", new_method->getDescriptor());

        const auto&    signature = new_method->getSignature();
        runtime::Frame next_frame(new_method);
        auto&          current_op_stack = op_stack;  // current frame's operand stack
        auto&          next_local_vars  = next_frame.getLocalVariables();

        U2 pos = signature.arg_slot_count + 1;
        for (int p = static_cast<int>(signature.params.size()) - 1; p >= 0; --p) {
          auto idx = static_cast<size_t>(p);
          if (descriptor::isCategory2(signature.params[idx])) {
            pos -= 2;
            next_local_vars.setWide(pos, current_op_stack.popWide());
          } else {
            pos -= 1;
            next_local_vars.setSlot(pos, current_op_stack.popSlot());
          }
        }
        next_local_vars.setSlot(0, op_stack.popSlot());

        thread->getCurrentFrame().setPC(pc);
        thread->pushFrame(std::move(next_frame));
        pc = 0;
      } break;
      case INVOKESTATIC: {
        // calling static method
        auto index = reader.readU2();

        if (tryStubIntercept(rt_cp, index, op_stack)) {
          LOG_DEBUG("Stub intercepted INVOKESTATIC at index=", index);
          break;
        }

        auto* new_method = rt_cp.resolveMethod(index);
        LOG_DEBUG("INVOKESTATIC ", new_method->getOwnerKlass()->getName(), ".",
                  new_method->getName(), ".", new_method->getDescriptor());

        // if the class is not initialied, initialize it
        auto* klass = new_method->getOwnerKlass();
        if (klass->getState() == oops::InstanceKlass::Linked) {
          thread->getCurrentFrame().setPC(pc - 3);
          klass->initialize(thread);
          pc = 0;
          break;
        }

        if (!new_method->isStatic()) {
          throw std::runtime_error("Cannot invoke non-static method as static");
        }

        if (new_method->isNative()) {
          auto key = new_method->getOwnerKlass()->getName() + "." + new_method->getName() + "." +
                     new_method->getDescriptor();
          auto fn  = NativeRegistry::getSingleton().find(key);
          if (fn == nullptr) {
            throw std::runtime_error("unbound native: " + key);
          }
          fn(op_stack);
          break;
        }

        const auto&    signature = new_method->getSignature();
        runtime::Frame next_frame(new_method);
        auto&          current_op_stack = op_stack;  // current frame's operand stack
        auto&          next_local_vars  = next_frame.getLocalVariables();

        U2 pos = signature.arg_slot_count;
        for (int p = static_cast<int>(signature.params.size()) - 1; p >= 0; --p) {
          auto idx = static_cast<size_t>(p);
          if (descriptor::isCategory2(signature.params[idx])) {
            pos -= 2;
            next_local_vars.setWide(pos, current_op_stack.popWide());
          } else {
            pos -= 1;
            next_local_vars.setSlot(pos, current_op_stack.popSlot());
          }
        }

        thread->getCurrentFrame().setPC(pc);
        thread->pushFrame(std::move(next_frame));
        // reset pc to 0 for the next frame
        pc = 0;

      } break;
      case INVOKEINTERFACE: {
        auto index = reader.readU2();
        reader.readU1();  // skip count
        reader.readU1();  // must be 0

        if (tryStubIntercept(rt_cp, index, op_stack)) {
          LOG_DEBUG("Stub intercepted INVOKEINTERFACE at index=", index);
          break;
        }

        auto* resolved = rt_cp.resolveMethod(index);
        LOG_DEBUG("INVOKEINTERFACE ", resolved->getOwnerKlass()->getName(), ".",
                  resolved->getName(), ".", resolved->getDescriptor());
        auto* recv = static_cast<oops::InstanceOopDesc*>(
          op_stack.peekRef(resolved->getSignature().arg_slot_count));

        if (recv == nullptr) {
          throw std::runtime_error("NPE");
        }
        oops::InstanceKlass* recv_klass = recv->getKlass();

        oops::Method* actual =
          recv_klass->findMethod(resolved->getName(), resolved->getDescriptor());
        if (actual == nullptr) {
          throw std::runtime_error("NPE");
        }

        const auto&    signature = actual->getSignature();
        runtime::Frame next_frame(actual);
        auto&          current_op_stack = op_stack;  // current frame's operand stack
        auto&          next_local_vars  = next_frame.getLocalVariables();

        U2 pos = signature.arg_slot_count + 1;
        for (int p = static_cast<int>(signature.params.size()) - 1; p >= 0; --p) {
          auto idx = static_cast<size_t>(p);
          if (descriptor::isCategory2(signature.params[idx])) {
            pos -= 2;
            next_local_vars.setWide(pos, current_op_stack.popWide());
          } else {
            pos -= 1;
            next_local_vars.setSlot(pos, current_op_stack.popSlot());
          }
        }
        next_local_vars.setSlot(0, op_stack.popSlot());

        thread->getCurrentFrame().setPC(pc);
        thread->pushFrame(std::move(next_frame));
        pc = 0;
      } break;
      case INVOKEDYNAMIC:
        // TODO: implement invokedynamic
        break;
      /* #endregion Methods */

      /* #region Objects */

      // Function: Object creation and type checking
      // Components: rt_cp, op_stack, thread (PC)
      case NEW: {
        auto                 index = reader.readU2();
        oops::InstanceKlass* klass = rt_cp.resolveClass(index);
        if (klass->getState() == oops::InstanceKlass::Linked) {
          thread->getCurrentFrame().setPC(pc - 3);
          klass->initialize(thread);
          pc = 0;
          break;
        }
        Jref obj_ref = memory::Heap::getSingleton().newInstance(klass);
        op_stack.pushRef(obj_ref);
      } break;

      case CHECKCAST: {
        auto  index = reader.readU2();
        auto* obj   = static_cast<oops::InstanceOopDesc*>(op_stack.popRef());
        if (obj == nullptr) {
          op_stack.pushRef(nullptr);
          break;
        }
        auto* klass = rt_cp.resolveClass(index);
        if (!obj->getKlass()->isInstanceOf(klass)) {
          throw std::runtime_error("ClassCastException");
        }
        op_stack.pushRef(obj);
      } break;

      case INSTANCEOF: {
        auto  index = reader.readU2();
        auto* obj   = static_cast<oops::InstanceOopDesc*>(op_stack.popRef());
        if (obj == nullptr) {
          op_stack.pushInt(0);
          break;
        }
        auto* klass = rt_cp.resolveClass(index);
        op_stack.pushInt(obj->getKlass()->isInstanceOf(klass) ? 1 : 0);
      } break;
      /* #endregion Objects */

      /* #region Exceptions */

      // Function: Exception handling
      // Components: op_stack
      case ATHROW:
        // TODO: implement athrow
        break;
      /* #endregion Exceptions */

      /* #region Monitors */

      // Function: Synchronization operations
      // Components: op_stack
      case MONITORENTER:
        // TODO: implement monitorenter
        break;
      case MONITOREXIT:
        // TODO: implement monitorexit
        break;
      /* #endregion Monitors */

      /* #region Arrays */

      // Function: Array operations (create arrays, get array length)
      // Components: op_stack, rt_cp, thread (PC)
      case NEWARRAY:
        // TODO: implement newarray
        break;
      case ANEWARRAY:
        // TODO: implement anewarray
        break;
      case ARRAYLENGTH:
        // TODO: implement arraylength
        break;
      case MULTIANEWARRAY:
        // TODO: implement multianewarray
        break;
        /* #endregion Arrays */

      case WIDE: {
        auto widened_opcode = reader.readU1();
        auto wide_index     = reader.readU2();
        switch (widened_opcode) {
          case ILOAD: {
            op_stack.pushInt(local_vars.getInt(wide_index));
            break;
          }
          case LLOAD: {
            op_stack.pushLong(local_vars.getLong(wide_index));
            break;
          }
          case FLOAD: {
            op_stack.pushFloat(local_vars.getFloat(wide_index));
            break;
          }
          case DLOAD: {
            op_stack.pushDouble(local_vars.getDouble(wide_index));
            break;
          }
          case ALOAD: {
            op_stack.pushRef(local_vars.getRef(wide_index));
            break;
          }
          case ISTORE: {
            local_vars.setInt(wide_index, op_stack.popInt());
            break;
          }
          case LSTORE: {
            local_vars.setLong(wide_index, op_stack.popLong());
            break;
          }
          case FSTORE: {
            local_vars.setFloat(wide_index, op_stack.popFloat());
            break;
          }
          case DSTORE: {
            local_vars.setDouble(wide_index, op_stack.popDouble());
            break;
          }
          case ASTORE: {
            local_vars.setRef(wide_index, op_stack.popRef());
            break;
          }
          case IINC: {
            auto const_val = reader.readSU2();
            auto current   = local_vars.getInt(wide_index);
            local_vars.setInt(wide_index, current + const_val);
            break;
          }
          case RET: { /* RET with wide index – unused in Java 8 */
            break;
          }
          default:
            throw std::runtime_error("Unsupported widened opcode: " +
                                     std::to_string(widened_opcode));
        }
      } break;

      default:
        throw std::runtime_error("Invalid opcode: " + std::to_string(opcode));
    }
    // NOLINTEND(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
    // NOLINTEND(bugprone-branch-clone)
  }
}

}  // namespace jvm::engine