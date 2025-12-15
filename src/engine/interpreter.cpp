#include "interpreter.h"

#include <cmath>

#include "common/types.h"
#include "opcode.h"
#include "runtime/frame.h"
#include "runtime/klass.h"
#include "runtime/method.h"
#include "runtime/thread.h"

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

    // fetch
    size_t pc = thread->getPC();
    if (pc >= code.size()) {
      // PC is beyond code length, method has finished executing
      // This typically means the method ended without an explicit RETURN
      // In a real JVM, this would be an error, but for testing we'll just pop the frame
      return;
    }
    auto opcode = code[pc];
    thread->incrementPC();

    switch (opcode) {
      case NOP:
        break;
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
      case BIPUSH: {
        op_stack.pushInt(code[pc]);
        thread->incrementPC();
      } break;
      case SIPUSH: {
        op_stack.pushInt(code[pc] + (code[pc + 1] << 8));
        thread->incrementPC();
        thread->incrementPC();
      } break;
      case LDC: {
        auto index = code[pc];
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
      case LDC_W: {
        auto index = code[pc] + (code[pc + 1] << 8);
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
        auto index = code[pc] + (code[pc + 1] << 8);
        thread->incrementPC();
        thread->incrementPC();
        auto constant = rt_cp.getConstant(index);
        if (std::holds_alternative<Jlong>(constant)) {
          op_stack.pushLong(std::get<Jlong>(constant));
        } else if (std::holds_alternative<Jdouble>(constant)) {
          op_stack.pushDouble(std::get<Jdouble>(constant));
        }
      } break;
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
        auto shift_count = op_stack.popInt() & 0x1f;  // Only use lower 5 bits
        auto value       = op_stack.popInt();
        op_stack.pushInt(value << shift_count);
      } break;
      case LSHL: {
        // Long shift left: value1 << (value2 & 0x3f)
        auto shift_count = op_stack.popInt() & 0x3f;  // Only use lower 6 bits
        auto value       = op_stack.popLong();
        op_stack.pushLong(value << shift_count);
      } break;
      case ISHR: {
        // Integer arithmetic shift right: value1 >> (value2 & 0x1f)
        auto shift_count = op_stack.popInt() & 0x1f;  // Only use lower 5 bits
        auto value       = op_stack.popInt();
        op_stack.pushInt(value >> shift_count);
      } break;
      case LSHR: {
        // Long arithmetic shift right: value1 >> (value2 & 0x3f)
        auto shift_count = op_stack.popInt() & 0x3f;  // Only use lower 6 bits
        auto value       = op_stack.popLong();
        op_stack.pushLong(value >> shift_count);
      } break;
      case IUSHR: {
        // Integer logical shift right: (unsigned)value1 >>> (value2 & 0x1f)
        auto shift_count = op_stack.popInt() & 0x1f;  // Only use lower 5 bits
        auto value       = op_stack.popInt();
        op_stack.pushInt(static_cast<Jint>(static_cast<U4>(value) >> shift_count));
      } break;
      case LUSHR: {
        // Long logical shift right: (unsigned)value1 >>> (value2 & 0x3f)
        auto shift_count = op_stack.popInt() & 0x3f;  // Only use lower 6 bits
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
      case IINC: {
        // Increment local variable by constant
        auto index     = code[pc];
        auto const_val = static_cast<Jint>(static_cast<Jbyte>(code[pc + 1]));
        thread->incrementPC();
        thread->incrementPC();
        auto current_value = local_vars.getInt(index);
        local_vars.setInt(index, current_value + const_val);
      } break;
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
        auto branch_offset = static_cast<Jshort>(code[pc] + (code[pc + 1] << 8));
        thread->incrementPC();
        thread->incrementPC();
        if (value == 0) {
          thread->setPC(thread->getPC() + branch_offset - 2);
        }
      } break;
      case IFNE: {
        // Branch if int value not equal to 0
        auto value         = op_stack.popInt();
        auto branch_offset = static_cast<Jshort>(code[pc] + (code[pc + 1] << 8));
        thread->incrementPC();
        thread->incrementPC();
        if (value != 0) {
          thread->setPC(thread->getPC() + branch_offset - 2);
        }
      } break;
      case IFLT: {
        // Branch if int value less than 0
        auto value         = op_stack.popInt();
        auto branch_offset = static_cast<Jshort>(code[pc] + (code[pc + 1] << 8));
        thread->incrementPC();
        thread->incrementPC();
        if (value < 0) {
          thread->setPC(thread->getPC() + branch_offset - 2);
        }
      } break;
      case IFGE: {
        // Branch if int value greater than or equal to 0
        auto value         = op_stack.popInt();
        auto branch_offset = static_cast<Jshort>(code[pc] + (code[pc + 1] << 8));
        thread->incrementPC();
        thread->incrementPC();
        if (value >= 0) {
          thread->setPC(thread->getPC() + branch_offset - 2);
        }
      } break;
      case IFGT: {
        // Branch if int value greater than 0
        auto value         = op_stack.popInt();
        auto branch_offset = static_cast<Jshort>(code[pc] + (code[pc + 1] << 8));
        thread->incrementPC();
        thread->incrementPC();
        if (value > 0) {
          thread->setPC(thread->getPC() + branch_offset - 2);
        }
      } break;
      case IFLE: {
        // Branch if int value less than or equal to 0
        auto value         = op_stack.popInt();
        auto branch_offset = static_cast<Jshort>(code[pc] + (code[pc + 1] << 8));
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
        auto branch_offset = static_cast<Jshort>(code[pc] + (code[pc + 1] << 8));
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
        auto branch_offset = static_cast<Jshort>(code[pc] + (code[pc + 1] << 8));
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
        auto branch_offset = static_cast<Jshort>(code[pc] + (code[pc + 1] << 8));
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
        auto branch_offset = static_cast<Jshort>(code[pc] + (code[pc + 1] << 8));
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
        auto branch_offset = static_cast<Jshort>(code[pc] + (code[pc + 1] << 8));
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
        auto branch_offset = static_cast<Jshort>(code[pc] + (code[pc + 1] << 8));
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
        auto branch_offset = static_cast<Jshort>(code[pc] + (code[pc + 1] << 8));
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
        auto branch_offset = static_cast<Jshort>(code[pc] + (code[pc + 1] << 8));
        thread->incrementPC();
        thread->incrementPC();
        if (value1 != value2) {
          thread->setPC(thread->getPC() + branch_offset - 2);
        }
      } break;
      case GOTO:
        // TODO: implement goto
        break;
      case JSR:
        // TODO: implement jsr
        break;
      case RET:
        // TODO: implement ret
        break;
      case TABLESWITCH:
        // TODO: implement tableswitch
        break;
      case LOOKUPSWITCH:
        // TODO: implement lookupswitch
        break;
      case IRETURN: {
        auto& callee_frame = thread->getCurrentFrame();
        Jint  ret          = callee_frame.getOperandStack().popInt();
        thread->popFrame();
        if (!thread->isStackEmpty()) {
          // push ret into caller frame's operand stack
          thread->getCurrentFrame().getOperandStack().pushInt(ret);
        }
      } break;
      case LRETURN: {
        auto& callee_frame = thread->getCurrentFrame();
        Jlong ret          = callee_frame.getOperandStack().popLong();
        thread->popFrame();
        if (!thread->isStackEmpty()) {
          thread->getCurrentFrame().getOperandStack().pushLong(ret);
        }
      } break;
      case FRETURN: {
        auto&  callee_frame = thread->getCurrentFrame();
        Jfloat ret          = callee_frame.getOperandStack().popFloat();
        thread->popFrame();
        if (!thread->isStackEmpty()) {
          thread->getCurrentFrame().getOperandStack().pushFloat(ret);
        }
      } break;
      case DRETURN: {
        auto&   callee_frame = thread->getCurrentFrame();
        Jdouble ret          = callee_frame.getOperandStack().popDouble();
        thread->popFrame();
        if (!thread->isStackEmpty()) {
          thread->getCurrentFrame().getOperandStack().pushDouble(ret);
        }
      } break;
      case ARETURN: {
        auto& callee_frame = thread->getCurrentFrame();
        Jref  ret          = callee_frame.getOperandStack().popRef();
        thread->popFrame();
        if (!thread->isStackEmpty()) {
          thread->getCurrentFrame().getOperandStack().pushRef(ret);
        }
      } break;
      case RETURN: {
        thread->popFrame();
      } break;
      case GETSTATIC: {
        auto index = static_cast<U2>(code[pc] + (code[pc + 1] << 8));
        thread->incrementPC();
        thread->incrementPC();
        auto field = rt_cp.resolveField(index);
        auto slot  = field->getOwnerKlass()->getStaticSlot(field->getSlotIndex());
        op_stack.pushSlot(slot);
      } break;
      case PUTSTATIC: {
        auto index = static_cast<U2>(code[pc] + (code[pc + 1] << 8));
        thread->incrementPC();
        thread->incrementPC();
        auto field = rt_cp.resolveField(index);
        // TODO: check compatibility
        field->getOwnerKlass()->getStaticSlot(field->getSlotIndex()) = op_stack.popSlot();
      } break;
      case GETFIELD:
        // TODO: implement getfield
        break;
      case PUTFIELD:
        // TODO: implement putfield
        break;
      case INVOKEVIRTUAL: {
        auto index = static_cast<U2>(code[pc] + (code[pc + 1] << 8));
        thread->incrementPC();
        thread->incrementPC();
        auto method = rt_cp.resolveMethod(index);
        // TODO: invoke virtual method
      }

      break;
      case INVOKESPECIAL:
        // TODO: implement invokespecial
        break;
      case INVOKESTATIC: {
        auto index = static_cast<U2>(code[pc] + (code[pc + 1] << 8));
        thread->incrementPC();
        thread->incrementPC();

        auto method = rt_cp.resolveMethod(index);

        if (!method->isStatic()) {
          throw std::runtime_error("Cannot invoke non-static method as static");
        }

        runtime::Frame next_frame(method);
        auto&          next_local_vars = next_frame.getLocalVariables();

        // push the method's arguments to next frame's local variables

      } break;
      case INVOKEINTERFACE:
        // TODO: implement invokeinterface
        break;
      case INVOKEDYNAMIC:
        // TODO: implement invokedynamic
        break;
      case NEW: {
        auto index = static_cast<Jshort>(code[pc] + (code[pc + 1] << 8));
        thread->incrementPC();
        thread->incrementPC();
        runtime::Klass* klass = rt_cp.resolveClass(index);
        // Jref            obj_ref = heap_.newInstance(klass);
        // op_stack.pushRef(obj_ref);
      } break;
      case NEWARRAY:
        // TODO: implement newarray
        break;
      case ANEWARRAY:
        // TODO: implement anewarray
        break;
      case ARRAYLENGTH:
        // TODO: implement arraylength
        break;
      case ATHROW:
        // TODO: implement athrow
        break;
      case CHECKCAST:
        // TODO: implement checkcast
        break;
      case INSTANCEOF:
        // TODO: implement instanceof
        break;
      case MONITORENTER:
        // TODO: implement monitorenter
        break;
      case MONITOREXIT:
        // TODO: implement monitorexit
        break;
      case WIDE:
        // TODO: implement wide
        break;
      case MULTIANEWARRAY:
        // TODO: implement multianewarray
        break;
      case IFNULL: {
        // Branch if reference value is null
        auto value         = op_stack.popRef();
        auto branch_offset = static_cast<Jshort>(code[pc] + (code[pc + 1] << 8));
        thread->incrementPC();
        thread->incrementPC();
        if (value == nullptr) {
          thread->setPC(thread->getPC() + branch_offset - 2);
        }
      } break;
      case IFNONNULL: {
        // Branch if reference value is not null
        auto value         = op_stack.popRef();
        auto branch_offset = static_cast<Jshort>(code[pc] + (code[pc + 1] << 8));
        thread->incrementPC();
        thread->incrementPC();
        if (value != nullptr) {
          thread->setPC(thread->getPC() + branch_offset - 2);
        }
      } break;
      case GOTO_W:
        // TODO: implement goto_w
        break;
      case JSR_W:
        // TODO: implement jsr_w
        break;

      default:
        throw std::runtime_error("Invalid opcode: " + std::to_string(opcode));
    }
  }
}

}  // namespace jvm::engine