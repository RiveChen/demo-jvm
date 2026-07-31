> ⚠️ **历史评估报告（2026-01-06 快照）**  
> 本文是项目早期（2026 年 1 月）的外部审查快照，其中关于"NEW/虚调用/字段/native 未实现""Hello World 进度 30%"等内容**不代表当前状态**。  
> 最新能力清单请见 [README.md](../README.md)，已知缺陷与路线图见 [TODO.md](../TODO.md)。

# JVM Learning Project - Critical Evaluation

**Date**: 2026-01-06
**Reviewer**: Antigravity (Senior C++ & JVM Engineer)

## Executive Summary

The project establishes a solid foundation with a modern C++ structure (CMake, Header-only Runtime, Separation of Concerns). However, it is **significantly distant** from running a standard "Hello World" (`System.out.println`). The current state allows for executing simple arithmetic and static method calls, but lacks the object-oriented essence of Java (Instances, Virtual Dispatch, Heap) and the necessary interaction with the host OS (Native Interface).

**Estimated Progress to Hello World**: 30%

---

## 1. Project Structure & Build System

**Status**: ✅ **Good**

- **CMake**: Correctly configured with clear module separation (`classfile`, `interpreter`, `runtime`, `oops`).
- **Modern C++**: Usage of `std::variant`, `std::optional`, and `std::span` (C++20/23 features) is commendable.
- **Code Quality**: Clean, readable, and seemingly well-linted.

## 2. Core Components Analysis

### Class Loader (`src/classfile`)

**Status**: ⚠️ **Partial**

- **Parsing**: `ClassFileParser` seems functional for standard class files.
- **Loading**: `ClassLoader` implements basic classpath searching and recursive loading.
- **Critical Issue**: Explicitly **disables `java.lang.Object`** loading (`ClassLoader.cpp:106`). This breaks the root of the class hierarchy, making standard Java execution impossible.
- **Missing**: No bootstrap classloader logic for finding `rt.jar` or `jmods`. Currently relies on a flat file lookup.

### Interpreter (`src/interpreter`)

**Status**: ⚠️ **Incomplete**

- **Implemented**:
  - Arithmetic operations (`IADD`, `IMUL`, etc.) are well-implemented.
  - Stack manipulations (`POP`, `DUP`, etc.).
  - Static Method Invocation (`INVOKESTATIC`).
  - branching (`IF*`, `GOTO`).
- **MISSING (Blockers for Hello World)**:
  - **Object Creation**: `NEW` opcode is commented out. No heap allocation logic visible in interpreter loop.
  - **Instance Methods**: `INVOKEVIRTUAL` and `INVOKESPECIAL` are `TODO`. "Hello World" requires calling `String` constructors (`invokespecial`) and `PrintStream.println` (`invokevirtual`).
  - **Field Access**: `GETFIELD`/`PUTFIELD` are `TODO`.
  - **Synchronization**: `MONITORENTER`/`EXIT` missing (less critical for simple Hello World but needed for `sysout` init).

### Runtime & Memory (`src/runtime`, `src/oops`, `src/memory`)

**Status**: ⚠️ **Skeleton**

- **Frames/Stacks**: Implemented as header-only classes (`frame.h`, `stack.h`). Efficient but simplistic.
- **Threading**: `Thread` class exists but lacks OS thread mapping. JVM is effectively single-threaded (ok for stage 1).
- **Heap/GC**: `src/memory` exists but wasn't deeply inspected. Given `NEW` is unimplemented, the Heap likely isn't wired to the Execution Engine yet.
- **Object Representation**: `oops` directory exists, but without `NEW`, `Klass` <-> `Instance` linkage is unverified.

### Native Interface (JNI/JNA)

**Status**: ❌ **Missing**

- **System.out**: "Hello World" prints to stdout. In Java, this routes through `FileOutputStream.writeBytes` (native).
- **Current State**: No mechanism found to register or execute native methods.
- **Impact**: Even if you could execute bytecode, you cannot print to the console unless you implement a "Hack" (e.g., intercepting `Ljava/io/PrintStream;println` in the interpreter) or a proper Native Method Interface.

---

## 3. The Path to Hello World

To reach the milestone of running this Java code:

```java
public class HelloWorld {
    public static void main(String[] args) {
        System.out.println("Hello, JVM!");
    }
}
```

You must bridge the following gaps:

### Step 1: Object Creation & Heap (The "New" Opcode)

- **Task**: Implement `NEW` opcode.
- **Req**: Connect `interpreter` to `memory` manager to allocate space for an instance of `Klass`.

### Step 2: Method Invocation (The OO Trio)

- **Task**: Implement `INVOKESPECIAL` (for constructors).
- **Task**: Implement `INVOKEVIRTUAL` (for `println`). requires vtable/itable lookup or simple name matching lookup.
- **Task**: Implement `GETSTATIC` (for `System.out`). *Seems partially implemented, verify logic.*

### Step 3: Minimal Class Library (The Hard Part)

- **Problem**: `System.out.println` relies on `java.lang.System`, `java.io.PrintStream`, `java.io.FilterOutputStream`, `java.io.OutputStream`, `java.lang.String`, `java.lang.Object`.
- **Solution A (Hard)**: Parse real JDK `rt.jar`. Requires robust parsing (Generic signatures, Annotations) and robust native interface.
- **Solution B (Easy/Learning)**: Create "Mock" classes in your resource folder.
  - `java/lang/Object.class`
  - `java/lang/System.class` (with a fake `out` field)
  - `java/io/PrintStream.class` (with an empty `println` method)
  - `java/lang/String.class`

### Step 4: The Native Trap (Output)

- **Problem**: `PrintStream.println` eventually calls a native method.
- **Solution**:
  - Implement a **Native Registry**.
  - When `INVOKESTATIC` or `INVOKEVIRTUAL` sees a method marked `ACC_NATIVE`:
    - Look up the C++ function pointer.
    - Execute it (marshalling arguments from Java Stack to C++).
  - Register a native handler for `java/io/PrintStream.println0` (or similar) that calls `std::cout`.

## Recommendation

Don't aim for `System.out.println` purely yet. It's a high wall.
**Intermediate Goal**:

1. Implement `NEW` and `INVOKESPECIAL`.
2. Write a test case that creates a simple object and calls a method on it that returns an `int`.

   ```java
   public class Calc {
       public int add(int a, int b) { return a + b; }
       public static void main(String[] args) {
           Calc c = new Calc();
           int res = c.add(1, 2); 
           // assert res == 3 via interpreter inspection
       }
   }
   ```

3. Once that works, implement the Native Interface to allow printing.
