#pragma once

#include "common/types.h"

namespace jvm::engine {

// ============================================================================
// Java Virtual Machine Instruction Set - JVMS8 Specification
// ============================================================================

// No operation - does nothing
constexpr U1 NOP = 0x00;

// --- Constants ---
// Push constant values onto the operand stack

constexpr U1 ACONST_NULL = 0x01;
constexpr U1 ICONST_M1   = 0x02;
constexpr U1 ICONST_0    = 0x03;
constexpr U1 ICONST_1    = 0x04;
constexpr U1 ICONST_2    = 0x05;
constexpr U1 ICONST_3    = 0x06;
constexpr U1 ICONST_4    = 0x07;
constexpr U1 ICONST_5    = 0x08;
constexpr U1 LCONST_0    = 0x09;
constexpr U1 LCONST_1    = 0x0A;
constexpr U1 FCONST_0    = 0x0B;
constexpr U1 FCONST_1    = 0x0C;
constexpr U1 FCONST_2    = 0x0D;
constexpr U1 DCONST_0    = 0x0E;
constexpr U1 DCONST_1    = 0x0F;

constexpr U1 BIPUSH = 0x10;
constexpr U1 SIPUSH = 0x11;

constexpr U1 LDC    = 0x12;
constexpr U1 LDC_W  = 0x13;
constexpr U1 LDC2_W = 0x14;

// --- Loads ---
// Load values from local variables onto the operand stack
constexpr U1 ILOAD = 0x15;
constexpr U1 LLOAD = 0x16;
constexpr U1 FLOAD = 0x17;
constexpr U1 DLOAD = 0x18;
constexpr U1 ALOAD = 0x19;

constexpr U1 ILOAD_0 = 0x1A;
constexpr U1 ILOAD_1 = 0x1B;
constexpr U1 ILOAD_2 = 0x1C;
constexpr U1 ILOAD_3 = 0x1D;
constexpr U1 LLOAD_0 = 0x1E;
constexpr U1 LLOAD_1 = 0x1F;
constexpr U1 LLOAD_2 = 0x20;
constexpr U1 LLOAD_3 = 0x21;
constexpr U1 FLOAD_0 = 0x22;
constexpr U1 FLOAD_1 = 0x23;
constexpr U1 FLOAD_2 = 0x24;
constexpr U1 FLOAD_3 = 0x25;
constexpr U1 DLOAD_0 = 0x26;
constexpr U1 DLOAD_1 = 0x27;
constexpr U1 DLOAD_2 = 0x28;
constexpr U1 DLOAD_3 = 0x29;
constexpr U1 ALOAD_0 = 0x2A;
constexpr U1 ALOAD_1 = 0x2B;
constexpr U1 ALOAD_2 = 0x2C;
constexpr U1 ALOAD_3 = 0x2D;

// --- Array Loads ---
// Load values from arrays onto the operand stack
constexpr U1 IALOAD = 0x2E;
constexpr U1 LALOAD = 0x2F;
constexpr U1 FALOAD = 0x30;
constexpr U1 DALOAD = 0x31;
constexpr U1 AALOAD = 0x32;
constexpr U1 BALOAD = 0x33;
constexpr U1 CALOAD = 0x34;
constexpr U1 SALOAD = 0x35;

// --- Stores ---
// Store values from the operand stack into local variables
constexpr U1 ISTORE = 0x36;
constexpr U1 LSTORE = 0x37;
constexpr U1 FSTORE = 0x38;
constexpr U1 DSTORE = 0x39;
constexpr U1 ASTORE = 0x3A;

constexpr U1 ISTORE_0 = 0x3B;
constexpr U1 ISTORE_1 = 0x3C;
constexpr U1 ISTORE_2 = 0x3D;
constexpr U1 ISTORE_3 = 0x3E;
constexpr U1 LSTORE_0 = 0x3F;
constexpr U1 LSTORE_1 = 0x40;
constexpr U1 LSTORE_2 = 0x41;
constexpr U1 LSTORE_3 = 0x42;
constexpr U1 FSTORE_0 = 0x43;
constexpr U1 FSTORE_1 = 0x44;
constexpr U1 FSTORE_2 = 0x45;
constexpr U1 FSTORE_3 = 0x46;
constexpr U1 DSTORE_0 = 0x47;
constexpr U1 DSTORE_1 = 0x48;
constexpr U1 DSTORE_2 = 0x49;
constexpr U1 DSTORE_3 = 0x4A;
constexpr U1 ASTORE_0 = 0x4B;
constexpr U1 ASTORE_1 = 0x4C;
constexpr U1 ASTORE_2 = 0x4D;
constexpr U1 ASTORE_3 = 0x4E;

// --- Array Stores ---
// Store values from the operand stack into array elements
constexpr U1 IASTORE = 0x4F;
constexpr U1 LASTORE = 0x50;
constexpr U1 FASTORE = 0x51;
constexpr U1 DASTORE = 0x52;
constexpr U1 AASTORE = 0x53;
constexpr U1 BASTORE = 0x54;
constexpr U1 CASTORE = 0x55;
constexpr U1 SASTORE = 0x56;

// --- Stack ---
// Manipulate the operand stack (pop, dup, swap operations)
constexpr U1 POP     = 0x57;
constexpr U1 POP2    = 0x58;
constexpr U1 DUP     = 0x59;
constexpr U1 DUP_X1  = 0x5A;
constexpr U1 DUP_X2  = 0x5B;
constexpr U1 DUP2    = 0x5C;
constexpr U1 DUP2_X1 = 0x5D;
constexpr U1 DUP2_X2 = 0x5E;
constexpr U1 SWAP    = 0x5F;

// --- Arithmetic ---
// Perform arithmetic operations on numeric values
constexpr U1 IADD = 0x60;
constexpr U1 LADD = 0x61;
constexpr U1 FADD = 0x62;
constexpr U1 DADD = 0x63;
constexpr U1 ISUB = 0x64;
constexpr U1 LSUB = 0x65;
constexpr U1 FSUB = 0x66;
constexpr U1 DSUB = 0x67;
constexpr U1 IMUL = 0x68;
constexpr U1 LMUL = 0x69;
constexpr U1 FMUL = 0x6A;
constexpr U1 DMUL = 0x6B;
constexpr U1 IDIV = 0x6C;
constexpr U1 LDIV = 0x6D;
constexpr U1 FDIV = 0x6E;
constexpr U1 DDIV = 0x6F;
constexpr U1 IREM = 0x70;
constexpr U1 LREM = 0x71;
constexpr U1 FREM = 0x72;
constexpr U1 DREM = 0x73;
constexpr U1 INEG = 0x74;
constexpr U1 LNEG = 0x75;
constexpr U1 FNEG = 0x76;
constexpr U1 DNEG = 0x77;

// Bitwise Operations - shift and logical operations on integers
constexpr U1 ISHL  = 0x78;
constexpr U1 LSHL  = 0x79;
constexpr U1 ISHR  = 0x7A;
constexpr U1 LSHR  = 0x7B;
constexpr U1 IUSHR = 0x7C;
constexpr U1 LUSHR = 0x7D;
constexpr U1 IAND  = 0x7E;
constexpr U1 LAND  = 0x7F;
constexpr U1 IOR   = 0x80;
constexpr U1 LOR   = 0x81;
constexpr U1 IXOR  = 0x82;
constexpr U1 LXOR  = 0x83;

// Increment - increment local variable by constant
constexpr U1 IINC = 0x84;

// --- Conversions ---
// Convert between different numeric types
constexpr U1 I2L = 0x85;
constexpr U1 I2F = 0x86;
constexpr U1 I2D = 0x87;
constexpr U1 L2I = 0x88;
constexpr U1 L2F = 0x89;
constexpr U1 L2D = 0x8A;
constexpr U1 F2I = 0x8B;
constexpr U1 F2L = 0x8C;
constexpr U1 F2D = 0x8D;
constexpr U1 D2I = 0x8E;
constexpr U1 D2L = 0x8F;
constexpr U1 D2F = 0x90;
constexpr U1 I2B = 0x91;
constexpr U1 I2C = 0x92;
constexpr U1 I2S = 0x93;

// --- Comparisons ---
// Compare values and perform conditional branches
constexpr U1 LCMP      = 0x94;
constexpr U1 FCMPL     = 0x95;
constexpr U1 FCMPG     = 0x96;
constexpr U1 DCMPL     = 0x97;
constexpr U1 DCMPG     = 0x98;
constexpr U1 IFEQ      = 0x99;
constexpr U1 IFNE      = 0x9A;
constexpr U1 IFLT      = 0x9B;
constexpr U1 IFGE      = 0x9C;
constexpr U1 IFGT      = 0x9D;
constexpr U1 IFLE      = 0x9E;
constexpr U1 IF_ICMPEQ = 0x9F;
constexpr U1 IF_ICMPNE = 0xA0;
constexpr U1 IF_ICMPLT = 0xA1;
constexpr U1 IF_ICMPGE = 0xA2;
constexpr U1 IF_ICMPGT = 0xA3;
constexpr U1 IF_ICMPLE = 0xA4;
constexpr U1 IF_ACMPEQ = 0xA5;
constexpr U1 IF_ACMPNE = 0xA6;

// --- Control ---
// Control flow instructions (branches, returns, switches)
constexpr U1 GOTO         = 0xA7;
constexpr U1 JSR          = 0xA8;
constexpr U1 RET          = 0xA9;
constexpr U1 TABLESWITCH  = 0xAA;
constexpr U1 LOOKUPSWITCH = 0xAB;
constexpr U1 IRETURN      = 0xAC;
constexpr U1 LRETURN      = 0xAD;
constexpr U1 FRETURN      = 0xAE;
constexpr U1 DRETURN      = 0xAF;
constexpr U1 ARETURN      = 0xB0;
constexpr U1 RETURN       = 0xB1;

// --- References ---
// Access fields and invoke methods
constexpr U1 GETSTATIC       = 0xB2;
constexpr U1 PUTSTATIC       = 0xB3;
constexpr U1 GETFIELD        = 0xB4;
constexpr U1 PUTFIELD        = 0xB5;
constexpr U1 INVOKEVIRTUAL   = 0xB6;
constexpr U1 INVOKESPECIAL   = 0xB7;
constexpr U1 INVOKESTATIC    = 0xB8;
constexpr U1 INVOKEINTERFACE = 0xB9;
constexpr U1 INVOKEDYNAMIC   = 0xBA;

// --- Objects ---
// Object creation, manipulation, and type checking
constexpr U1 NEW          = 0xBB;
constexpr U1 NEWARRAY     = 0xBC;
constexpr U1 ANEWARRAY    = 0xBD;
constexpr U1 ARRAYLENGTH  = 0xBE;
constexpr U1 ATHROW       = 0xBF;
constexpr U1 CHECKCAST    = 0xC0;
constexpr U1 INSTANCEOF   = 0xC1;
constexpr U1 MONITORENTER = 0xC2;
constexpr U1 MONITOREXIT  = 0xC3;

// --- Wide ---
// Extended local variable access with 16-bit index
constexpr U1 WIDE = 0xC4;

// --- Multi-Array ---
// Create multi-dimensional arrays
constexpr U1 MULTIANEWARRAY = 0xC5;

// --- Conditional Branch ---
// Branch based on null/non-null reference values
constexpr U1 IFNULL    = 0xC6;
constexpr U1 IFNONNULL = 0xC7;

// --- Goto Wide ---
// Extended control flow with 32-bit offsets
constexpr U1 GOTO_W = 0xC8;
constexpr U1 JSR_W  = 0xC9;

// --- Reserved ---
// Reserved opcodes for debugging and implementation-specific use
constexpr U1 BREAKPOINT = 0xCA;
constexpr U1 IMPDEP1    = 0xFE;
constexpr U1 IMPDEP2    = 0xFF;

}  // namespace jvm::engine