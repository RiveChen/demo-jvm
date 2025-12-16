# Interpreter 单元测试

本目录包含 JVM Interpreter 的单元测试。

## 测试文件结构

### 测试基类
- `interpreter_test_base.h` - 提供共享的测试辅助函数和测试夹具

### 测试文件
- `interpreter_arithmetic_test.cpp` - 算术运算指令测试（已有）
- `interpreter_constants_test.cpp` - 常量加载指令测试
- `interpreter_load_store_test.cpp` - 加载/存储指令测试
- `interpreter_stack_test.cpp` - 栈操作指令测试
- `interpreter_conversion_test.cpp` - 类型转换指令测试

## 测试覆盖范围

### 常量加载指令
- ✅ `ICONST_M1`, `ICONST_0` ~ `ICONST_5`
- ✅ `LCONST_0`, `LCONST_1`
- ✅ `FCONST_0`, `FCONST_1`, `FCONST_2`
- ✅ `DCONST_0`, `DCONST_1`
- ✅ `BIPUSH` (边界值测试)
- ✅ `SIPUSH` (边界值测试)
- ✅ `ACONST_NULL`

### 加载/存储指令
- ✅ `ILOAD`, `ILOAD_0` ~ `ILOAD_3`
- ✅ `ISTORE`, `ISTORE_0` ~ `ISTORE_3`
- ✅ `LLOAD`, `LLOAD_0`
- ✅ `LSTORE`, `LSTORE_0`
- ✅ `FLOAD`, `FLOAD_0`
- ✅ `FSTORE`, `FSTORE_0`
- ✅ `DLOAD`, `DLOAD_0`
- ✅ `DSTORE`, `DSTORE_0`
- ✅ `ALOAD`, `ALOAD_0`
- ✅ `ASTORE`, `ASTORE_0`

### 栈操作指令
- ✅ `POP`, `POP2`
- ✅ `DUP`, `DUP_X1`, `DUP_X2`
- ✅ `DUP2`
- ✅ `SWAP`

### 类型转换指令
- ✅ `I2L`, `I2F`, `I2D`
- ✅ `L2I`, `L2F`, `L2D`
- ✅ `F2I`, `F2L`, `F2D` (包括 NaN/Infinity 处理)
- ✅ `D2I`, `D2L`, `D2F` (包括 NaN/Infinity 处理)
- ✅ `I2B`, `I2C`, `I2S`

### 算术运算指令
- ✅ 已在 `interpreter_arithmetic_test.cpp` 中实现

## 运行测试

### 运行所有 interpreter 测试
```bash
cd build
ctest -R test_interpreter
```

### 运行特定测试
```bash
# 常量测试
./build/bin/test_interpreter_constants

# 加载/存储测试
./build/bin/test_interpreter_load_store

# 栈操作测试
./build/bin/test_interpreter_stack

# 类型转换测试
./build/bin/test_interpreter_conversion

# 算术运算测试
./build/bin/test_interpreter_arithmetic
```

### 运行特定测试用例
```bash
# 只运行 ICONST 相关测试
./build/bin/test_interpreter_constants --gtest_filter="*ICONST*"

# 只运行 ILOAD 相关测试
./build/bin/test_interpreter_load_store --gtest_filter="*ILOAD*"
```

## 测试辅助函数

`InterpreterTestBase` 提供了以下辅助函数：

- `executeIntMethod()` - 执行返回 int 的方法（2 个参数）
- `executeIntMethod1Arg()` - 执行返回 int 的方法（1 个参数）
- `executeIntMethod0Arg()` - 执行返回 int 的方法（无参数）
- `executeLongMethod()` - 执行返回 long 的方法（2 个参数）
- `executeLongMethod1Arg()` - 执行返回 long 的方法（1 个参数）
- `executeLongMethod0Arg()` - 执行返回 long 的方法（无参数）
- `executeFloatMethod()` - 执行返回 float 的方法（2 个参数）
- `executeFloatMethod1Arg()` - 执行返回 float 的方法（1 个参数）
- `executeFloatMethod0Arg()` - 执行返回 float 的方法（无参数）
- `executeDoubleMethod()` - 执行返回 double 的方法（2 个参数）
- `executeDoubleMethod1Arg()` - 执行返回 double 的方法（1 个参数）
- `executeDoubleMethod0Arg()` - 执行返回 double 的方法（无参数）
- `executeVoidMethod()` - 执行无返回值的方法

## 待实现的测试

以下指令类别尚未实现测试（因为 interpreter 中还未实现）：

- 分支指令 (`IFEQ`, `IF_ICMPEQ` 等) - 需要实现 `GOTO` 支持
- 控制流指令 (`GOTO`, `TABLESWITCH`, `LOOKUPSWITCH`)
- 返回指令 (`IRETURN`, `RETURN` 等) - 部分已通过其他测试间接测试
- 字段访问 (`GETSTATIC`, `PUTSTATIC`, `GETFIELD`, `PUTFIELD`)
- 方法调用 (`INVOKESTATIC`, `INVOKEVIRTUAL` 等)
- 对象操作 (`NEW`, `CHECKCAST`, `INSTANCEOF`)
- 数组操作 (`NEWARRAY`, `ANEWARRAY`, `ARRAYLENGTH` 等)
- 位运算指令 (`ISHL`, `ISHR`, `IAND`, `IOR`, `IXOR` 等)
- `IINC` 指令

## 注意事项

1. 所有测试都依赖于编译好的 Java 测试类（位于 `build/test_classes/`）
2. 测试会在 `SetUp()` 中重置 `MethodArea`，确保测试之间相互独立
3. 测试使用真实的类加载器加载 Java 类，然后执行字节码

