
#include "interpreter.h"

#include <bit>
#include <cmath>

#include "common/types.h"
#include "opcode.h"
#include "runtime/frame.h"
#include "runtime/klass.h"
#include "runtime/method.h"
#include "runtime/thread.h"

namespace {
constexpr jvm::Jshort combine_bytes(jvm::U1 b1, jvm::U1 b2) {
  // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
  return static_cast<jvm::Jshort>((0xFFU & b1) << 8U | (0xFFU & b2));
}

constexpr jvm::Jint combine_bytes(jvm::U1 b1, jvm::U1 b2, jvm::U1 b3, jvm::U1 b4) {
  // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
  return static_cast<jvm::Jint>((0xFFU & b1) << 24U | (0xFFU & b2) << 16U | (0xFFU & b3) << 8U |
                                (0xFFU & b4));
}

jvm::U2 calculateArgSlotCount(const std::string& descriptor) {
  jvm::U2 slot_count = 0;
  for (size_t i = 1; i < descriptor.length(); ++i) {
    char c = descriptor[i];
    if (c == ')') {
      break;
    }
    if (c == 'L') {
      // object reference: Ljava/lang/String;
      // skip until ';'
      while (descriptor[i] != ';') i++;
      slot_count++;
    } else if (c == '[') {
      // array: [[I or [Ljava/lang/String;
      // array reference only takes 1 slot, skip all '['
      while (descriptor[i + 1] == '[') i++;
      if (descriptor[i + 1] == 'L') {
        i++;
        while (descriptor[i] != ';') i++;
      } else {
        i++;  // skip basic type characters
      }
      slot_count++;
    } else if (c == 'J' || c == 'D') {
      // long or double takes 2 slots
      slot_count += 2;
    } else {
      // other basic types (I, F, B, C, S, Z) take 1 slot
      slot_count++;
    }
  }
  return slot_count;
}
}  // namespace

