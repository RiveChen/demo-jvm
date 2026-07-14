/**
 * @file opcode.h
 * @brief JVM bytecode opcode constants.
 *
 * Defines all JVM instruction opcodes as U1 constants,
 * organized by instruction category.
 *
 * @see JVM Specification §6.5 "Instructions" for the complete list.
 */

#pragma once

#include "utilities/types.hpp"

namespace jvm::engine {

// ============================================================================
// Java Virtual Machine Instruction Set - JVMS8 Specification
// ============================================================================

/** @name Constants */
///@{
constexpr U1 NOP         = 0x00;  ///< Do nothing
constexpr U1 ACONST_NULL = 0x01;  ///< Push null object reference onto stack
constexpr U1 ICONST_M1   = 0x02;  ///< Push int constant -1
constexpr U1 ICONST_0    = 0x03;  ///< Push int constant 0
constexpr U1 ICONST_1    = 0x04;  ///< Push int constant 1
constexpr U1 ICONST_2    = 0x05;  ///< Push int constant 2
constexpr U1 ICONST_3    = 0x06;  ///< Push int constant 3
constexpr U1 ICONST_4    = 0x07;  ///< Push int constant 4
constexpr U1 ICONST_5    = 0x08;  ///< Push int constant 5
constexpr U1 LCONST_0    = 0x09;  ///< Push long constant 0
constexpr U1 LCONST_1    = 0x0A;  ///< Push long constant 1
constexpr U1 FCONST_0    = 0x0B;  ///< Push float constant 0
constexpr U1 FCONST_1    = 0x0C;  ///< Push float constant 1
constexpr U1 FCONST_2    = 0x0D;  ///< Push float constant 2
constexpr U1 DCONST_0    = 0x0E;  ///< Push double constant 0
constexpr U1 DCONST_1    = 0x0F;  ///< Push double constant 1
constexpr U1 BIPUSH      = 0x10;  ///< Push byte immediate
constexpr U1 SIPUSH      = 0x11;  ///< Push short immediate
constexpr U1 LDC         = 0x12;  ///< Push item from runtime constant pool (8-bit index)
constexpr U1 LDC_W       = 0x13;  ///< Push item from runtime constant pool (16-bit index, wide)
constexpr U1 LDC2_W      = 0x14;  ///< Push long or double from constant pool (wide index)
///@}

/** @name Loads */
///@{
constexpr U1 ILOAD   = 0x15;  ///< Load int from local variable
constexpr U1 LLOAD   = 0x16;  ///< Load long from local variable
constexpr U1 FLOAD   = 0x17;  ///< Load float from local variable
constexpr U1 DLOAD   = 0x18;  ///< Load double from local variable
constexpr U1 ALOAD   = 0x19;  ///< Load reference from local variable
constexpr U1 ILOAD_0 = 0x1A;  ///< Load int from local variable 0
constexpr U1 ILOAD_1 = 0x1B;  ///< Load int from local variable 1
constexpr U1 ILOAD_2 = 0x1C;  ///< Load int from local variable 2
constexpr U1 ILOAD_3 = 0x1D;  ///< Load int from local variable 3
constexpr U1 LLOAD_0 = 0x1E;  ///< Load long from local variable 0
constexpr U1 LLOAD_1 = 0x1F;  ///< Load long from local variable 1
constexpr U1 LLOAD_2 = 0x20;  ///< Load long from local variable 2
constexpr U1 LLOAD_3 = 0x21;  ///< Load long from local variable 3
constexpr U1 FLOAD_0 = 0x22;  ///< Load float from local variable 0
constexpr U1 FLOAD_1 = 0x23;  ///< Load float from local variable 1
constexpr U1 FLOAD_2 = 0x24;  ///< Load float from local variable 2
constexpr U1 FLOAD_3 = 0x25;  ///< Load float from local variable 3
constexpr U1 DLOAD_0 = 0x26;  ///< Load double from local variable 0
constexpr U1 DLOAD_1 = 0x27;  ///< Load double from local variable 1
constexpr U1 DLOAD_2 = 0x28;  ///< Load double from local variable 2
constexpr U1 DLOAD_3 = 0x29;  ///< Load double from local variable 3
constexpr U1 ALOAD_0 = 0x2A;  ///< Load reference from local variable 0
constexpr U1 ALOAD_1 = 0x2B;  ///< Load reference from local variable 1
constexpr U1 ALOAD_2 = 0x2C;  ///< Load reference from local variable 2
constexpr U1 ALOAD_3 = 0x2D;  ///< Load reference from local variable 3
///@}

/** @name Array Loads */
///@{
constexpr U1 IALOAD = 0x2E;  ///< Load int from array
constexpr U1 LALOAD = 0x2F;  ///< Load long from array
constexpr U1 FALOAD = 0x30;  ///< Load float from array
constexpr U1 DALOAD = 0x31;  ///< Load double from array
constexpr U1 AALOAD = 0x32;  ///< Load reference from array
constexpr U1 BALOAD = 0x33;  ///< Load byte or boolean from array
constexpr U1 CALOAD = 0x34;  ///< Load char from array
constexpr U1 SALOAD = 0x35;  ///< Load short from array
///@}

/** @name Stores */
///@{
constexpr U1 ISTORE   = 0x36;  ///< Store int into local variable
constexpr U1 LSTORE   = 0x37;  ///< Store long into local variable
constexpr U1 FSTORE   = 0x38;  ///< Store float into local variable
constexpr U1 DSTORE   = 0x39;  ///< Store double into local variable
constexpr U1 ASTORE   = 0x3A;  ///< Store reference into local variable
constexpr U1 ISTORE_0 = 0x3B;  ///< Store int into local variable 0
constexpr U1 ISTORE_1 = 0x3C;  ///< Store int into local variable 1
constexpr U1 ISTORE_2 = 0x3D;  ///< Store int into local variable 2
constexpr U1 ISTORE_3 = 0x3E;  ///< Store int into local variable 3
constexpr U1 LSTORE_0 = 0x3F;  ///< Store long into local variable 0
constexpr U1 LSTORE_1 = 0x40;  ///< Store long into local variable 1
constexpr U1 LSTORE_2 = 0x41;  ///< Store long into local variable 2
constexpr U1 LSTORE_3 = 0x42;  ///< Store long into local variable 3
constexpr U1 FSTORE_0 = 0x43;  ///< Store float into local variable 0
constexpr U1 FSTORE_1 = 0x44;  ///< Store float into local variable 1
constexpr U1 FSTORE_2 = 0x45;  ///< Store float into local variable 2
constexpr U1 FSTORE_3 = 0x46;  ///< Store float into local variable 3
constexpr U1 DSTORE_0 = 0x47;  ///< Store double into local variable 0
constexpr U1 DSTORE_1 = 0x48;  ///< Store double into local variable 1
constexpr U1 DSTORE_2 = 0x49;  ///< Store double into local variable 2
constexpr U1 DSTORE_3 = 0x4A;  ///< Store double into local variable 3
constexpr U1 ASTORE_0 = 0x4B;  ///< Store reference into local variable 0
constexpr U1 ASTORE_1 = 0x4C;  ///< Store reference into local variable 1
constexpr U1 ASTORE_2 = 0x4D;  ///< Store reference into local variable 2
constexpr U1 ASTORE_3 = 0x4E;  ///< Store reference into local variable 3
///@}

/** @name Array Stores */
///@{
constexpr U1 IASTORE = 0x4F;  ///< Store into int array
constexpr U1 LASTORE = 0x50;  ///< Store into long array
constexpr U1 FASTORE = 0x51;  ///< Store into float array
constexpr U1 DASTORE = 0x52;  ///< Store into double array
constexpr U1 AASTORE = 0x53;  ///< Store into reference array
constexpr U1 BASTORE = 0x54;  ///< Store into byte/boolean array
constexpr U1 CASTORE = 0x55;  ///< Store into char array
constexpr U1 SASTORE = 0x56;  ///< Store into short array
///@}

/** @name Stack Operations */
///@{
constexpr U1 POP     = 0x57;  ///< Pop the top operand stack value
constexpr U1 POP2    = 0x58;  ///< Pop the top two operand stack values
constexpr U1 DUP     = 0x59;  ///< Duplicate the top operand stack value
constexpr U1 DUP_X1  = 0x5A;  ///< Duplicate top value and insert two values down
constexpr U1 DUP_X2  = 0x5B;  ///< Duplicate top value and insert two or three values down
constexpr U1 DUP2    = 0x5C;  ///< Duplicate the top one or two values
constexpr U1 DUP2_X1 = 0x5D;  ///< Duplicate top values and insert two or three values down
constexpr U1 DUP2_X2 = 0x5E;  ///< Duplicate top values and insert two, three, or four values down
constexpr U1 SWAP    = 0x5F;  ///< Swap the top two operand stack values
///@}

/** @name Arithmetic / Bitwise */
///@{
constexpr U1 IADD  = 0x60;  ///< Add int
constexpr U1 LADD  = 0x61;  ///< Add long
constexpr U1 FADD  = 0x62;  ///< Add float
constexpr U1 DADD  = 0x63;  ///< Add double
constexpr U1 ISUB  = 0x64;  ///< Subtract int
constexpr U1 LSUB  = 0x65;  ///< Subtract long
constexpr U1 FSUB  = 0x66;  ///< Subtract float
constexpr U1 DSUB  = 0x67;  ///< Subtract double
constexpr U1 IMUL  = 0x68;  ///< Multiply int
constexpr U1 LMUL  = 0x69;  ///< Multiply long
constexpr U1 FMUL  = 0x6A;  ///< Multiply float
constexpr U1 DMUL  = 0x6B;  ///< Multiply double
constexpr U1 IDIV  = 0x6C;  ///< Divide int
constexpr U1 LDIV  = 0x6D;  ///< Divide long
constexpr U1 FDIV  = 0x6E;  ///< Divide float
constexpr U1 DDIV  = 0x6F;  ///< Divide double
constexpr U1 IREM  = 0x70;  ///< Remainder int
constexpr U1 LREM  = 0x71;  ///< Remainder long
constexpr U1 FREM  = 0x72;  ///< Remainder float
constexpr U1 DREM  = 0x73;  ///< Remainder double
constexpr U1 INEG  = 0x74;  ///< Negate int
constexpr U1 LNEG  = 0x75;  ///< Negate long
constexpr U1 FNEG  = 0x76;  ///< Negate float
constexpr U1 DNEG  = 0x77;  ///< Negate double
constexpr U1 ISHL  = 0x78;  ///< Shift left int
constexpr U1 LSHL  = 0x79;  ///< Shift left long
constexpr U1 ISHR  = 0x7A;  ///< Arithmetic shift right int
constexpr U1 LSHR  = 0x7B;  ///< Arithmetic shift right long
constexpr U1 IUSHR = 0x7C;  ///< Logical shift right int
constexpr U1 LUSHR = 0x7D;  ///< Logical shift right long
constexpr U1 IAND  = 0x7E;  ///< Bitwise AND int
constexpr U1 LAND  = 0x7F;  ///< Bitwise AND long
constexpr U1 IOR   = 0x80;  ///< Bitwise OR int
constexpr U1 LOR   = 0x81;  ///< Bitwise OR long
constexpr U1 IXOR  = 0x82;  ///< Bitwise XOR int
constexpr U1 LXOR  = 0x83;  ///< Bitwise XOR long
constexpr U1 IINC  = 0x84;  ///< Increment local variable by constant
///@}

/** @name Type Conversions */
///@{
constexpr U1 I2L = 0x85;  ///< Convert int to long
constexpr U1 I2F = 0x86;  ///< Convert int to float
constexpr U1 I2D = 0x87;  ///< Convert int to double
constexpr U1 L2I = 0x88;  ///< Convert long to int
constexpr U1 L2F = 0x89;  ///< Convert long to float
constexpr U1 L2D = 0x8A;  ///< Convert long to double
constexpr U1 F2I = 0x8B;  ///< Convert float to int
constexpr U1 F2L = 0x8C;  ///< Convert float to long
constexpr U1 F2D = 0x8D;  ///< Convert float to double
constexpr U1 D2I = 0x8E;  ///< Convert double to int
constexpr U1 D2L = 0x8F;  ///< Convert double to long
constexpr U1 D2F = 0x90;  ///< Convert double to float
constexpr U1 I2B = 0x91;  ///< Convert int to byte
constexpr U1 I2C = 0x92;  ///< Convert int to char
constexpr U1 I2S = 0x93;  ///< Convert int to short
///@}

/** @name Comparisons & Branches */
///@{
constexpr U1 LCMP      = 0x94;  ///< Compare long values
constexpr U1 FCMPL     = 0x95;  ///< Compare float values (NaN → -1)
constexpr U1 FCMPG     = 0x96;  ///< Compare float values (NaN → +1)
constexpr U1 DCMPL     = 0x97;  ///< Compare double values (NaN → -1)
constexpr U1 DCMPG     = 0x98;  ///< Compare double values (NaN → +1)
constexpr U1 IFEQ      = 0x99;  ///< Branch if int comparison with zero succeeds (== 0)
constexpr U1 IFNE      = 0x9A;  ///< Branch if int comparison with zero succeeds (!= 0)
constexpr U1 IFLT      = 0x9B;  ///< Branch if int comparison with zero succeeds (< 0)
constexpr U1 IFGE      = 0x9C;  ///< Branch if int comparison with zero succeeds (>= 0)
constexpr U1 IFGT      = 0x9D;  ///< Branch if int comparison with zero succeeds (> 0)
constexpr U1 IFLE      = 0x9E;  ///< Branch if int comparison with zero succeeds (<= 0)
constexpr U1 IF_ICMPEQ = 0x9F;  ///< Branch if int comparison succeeds (==)
constexpr U1 IF_ICMPNE = 0xA0;  ///< Branch if int comparison succeeds (!=)
constexpr U1 IF_ICMPLT = 0xA1;  ///< Branch if int comparison succeeds (<)
constexpr U1 IF_ICMPGE = 0xA2;  ///< Branch if int comparison succeeds (>=)
constexpr U1 IF_ICMPGT = 0xA3;  ///< Branch if int comparison succeeds (>)
constexpr U1 IF_ICMPLE = 0xA4;  ///< Branch if int comparison succeeds (<=)
constexpr U1 IF_ACMPEQ = 0xA5;  ///< Branch if reference comparison succeeds (==)
constexpr U1 IF_ACMPNE = 0xA6;  ///< Branch if reference comparison succeeds (!=)
///@}

/** @name Control Flow */
///@{
constexpr U1 GOTO         = 0xA7;  ///< Branch always (16-bit offset)
constexpr U1 JSR          = 0xA8;  ///< Jump subroutine (obsolete in Java 8)
constexpr U1 RET          = 0xA9;  ///< Return from subroutine (obsolete in Java 8)
constexpr U1 TABLESWITCH  = 0xAA;  ///< Access jump table by index and jump
constexpr U1 LOOKUPSWITCH = 0xAB;  ///< Access jump table by key match and jump
constexpr U1 IRETURN      = 0xAC;  ///< Return int from method
constexpr U1 LRETURN      = 0xAD;  ///< Return long from method
constexpr U1 FRETURN      = 0xAE;  ///< Return float from method
constexpr U1 DRETURN      = 0xAF;  ///< Return double from method
constexpr U1 ARETURN      = 0xB0;  ///< Return reference from method
constexpr U1 RETURN       = 0xB1;  ///< Return void from method
///@}

/** @name Field & Method References */
///@{
constexpr U1 GETSTATIC       = 0xB2;  ///< Get static field from class
constexpr U1 PUTSTATIC       = 0xB3;  ///< Set static field in class
constexpr U1 GETFIELD        = 0xB4;  ///< Fetch field from object
constexpr U1 PUTFIELD        = 0xB5;  ///< Set field in object
constexpr U1 INVOKEVIRTUAL   = 0xB6;  ///< Invoke instance method (virtual dispatch)
constexpr U1 INVOKESPECIAL   = 0xB7;  ///< Invoke instance method (non-virtual)
constexpr U1 INVOKESTATIC    = 0xB8;  ///< Invoke class (static) method
constexpr U1 INVOKEINTERFACE = 0xB9;  ///< Invoke interface method
constexpr U1 INVOKEDYNAMIC   = 0xBA;  ///< Invoke dynamic method
///@}

/** @name Object Operations */
///@{
constexpr U1 NEW          = 0xBB;  ///< Create new object
constexpr U1 NEWARRAY     = 0xBC;  ///< Create new array of primitive type
constexpr U1 ANEWARRAY    = 0xBD;  ///< Create new array of reference type
constexpr U1 ARRAYLENGTH  = 0xBE;  ///< Get length of array
constexpr U1 ATHROW       = 0xBF;  ///< Throw exception
constexpr U1 CHECKCAST    = 0xC0;  ///< Check whether object is of given type
constexpr U1 INSTANCEOF   = 0xC1;  ///< Determine if object is of given type
constexpr U1 MONITORENTER = 0xC2;  ///< Enter monitor for object
constexpr U1 MONITOREXIT  = 0xC3;  ///< Exit monitor for object
///@}

/** @name Extended Addresses */
///@{
constexpr U1 WIDE           = 0xC4;  ///< Extend local variable index (16-bit)
constexpr U1 MULTIANEWARRAY = 0xC5;  ///< Create new multi-dimensional array
///@}

/** @name Additional Branches */
///@{
constexpr U1 IFNULL    = 0xC6;  ///< Branch if reference is null
constexpr U1 IFNONNULL = 0xC7;  ///< Branch if reference is not null
///@}

/** @name Wide Branches */
///@{
constexpr U1 GOTO_W = 0xC8;  ///< Branch always (32-bit offset)
constexpr U1 JSR_W  = 0xC9;  ///< Jump subroutine (wide index) [obsolete]
///@}

/** @name Reserved */
///@{
constexpr U1 BREAKPOINT = 0xCA;  ///< Reserved for breakpoints in debuggers
constexpr U1 IMPDEP1    = 0xFE;  ///< Implementation-dependent, first
constexpr U1 IMPDEP2    = 0xFF;  ///< Implementation-dependent, second
///@}

/// @brief Return a human-readable name for the given opcode.
/// @param op The opcode value (0x00–0xFF).
/// @return A string literal identifying the instruction.
inline const char* opcode_name(U1 op) {
  switch (op) {
    case NOP:
      return "NOP";
    case ACONST_NULL:
      return "ACONST_NULL";
    case ICONST_M1:
      return "ICONST_M1";
    case ICONST_0:
      return "ICONST_0";
    case ICONST_1:
      return "ICONST_1";
    case ICONST_2:
      return "ICONST_2";
    case ICONST_3:
      return "ICONST_3";
    case ICONST_4:
      return "ICONST_4";
    case ICONST_5:
      return "ICONST_5";
    case LCONST_0:
      return "LCONST_0";
    case LCONST_1:
      return "LCONST_1";
    case FCONST_0:
      return "FCONST_0";
    case FCONST_1:
      return "FCONST_1";
    case FCONST_2:
      return "FCONST_2";
    case DCONST_0:
      return "DCONST_0";
    case DCONST_1:
      return "DCONST_1";
    case BIPUSH:
      return "BIPUSH";
    case SIPUSH:
      return "SIPUSH";
    case LDC:
      return "LDC";
    case LDC_W:
      return "LDC_W";
    case LDC2_W:
      return "LDC2_W";
    case ILOAD:
      return "ILOAD";
    case LLOAD:
      return "LLOAD";
    case FLOAD:
      return "FLOAD";
    case DLOAD:
      return "DLOAD";
    case ALOAD:
      return "ALOAD";
    case ILOAD_0:
      return "ILOAD_0";
    case ILOAD_1:
      return "ILOAD_1";
    case ILOAD_2:
      return "ILOAD_2";
    case ILOAD_3:
      return "ILOAD_3";
    case LLOAD_0:
      return "LLOAD_0";
    case LLOAD_1:
      return "LLOAD_1";
    case LLOAD_2:
      return "LLOAD_2";
    case LLOAD_3:
      return "LLOAD_3";
    case FLOAD_0:
      return "FLOAD_0";
    case FLOAD_1:
      return "FLOAD_1";
    case FLOAD_2:
      return "FLOAD_2";
    case FLOAD_3:
      return "FLOAD_3";
    case DLOAD_0:
      return "DLOAD_0";
    case DLOAD_1:
      return "DLOAD_1";
    case DLOAD_2:
      return "DLOAD_2";
    case DLOAD_3:
      return "DLOAD_3";
    case ALOAD_0:
      return "ALOAD_0";
    case ALOAD_1:
      return "ALOAD_1";
    case ALOAD_2:
      return "ALOAD_2";
    case ALOAD_3:
      return "ALOAD_3";
    case IALOAD:
      return "IALOAD";
    case LALOAD:
      return "LALOAD";
    case FALOAD:
      return "FALOAD";
    case DALOAD:
      return "DALOAD";
    case AALOAD:
      return "AALOAD";
    case BALOAD:
      return "BALOAD";
    case CALOAD:
      return "CALOAD";
    case SALOAD:
      return "SALOAD";
    case ISTORE:
      return "ISTORE";
    case LSTORE:
      return "LSTORE";
    case FSTORE:
      return "FSTORE";
    case DSTORE:
      return "DSTORE";
    case ASTORE:
      return "ASTORE";
    case ISTORE_0:
      return "ISTORE_0";
    case ISTORE_1:
      return "ISTORE_1";
    case ISTORE_2:
      return "ISTORE_2";
    case ISTORE_3:
      return "ISTORE_3";
    case LSTORE_0:
      return "LSTORE_0";
    case LSTORE_1:
      return "LSTORE_1";
    case LSTORE_2:
      return "LSTORE_2";
    case LSTORE_3:
      return "LSTORE_3";
    case FSTORE_0:
      return "FSTORE_0";
    case FSTORE_1:
      return "FSTORE_1";
    case FSTORE_2:
      return "FSTORE_2";
    case FSTORE_3:
      return "FSTORE_3";
    case DSTORE_0:
      return "DSTORE_0";
    case DSTORE_1:
      return "DSTORE_1";
    case DSTORE_2:
      return "DSTORE_2";
    case DSTORE_3:
      return "DSTORE_3";
    case ASTORE_0:
      return "ASTORE_0";
    case ASTORE_1:
      return "ASTORE_1";
    case ASTORE_2:
      return "ASTORE_2";
    case ASTORE_3:
      return "ASTORE_3";
    case IASTORE:
      return "IASTORE";
    case LASTORE:
      return "LASTORE";
    case FASTORE:
      return "FASTORE";
    case DASTORE:
      return "DASTORE";
    case AASTORE:
      return "AASTORE";
    case BASTORE:
      return "BASTORE";
    case CASTORE:
      return "CASTORE";
    case SASTORE:
      return "SASTORE";
    case POP:
      return "POP";
    case POP2:
      return "POP2";
    case DUP:
      return "DUP";
    case DUP_X1:
      return "DUP_X1";
    case DUP_X2:
      return "DUP_X2";
    case DUP2:
      return "DUP2";
    case DUP2_X1:
      return "DUP2_X1";
    case DUP2_X2:
      return "DUP2_X2";
    case SWAP:
      return "SWAP";
    case IADD:
      return "IADD";
    case LADD:
      return "LADD";
    case FADD:
      return "FADD";
    case DADD:
      return "DADD";
    case ISUB:
      return "ISUB";
    case LSUB:
      return "LSUB";
    case FSUB:
      return "FSUB";
    case DSUB:
      return "DSUB";
    case IMUL:
      return "IMUL";
    case LMUL:
      return "LMUL";
    case FMUL:
      return "FMUL";
    case DMUL:
      return "DMUL";
    case IDIV:
      return "IDIV";
    case LDIV:
      return "LDIV";
    case FDIV:
      return "FDIV";
    case DDIV:
      return "DDIV";
    case IREM:
      return "IREM";
    case LREM:
      return "LREM";
    case FREM:
      return "FREM";
    case DREM:
      return "DREM";
    case INEG:
      return "INEG";
    case LNEG:
      return "LNEG";
    case FNEG:
      return "FNEG";
    case DNEG:
      return "DNEG";
    case ISHL:
      return "ISHL";
    case LSHL:
      return "LSHL";
    case ISHR:
      return "ISHR";
    case LSHR:
      return "LSHR";
    case IUSHR:
      return "IUSHR";
    case LUSHR:
      return "LUSHR";
    case IAND:
      return "IAND";
    case LAND:
      return "LAND";
    case IOR:
      return "IOR";
    case LOR:
      return "LOR";
    case IXOR:
      return "IXOR";
    case LXOR:
      return "LXOR";
    case IINC:
      return "IINC";
    case I2L:
      return "I2L";
    case I2F:
      return "I2F";
    case I2D:
      return "I2D";
    case L2I:
      return "L2I";
    case L2F:
      return "L2F";
    case L2D:
      return "L2D";
    case F2I:
      return "F2I";
    case F2L:
      return "F2L";
    case F2D:
      return "F2D";
    case D2I:
      return "D2I";
    case D2L:
      return "D2L";
    case D2F:
      return "D2F";
    case I2B:
      return "I2B";
    case I2C:
      return "I2C";
    case I2S:
      return "I2S";
    case LCMP:
      return "LCMP";
    case FCMPL:
      return "FCMPL";
    case FCMPG:
      return "FCMPG";
    case DCMPL:
      return "DCMPL";
    case DCMPG:
      return "DCMPG";
    case IFEQ:
      return "IFEQ";
    case IFNE:
      return "IFNE";
    case IFLT:
      return "IFLT";
    case IFGE:
      return "IFGE";
    case IFGT:
      return "IFGT";
    case IFLE:
      return "IFLE";
    case IF_ICMPEQ:
      return "IF_ICMPEQ";
    case IF_ICMPNE:
      return "IF_ICMPNE";
    case IF_ICMPLT:
      return "IF_ICMPLT";
    case IF_ICMPGE:
      return "IF_ICMPGE";
    case IF_ICMPGT:
      return "IF_ICMPGT";
    case IF_ICMPLE:
      return "IF_ICMPLE";
    case IF_ACMPEQ:
      return "IF_ACMPEQ";
    case IF_ACMPNE:
      return "IF_ACMPNE";
    case GOTO:
      return "GOTO";
    case JSR:
      return "JSR";
    case RET:
      return "RET";
    case TABLESWITCH:
      return "TABLESWITCH";
    case LOOKUPSWITCH:
      return "LOOKUPSWITCH";
    case IRETURN:
      return "IRETURN";
    case LRETURN:
      return "LRETURN";
    case FRETURN:
      return "FRETURN";
    case DRETURN:
      return "DRETURN";
    case ARETURN:
      return "ARETURN";
    case RETURN:
      return "RETURN";
    case GETSTATIC:
      return "GETSTATIC";
    case PUTSTATIC:
      return "PUTSTATIC";
    case GETFIELD:
      return "GETFIELD";
    case PUTFIELD:
      return "PUTFIELD";
    case INVOKEVIRTUAL:
      return "INVOKEVIRTUAL";
    case INVOKESPECIAL:
      return "INVOKESPECIAL";
    case INVOKESTATIC:
      return "INVOKESTATIC";
    case INVOKEINTERFACE:
      return "INVOKEINTERFACE";
    case INVOKEDYNAMIC:
      return "INVOKEDYNAMIC";
    case NEW:
      return "NEW";
    case NEWARRAY:
      return "NEWARRAY";
    case ANEWARRAY:
      return "ANEWARRAY";
    case ARRAYLENGTH:
      return "ARRAYLENGTH";
    case ATHROW:
      return "ATHROW";
    case CHECKCAST:
      return "CHECKCAST";
    case INSTANCEOF:
      return "INSTANCEOF";
    case MONITORENTER:
      return "MONITORENTER";
    case MONITOREXIT:
      return "MONITOREXIT";
    case WIDE:
      return "WIDE";
    case MULTIANEWARRAY:
      return "MULTIANEWARRAY";
    case IFNULL:
      return "IFNULL";
    case IFNONNULL:
      return "IFNONNULL";
    case GOTO_W:
      return "GOTO_W";
    case JSR_W:
      return "JSR_W";
    case BREAKPOINT:
      return "BREAKPOINT";
    case IMPDEP1:
      return "IMPDEP1";
    case IMPDEP2:
      return "IMPDEP2";
    default:
      return "???";
  }
}

}  // namespace jvm::engine
