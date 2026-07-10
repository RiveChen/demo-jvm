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
