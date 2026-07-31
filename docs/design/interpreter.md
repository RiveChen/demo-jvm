# 解释器

通过解释执行字节码。

其最核心的思路无非是一个 `while` + `switch` 的组合。

所涉及的组件包括：

- `pc`
- `code`
- `operand_stack`
- `local_variables`
- `runtime_constant_pool`

个人将字节码大致分为以下几类：

- `push`：将常量、立即数、常量池中的常量压入`op_stack`
  - `CONST`
  - `BIPUSH` `SIPUSH`：从字节码中读取出 `byte` 或 `short` 类型的立即数，然后推入`op_stack`
  - `LDC` `LDC_W` `LDC2_W`：从`rt_cp`中读取出常量，然后推入`op_stack`
- `load`：从`local_vars`中加载值到`op_stack`
- `store`：将`op_stack`中的值存储到`local_vars`
- `stack`：操作`op_stack`
- `arithmetic`：在`op_stack`上执行算术运算
- `IINC`：将`local_vars`中的值增加指定值
- `conversion`：在`op_stack`上执行类型转换
- `comparison`：在`op_stack`上执行比较运算
  - `CMP`：将比较结果压入`op_stack`
  - `IF`：根据比较结果操作`pc`
- `control flow`：操作`pc`
  - `GOTO`：无条件跳转
  - `JSR` `JSR_W` `RET` 目前不被 Java SE 8 使用，暂时先不实现
  - `TABLESWITCH` `LOOKUPSWITCH`：从字节码中建立索引表，从`op_stack`中弹出索引，根据索引从索引表中获取跳转地址
- `return`：从`op_stack`中弹出返回值，并将其压入调用者的`op_stack`
- `fields`：访问静态和实例字段
  - `GETSTATIC` `PUTSTATIC`：访问静态字段
  - `GETFIELD` `PUTFIELD`：访问实例字段
- `methods`：调用方法
  - `INVOKEVIRTUAL`：调用实例方法
  - `INVOKESPECIAL`：调用实例方法
  - `INVOKESTATIC`：调用静态方法
  - `INVOKEINTERFACE`：调用接口方法
  - `INVOKEDYNAMIC`：调用动态方法
- `objects`：对象创建和类型检查
   - `NEW`：创建对象
   - `CHECKCAST`：检查对象类型
   - `INSTANCEOF`：检查对象类型

---

## C++ UB 与 JVM 语义

JVM 规范定义了明确的数值语义，但部分语义在 C++ 中属于**未定义行为（UB）**。实现解释器时，不能直接翻译 Java 语义为 C++ 代码，必须用 well-defined 的方式表达 JVM 要求的行为。以下为 UBSan 发现的全部 4 类问题及其修复：

### 1. 有符号整数溢出 → 应回绕（Two's complement）

| 指令 | C++ UB | JVM 要求 |
|------|--------|---------|
| `IADD` `ISUB` `IMUL` `INEG` | 有符号溢出即 UB | 回绕 |
| `LADD` `LSUB` `LMUL` `LNEG` | 同上 | 同上 |

**修复**：操作数转为无符号运算（无符号溢出 well-defined），结果转回有符号。

```cpp
auto v1 = static_cast<U4>(op_stack.popInt());
auto v2 = static_cast<U4>(op_stack.popInt());
op_stack.pushInt(static_cast<Jint>(v1 + v2));
```

### 2. `INT_MIN / -1` → 应返回被除数本身

| 指令 | C++ UB | JVM 要求 |
|------|--------|---------|
| `IDIV` | `INT32_MIN / -1` 溢出 | 返回 `INT32_MIN` |
| `LDIV` | `INT64_MIN / -1` 溢出 | 返回 `INT64_MIN` |
| `IREM` | `INT32_MIN % -1` 溢出 | 返回 `0` |
| `LREM` | `INT64_MIN % -1` 溢出 | 返回 `0` |

**修复**：分支特判，绕过 C++ 除法/取余。

```cpp
if (value1 == INT32_MIN && value2 == -1) {
  op_stack.pushInt(INT32_MIN);  // JVM §6.5.idiv
} else {
  op_stack.pushInt(value1 / value2);
}
```

### 3. 浮点 → 整数转换超出范围 → 应饱和

| 指令 | C++ UB | JVM 要求 |
|------|--------|---------|
| `F2I` `F2L` `D2I` `D2L` | `static_cast` 超出目标范围 | NaN→0，超限→±极值 |

**修复**：先做范围检查再 `static_cast`。

```cpp
if (std::isnan(value)) {
  op_stack.pushInt(0);
} else if (value >= static_cast<float>(INT32_MAX)) {
  op_stack.pushInt(INT32_MAX);
} else if (value <= static_cast<float>(INT32_MIN)) {
  op_stack.pushInt(INT32_MIN);
} else {
  op_stack.pushInt(static_cast<Jint>(value));
}
```

### 4. UBSan 配置与测试架构

- `-fno-sanitize-recover=undefined` 会让首个 UB 触发 SIGABRT，导致同一可执行文件中的后续 GoogleTest 全部丢失。
- **修复**：移除该 flag，保留 `-fsanitize=undefined`。UBSan 诊断输出到 stderr，进程继续运行，所有测试执行完毕。
- ASan 保持默认 abort 模式（`use-after-free` 等错误继续执行会导致更混乱的状态）。