namespace jvm::engine {

void Interpreter::interpret(runtime::Thread* thread) {
  while (true) {
    if (thread->isStackEmpty()) {
      return;
    }

    auto& frame      = thread->getCurrentFrame();
    auto* method     = frame.getMethod();
    auto& local_vars = frame.getLocalVariables();
    auto& op_stack   = frame.getOperandStack();
    auto& rt_cp      = method->getOwnerKlass()->getRuntimeConstantPool();
    auto  code       = method->getCode();

    size_t pc = thread->getPC();
    if (pc >= code.size()) {
      // PC is beyond code length, method has finished executing
      // This typically means the method ended without an explicit RETURN
      // In a real JVM, this would be an error, but for testing we'll just pop the frame
      // If there's a return value on the stack, it means IRETURN/LRETURN/etc
      // just executed - return now so the test helper can read the value
      return;
    }

    // fetch
    auto opcode = code[pc];
    thread->incrementPC();
    pc = thread->getPC();

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
        // byte integer push
        op_stack.pushInt(code[pc]);
        thread->incrementPC();
      } break;
      case SIPUSH: {
        // short integer push
        auto value = combine_bytes(code[pc], code[pc + 1]);
        op_stack.pushInt(value);
        thread->incrementPC();
        thread->incrementPC();
      } break;
      /* #endregion Push immediate values */

      /* #region Push from constant pool */

      // Function: Load constants from runtime constant pool onto operand stack
      // Components: rt_cp, op_stack, thread (PC)
      case LDC: {
        auto index = code[pc];
        thread->incrementPC();
        auto constant = rt_cp.getConstant(index);
        if (std::holds_alternative<Jint>(constant)) {
          op_stack.pushInt(std::get<Jint>(constant));
        } else if (std::holds_alternative<Jfloat>(constant)) {
          op_stack.pushFloat(std::get<Jfloat>(constant));
        } else if (std::holds_alternative<std::string>(constant)) {
          // For now, push char* for string literals
          // TODO: implement proper string object creation
          op_stack.pushRef(
            static_cast<Jref>(const_cast<char*>(std::get<std::string>(constant).c_str())));
        }
      } break;
      case LDC_W: {
        auto index = combine_bytes(code[pc], code[pc + 1]);
        thread->incrementPC();
        thread->incrementPC();
        auto constant = rt_cp.getConstant(index);
        if (std::holds_alternative<Jint>(constant)) {
          op_stack.pushInt(std::get<Jint>(constant));
        } else if (std::holds_alternative<Jfloat>(constant)) {
          op_stack.pushFloat(std::get<Jfloat>(constant));
        } else if (std::holds_alternative<std::string>(constant)) {
          // For now, push null reference for string literals
          // TODO: implement proper string object creation
          op_stack.pushRef(nullptr);
        }
      } break;
      case LDC2_W: {
        auto index = combine_bytes(code[pc], code[pc + 1]);
        thread->incrementPC();
        thread->incrementPC();
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
        auto index = code[pc];
        thread->incrementPC();
        auto value = local_vars.getInt(index);
        op_stack.pushInt(value);
      } break;
      case LLOAD: {
        auto index = code[pc];
        thread->incrementPC();
        auto value = local_vars.getLong(index);
        op_stack.pushLong(value);
      } break;
      case FLOAD: {
        auto index = code[pc];
        thread->incrementPC();
        auto value = local_vars.getFloat(index);
        op_stack.pushFloat(value);
      } break;
      case DLOAD: {
        auto index = code[pc];
        thread->incrementPC();
        auto value = local_vars.getDouble(index);
        op_stack.pushDouble(value);
      } break;
      case ALOAD: {
        auto index = code[pc];
        thread->incrementPC();
        auto value = local_vars.getRef(index);
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
        auto value = local_vars.getRef(0);
        op_stack.pushRef(value);
      } break;
      case ALOAD_1: {
        auto value = local_vars.getRef(1);
        op_stack.pushRef(value);
      } break;
      case ALOAD_2: {
        auto value = local_vars.getRef(2);
        op_stack.pushRef(value);
      } break;
      case ALOAD_3: {
        auto value = local_vars.getRef(3);
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
        auto index = code[pc];
        thread->incrementPC();
        auto value = op_stack.popInt();
        local_vars.setInt(index, value);
      } break;
      case LSTORE: {
        auto index = code[pc];
        thread->incrementPC();
        auto value = op_stack.popLong();
        local_vars.setLong(index, value);
      } break;
      case FSTORE: {
        auto index = code[pc];
        thread->incrementPC();
        auto value = op_stack.popFloat();
        local_vars.setFloat(index, value);
      } break;
      case DSTORE: {
        auto index = code[pc];
        thread->incrementPC();
        auto value = op_stack.popDouble();
        local_vars.setDouble(index, value);
      } break;
      case ASTORE: {
        auto index = code[pc];
        thread->incrementPC();
        auto value = op_stack.popRef();
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
        auto value = op_stack.popRef();
        local_vars.setRef(0, value);
      } break;
      case ASTORE_1: {
        auto value = op_stack.popRef();
        local_vars.setRef(1, value);
      } break;
      case ASTORE_2: {
        auto value = op_stack.popRef();
        local_vars.setRef(2, value);
      } break;
      case ASTORE_3: {
        auto value = op_stack.popRef();
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
        auto shift_count = op_stack.popInt() & 0x1F;  // Only use lower 5 bits
        auto value       = op_stack.popInt();
        op_stack.pushInt(value << shift_count);
      } break;
      case LSHL: {
        // Long shift left: value1 << (value2 & 0x3f)
        auto shift_count = op_stack.popInt() & 0x3F;  // Only use lower 6 bits
        auto value       = op_stack.popLong();
        op_stack.pushLong(value << shift_count);
      } break;
      case ISHR: {
        // Integer arithmetic shift right: value1 >> (value2 & 0x1f)
        auto shift_count = op_stack.popInt() & 0x1F;  // Only use lower 5 bits
        auto value       = op_stack.popInt();
        op_stack.pushInt(value >> shift_count);
      } break;
      case LSHR: {
        // Long arithmetic shift right: value1 >> (value2 & 0x3f)
        auto shift_count = op_stack.popInt() & 0x3F;  // Only use lower 6 bits
        auto value       = op_stack.popLong();
        op_stack.pushLong(value >> shift_count);
      } break;
      case IUSHR: {
        // Integer logical shift right: (unsigned)value1 >>> (value2 & 0x1f)
        auto shift_count = op_stack.popInt() & 0x1F;  // Only use lower 5 bits
        auto value       = op_stack.popInt();
        op_stack.pushInt(static_cast<Jint>(static_cast<U4>(value) >> shift_count));
      } break;
      case LUSHR: {
        // Long logical shift right: (unsigned)value1 >>> (value2 & 0x3f)
        auto shift_count = op_stack.popInt() & 0x3F;  // Only use lower 6 bits
        auto value       = op_stack.popLong();
        op_stack.pushLong(static_cast<Jlong>(static_cast<U8>(value) >> shift_count));
      } break;
      case IAND: {
        // Integer bitwise AND
        auto value2 = op_stack.popInt();
        auto value1 = op_stack.popInt();
        op_stack.pushInt(value1 & value2);
      } break;
      case LAND: {
        // Long bitwise AND
        auto value2 = op_stack.popLong();
        auto value1 = op_stack.popLong();
        op_stack.pushLong(value1 & value2);
      } break;
      case IOR: {
        // Integer bitwise OR
        auto value2 = op_stack.popInt();
        auto value1 = op_stack.popInt();
        op_stack.pushInt(value1 | value2);
      } break;
      case LOR: {
        // Long bitwise OR
        auto value2 = op_stack.popLong();
        auto value1 = op_stack.popLong();
        op_stack.pushLong(value1 | value2);
      } break;
      case IXOR: {
        // Integer bitwise XOR
        auto value2 = op_stack.popInt();
        auto value1 = op_stack.popInt();
        op_stack.pushInt(value1 ^ value2);
      } break;
      case LXOR: {
        // Long bitwise XOR
        auto value2 = op_stack.popLong();
        auto value1 = op_stack.popLong();
        op_stack.pushLong(value1 ^ value2);
      } break;
      /* #endregion Arithmetic */

      /* #region IINC */

      // Function: Increment local variable by an immediate value
      // Components: local_vars, thread (PC)
      case IINC: {
        auto index     = code[pc];
        auto const_val = combine_bytes(code[pc + 1], code[pc + 2]);
        thread->incrementPC();
        thread->incrementPC();
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
        auto value         = op_stack.popInt();
        auto branch_offset = combine_bytes(code[pc], code[pc + 1]);
        thread->incrementPC();
        thread->incrementPC();
        if (value == 0) {
          thread->setPC(thread->getPC() + branch_offset - 2);
        }
      } break;
      case IFNE: {
        // Branch if int value not equal to 0
        auto value         = op_stack.popInt();
        auto branch_offset = combine_bytes(code[pc], code[pc + 1]);
        thread->incrementPC();
        thread->incrementPC();
        if (value != 0) {
          thread->setPC(thread->getPC() + branch_offset - 2);
        }
      } break;
      case IFLT: {
        // Branch if int value less than 0
        auto value         = op_stack.popInt();
        auto branch_offset = combine_bytes(code[pc], code[pc + 1]);
        thread->incrementPC();
        thread->incrementPC();
        if (value < 0) {
          thread->setPC(thread->getPC() + branch_offset - 2);
        }
      } break;
      case IFGE: {
        // Branch if int value greater than or equal to 0
        auto value         = op_stack.popInt();
        auto branch_offset = combine_bytes(code[pc], code[pc + 1]);
        thread->incrementPC();
        thread->incrementPC();
        if (value >= 0) {
          thread->setPC(thread->getPC() + branch_offset - 2);
        }
      } break;
      case IFGT: {
        // Branch if int value greater than 0
        auto value         = op_stack.popInt();
        auto branch_offset = combine_bytes(code[pc], code[pc + 1]);
        thread->incrementPC();
        thread->incrementPC();
        if (value > 0) {
          thread->setPC(thread->getPC() + branch_offset - 2);
        }
      } break;
      case IFLE: {
        // Branch if int value less than or equal to 0
        auto value         = op_stack.popInt();
        auto branch_offset = combine_bytes(code[pc], code[pc + 1]);
        thread->incrementPC();
        thread->incrementPC();
        if (value <= 0) {
          thread->setPC(thread->getPC() + branch_offset - 2);
        }
      } break;
      case IF_ICMPEQ: {
        // Branch if two int values are equal
        auto value2        = op_stack.popInt();
        auto value1        = op_stack.popInt();
        auto branch_offset = combine_bytes(code[pc], code[pc + 1]);
        thread->incrementPC();
        thread->incrementPC();
        if (value1 == value2) {
          thread->setPC(thread->getPC() + branch_offset - 2);
        }
      } break;
      case IF_ICMPNE: {
        // Branch if two int values are not equal
        auto value2        = op_stack.popInt();
        auto value1        = op_stack.popInt();
        auto branch_offset = combine_bytes(code[pc], code[pc + 1]);
        thread->incrementPC();
        thread->incrementPC();
        if (value1 != value2) {
          thread->setPC(thread->getPC() + branch_offset - 2);
        }
      } break;
      case IF_ICMPLT: {
        // Branch if first int value less than second
        auto value2        = op_stack.popInt();
        auto value1        = op_stack.popInt();
        auto branch_offset = combine_bytes(code[pc], code[pc + 1]);
        thread->incrementPC();
        thread->incrementPC();
        if (value1 < value2) {
          thread->setPC(thread->getPC() + branch_offset - 2);
        }
      } break;
      case IF_ICMPGE: {
        // Branch if first int value greater than or equal to second
        auto value2        = op_stack.popInt();
        auto value1        = op_stack.popInt();
        auto branch_offset = combine_bytes(code[pc], code[pc + 1]);
        thread->incrementPC();
        thread->incrementPC();
        if (value1 >= value2) {
          thread->setPC(thread->getPC() + branch_offset - 2);
        }
      } break;
      case IF_ICMPGT: {
        // Branch if first int value greater than second
        auto value2        = op_stack.popInt();
        auto value1        = op_stack.popInt();
        auto branch_offset = combine_bytes(code[pc], code[pc + 1]);
        thread->incrementPC();
        thread->incrementPC();
        if (value1 > value2) {
          thread->setPC(thread->getPC() + branch_offset - 2);
        }
      } break;
      case IF_ICMPLE: {
        // Branch if first int value less than or equal to second
        auto value2        = op_stack.popInt();
        auto value1        = op_stack.popInt();
        auto branch_offset = combine_bytes(code[pc], code[pc + 1]);
        thread->incrementPC();
        thread->incrementPC();
        if (value1 <= value2) {
          thread->setPC(thread->getPC() + branch_offset - 2);
        }
      } break;
      case IF_ACMPEQ: {
        // Branch if two reference values are equal
        auto value2        = op_stack.popRef();
        auto value1        = op_stack.popRef();
        auto branch_offset = combine_bytes(code[pc], code[pc + 1]);
        thread->incrementPC();
        thread->incrementPC();
        if (value1 == value2) {
          thread->setPC(thread->getPC() + branch_offset - 2);
        }
      } break;
      case IF_ACMPNE: {
        // Branch if two reference values are not equal
        auto value2        = op_stack.popRef();
        auto value1        = op_stack.popRef();
        auto branch_offset = combine_bytes(code[pc], code[pc + 1]);
        thread->incrementPC();
        thread->incrementPC();
        if (value1 != value2) {
          thread->setPC(thread->getPC() + branch_offset - 2);
        }
      } break;
      case IFNULL: {
        // Branch if reference value is null
        auto value         = op_stack.popRef();
        auto branch_offset = combine_bytes(code[pc], code[pc + 1]);
        thread->incrementPC();
        thread->incrementPC();
        if (value == nullptr) {
          thread->setPC(thread->getPC() + branch_offset - 2);
        }
      } break;
      case IFNONNULL: {
        // Branch if reference value is not null
        auto value         = op_stack.popRef();
        auto branch_offset = combine_bytes(code[pc], code[pc + 1]);
        thread->incrementPC();
        thread->incrementPC();
        if (value != nullptr) {
          thread->setPC(thread->getPC() + branch_offset - 2);
        }
      } break;
      /* #endregion Comparisons */

      /* #region Control flow */

      // Function: Unconditional branches and switch statements
      // Components: thread (PC)
      case GOTO: {
        auto branch_offset = combine_bytes(code[pc], code[pc + 1]);
        thread->setPC(thread->getPC() + branch_offset);
      } break;
      case GOTO_W: {
        auto branch_offset = combine_bytes(code[pc], code[pc + 1], code[pc + 2], code[pc + 3]);
        thread->setPC(thread->getPC() + branch_offset);
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
        auto bass_addr = thread->getPC();
        // skip padding to make sure the defaultOffset' address in bytecode is always 4-byte aligned
        thread->incrementPC(thread->getPC() % 4);
        // defaultOffset
        auto default_offset = combine_bytes(code[pc], code[pc + 1], code[pc + 2], code[pc + 3]);
        thread->incrementPC(4);
        // low
        auto low_bytes = combine_bytes(code[pc], code[pc + 1], code[pc + 2], code[pc + 3]);
        thread->incrementPC(4);
        // high
        auto high_bytes = combine_bytes(code[pc], code[pc + 1], code[pc + 2], code[pc + 3]);
        thread->incrementPC(4);
        // jump_offsets
        auto jump_offsets = std::vector<Jint>(high_bytes - low_bytes + 1);
        for (size_t i = 0; i < jump_offsets.size(); i++) {
          jump_offsets[i] = combine_bytes(code[pc], code[pc + 1], code[pc + 2], code[pc + 3]);
          thread->incrementPC(4);
        }
        // pop index from operand stack
        auto index = op_stack.popInt();
        if (index < low_bytes || index > high_bytes) {
          thread->setPC(bass_addr + default_offset);
        } else {
          thread->setPC(bass_addr + jump_offsets[index - low_bytes]);
        }
      } break;
      case LOOKUPSWITCH: {
        auto bass_addr = thread->getPC();
        // skip padding to make sure 4-byte alignment
        thread->incrementPC(thread->getPC() % 4);
        auto default_bytes = combine_bytes(code[pc], code[pc + 1], code[pc + 2], code[pc + 3]);
        thread->incrementPC(4);
        auto npairs_count = combine_bytes(code[pc], code[pc + 1], code[pc + 2], code[pc + 3]);
        thread->incrementPC(4);
        auto jump_offsets = std::vector<std::pair<Jint, Jint>>(npairs_count);
        for (size_t i = 0; i < jump_offsets.size(); i++) {
          jump_offsets[i].first = combine_bytes(code[pc], code[pc + 1], code[pc + 2], code[pc + 3]);
          thread->incrementPC(4);
          jump_offsets[i].second =
            combine_bytes(code[pc], code[pc + 1], code[pc + 2], code[pc + 3]);
          thread->incrementPC(4);
        }
        // pop key from operand stack
        auto key = op_stack.popInt();
        for (size_t i = 0; i < jump_offsets.size(); i++) {
          if (key == jump_offsets[i].first) {
            thread->setPC(bass_addr + jump_offsets[i].second);
            break;
          }
        }
        thread->setPC(bass_addr + default_bytes);
      } break;

      /* #endregion Control flow */

      /* #region Returns */

      // Function: Return from method
      // Components: thread, op_stack
      case IRETURN: {
        auto& callee_frame = thread->getCurrentFrame();
        Jint  ret          = callee_frame.getOperandStack().popInt();
        thread->popFrame();
        if (!thread->isStackEmpty()) {
          // push ret into caller frame's operand stack
          auto& caller_frame = thread->getCurrentFrame();
          caller_frame.getOperandStack().pushInt(ret);
          thread->setPC(caller_frame.getCallerPC());
        }
      } break;
      case LRETURN: {
        auto& callee_frame = thread->getCurrentFrame();
        Jlong ret          = callee_frame.getOperandStack().popLong();
        thread->popFrame();
        if (!thread->isStackEmpty()) {
          auto& caller_frame = thread->getCurrentFrame();
          caller_frame.getOperandStack().pushLong(ret);
          thread->setPC(caller_frame.getCallerPC());
        }
      } break;
      case FRETURN: {
        auto&  callee_frame = thread->getCurrentFrame();
        Jfloat ret          = callee_frame.getOperandStack().popFloat();
        thread->popFrame();
        if (!thread->isStackEmpty()) {
          auto& caller_frame = thread->getCurrentFrame();
          caller_frame.getOperandStack().pushFloat(ret);
          thread->setPC(caller_frame.getCallerPC());
        }
      } break;
      case DRETURN: {
        auto&   callee_frame = thread->getCurrentFrame();
        Jdouble ret          = callee_frame.getOperandStack().popDouble();
        thread->popFrame();
        if (!thread->isStackEmpty()) {
          auto& caller_frame = thread->getCurrentFrame();
          caller_frame.getOperandStack().pushDouble(ret);
          thread->setPC(caller_frame.getCallerPC());
        }
      } break;
      case ARETURN: {
        auto& callee_frame = thread->getCurrentFrame();
        Jref  ret          = callee_frame.getOperandStack().popRef();
        thread->popFrame();
        if (!thread->isStackEmpty()) {
          auto& caller_frame = thread->getCurrentFrame();
          caller_frame.getOperandStack().pushRef(ret);
          thread->setPC(caller_frame.getCallerPC());
        }
      } break;
      case RETURN: {
        thread->popFrame();
        if (!thread->isStackEmpty()) {
          size_t return_pc = thread->getCurrentFrame().getCallerPC();
          thread->setPC(return_pc);
        } else {
          return;
        }
      } break;
      /* #endregion Returns */

      /* #region Fields */

      // Function: Access static and instance fields
      // Components: rt_cp, op_stack, thread (PC)
      case GETSTATIC: {
        auto index = combine_bytes(code[pc], code[pc + 1]);
        thread->incrementPC(2);
        auto field = rt_cp.resolveField(index);
        auto slot  = field->getOwnerKlass()->getStaticSlot(field->getSlotIndex());
        op_stack.pushSlot(slot);
      } break;
      case PUTSTATIC: {
        auto index = combine_bytes(code[pc], code[pc + 1]);
        thread->incrementPC(2);
        auto field = rt_cp.resolveField(index);
        // TODO: check compatibility
        field->getOwnerKlass()->getStaticSlot(field->getSlotIndex()) = op_stack.popSlot();
      } break;
      case GETFIELD:
        // TODO: implement getfield, Object module are needed
        break;
      case PUTFIELD:
        // TODO: implement putfield, Object module are needed
        break;
      /* #endregion Fields */

      /* #region Methods */

      // Function: Invoke methods
      // Components: rt_cp, thread (PC), op_stack
      case INVOKEVIRTUAL: {
        // TODO: invoke virtual method
      } break;
      case INVOKESPECIAL:
        // TODO: implement invokespecial
        break;
      case INVOKESTATIC: {
        auto index = combine_bytes(code[pc], code[pc + 1]);
        thread->incrementPC(2);

        auto* method = rt_cp.resolveMethod(index);

        if (!method->isStatic()) {
          throw std::runtime_error("Cannot invoke non-static method as static");
        }

        U2 arg_slot_count = calculateArgSlotCount(method->getDescriptor());

        runtime::Frame next_frame(method);

        auto& current_op_stack = op_stack;  // 当前栈帧的操作数栈
        auto& next_local_vars  = next_frame.getLocalVariables();

        if (arg_slot_count > 0) {
          // must use int instead of U2, because the loop may decrement to negative numbers
          for (int i = arg_slot_count - 1; i >= 0; i--) {
            runtime::Slot val = current_op_stack.popSlot();
            next_local_vars.setSlot(i, val);
          }
        }

        thread->getCurrentFrame().setCallerPC(thread->getPC());

        thread->pushFrame(std::move(next_frame));
        thread->setPC(0);

      } break;
      case INVOKEINTERFACE:
        // TODO: implement invokeinterface
        break;
      case INVOKEDYNAMIC:
        // TODO: implement invokedynamic
        break;
      /* #endregion Methods */

      /* #region Objects */

      // Function: Object creation and type checking
      // Components: rt_cp, op_stack, thread (PC)
      case NEW: {
        // auto index = combine_bytes(code[pc], code[pc + 1]);
        // thread->incrementPC();
        // thread->incrementPC();
        // runtime::Klass* klass = rt_cp.resolveClass(index);
        // Jref            obj_ref = heap_.newInstance(klass);
        // op_stack.pushRef(obj_ref);
      } break;
      case CHECKCAST:
        // TODO: implement checkcast
        break;
      case INSTANCEOF:
        // TODO: implement instanceof
        break;
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

      case WIDE:
        // TODO: implement wide
        break;

      default:
        throw std::runtime_error("Invalid opcode: " + std::to_string(opcode));
    }
    // NOLINTEND(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
  }
}

}  // namespace jvm::engine