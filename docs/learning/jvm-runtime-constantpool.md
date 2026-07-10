# 运行时常量池

主要存储两种信息：

- 字面量（Literals）
  - `int`
  - `long`
  - `float`
  - `double`
  - 字符串字面量（指向 CONSTANT_Utf8_info）
- 符号引用（Symbolic References）
  - 类符号引用
  - 字段符号引用
  - 方法符号引用
  - 接口方法符号引用

运行时常量池（Runtime Constant Pool）完整保留了 Class 文件常量池中的内容。无论方法是属于“本类”还是“其他类”，只要在字节码中发生了方法调用（Method Invocation），就必须依赖常量池中的 符号引用（Symbolic Reference）。

如果本类中定义了一个方法 bar()，但在本类的代码中从未显式调用过它，那么在本类的常量池（Constant Pool）中，绝对不会生成指向 bar() 的 CONSTANT_Methodref_info（符号引用）。


``` txt
CONSTANT_Class_info {
    u1 tag;
    u2 name_index; // 指向 CONSTANT_Utf8_info
}
```



``` txt
CONSTANT_Fieldref_info {
    u1 tag;
    u2 class_index;
    u2 name_and_type_index;
}
CONSTANT_Methodref_info {
    u1 tag;
    u2 class_index;
    u2 name_and_type_index;
}
CONSTANT_InterfaceMethodref_info {
    u1 tag;
    u2 class_index;
    u2 name_and_type_index;
}
```

