# demo-jvm 待办清单

_更新于 2026-07-12。`[x]` 已完成,`[ ]` 待办,引用处标注 `文件:行`。_

---

## ✅ 已完成

- [x] **descriptor 模块 + 单测**(`utilities/descriptor.*`、`tests/modules/utilities/descriptor_test.cpp`);带游标原语 + 边界检查(A5 随之解决)。
- [x] **Slot 去 tag 重构**:`slot.h` 裸 union(B4);GETSTATIC/PUTSTATIC 按 descriptor + `pushWide`/`popWide` 分派(B2);`getStaticSlot` 去 PADDING 抛异常(B3);long/double 两槽仅作计数(B6)。
- [x] **静态字段默认值**:`statics_.resize()` 值初始化(全零 = 所有 Java 默认值)(B1)。
- [x] **Method/Field 身份与缓存分离**:`getDescriptor()` 返回字符串做身份(find\* 比字符串,无损),另存缓存 `MethodType`/`TypeKind`。
- [x] **命名空间/目录/CMake target 三向对齐**:`interpreter/`→`engine/`、ns `class_loader`→`classfile`、测试 `common/`→`utilities/`、`jvm::common` 折入 `jvm`、`jvm_utilities` INTERFACE→STATIC。
- [x] **`.DS_Store` 入 `.gitignore`**(E1)。
- [x] **A2 long/double 传参已修 + 已验证** — INVOKESTATIC 单趟反向 + `setWide`/`popWide` typed 转移,`arg_slot_count` 缓存进 `MethodType`;`interpreter_method_invocation_test.cpp` 加了 6 个 `InvokeStatic_*` 用例(值/顺序/64 位宽值/第二 cat-2 偏移/double/混合偏移)覆盖。
- [x] **A3 `getSize()` → `size_t`** — `operand_stack.h`、`local_variables.h` 均已改。
- [x] **C1 PC 收进 Frame** — `Thread` 删掉 pc 整套;`Frame` 持 `pc_`(唯一真相);解释器本地 `pc` 做热寄存器,invoke 前 spill、return 后 reload。
- [x] **边界检查加固** — CP 下标(`constant_pool.*`)、`getStaticSlot` 用 `.at()`;`BytecodeReader::readU1` 加界检查(`readU2`/`readU4` 走它);`LocalVariables::setWide` 修 index+1 越界写、`checkBounds` 参数改 `size_t`。
- [x] **C4 断开 oops→ClassFile 耦合** — SymRef 烘焙成字符串(`class_name` / `class_cp_index`+`member_name`+`descriptor`),运行期 resolve\* 自给自足、不再碰 ClassFile;Option B 完成:`Klass` 删除 `class_file_`/`getClassFile`,loader 把 `ClassFile*` 直接 thread 进 link\*/prepare\*。修了 `/`→`.` 烘焙丢失和 `loadClass` 的 use-after-move 段错误。`constant_pool_test` 按新 API 重写。
- [x] **C5 迁移 oops 测试** — `klass`/`method_area`/`constant_pool_test` 移入 `tests/modules/oops/`,`test_runtime` 只留 `local_variables`/`operand_stack`。
- [x] **M1/D2 无 GC 堆 + Object** — `memory/heap.*`(bump arena,64MB Meyer's 单例,8 对齐 + OOM)、`oops/object.h`(`MarkWord` 预留、无虚函数、`this+1` 内联字段、typed 访问器);`Jref` 直接指针。`tests/modules/memory/heap_test.cpp` 单测(newInstance/字段往返/对象独立/ref 访问器)。
- [x] **D3 NEW + GETFIELD + PUTFIELD** — NEW 经 `Heap::getSingleton().newInstance`;字段读写复用 GETSTATIC 的 `isCategory2` 宽度分派。修了 NEW 的 `pc += 2` 双进、PUTFIELD 的弹栈顺序(先 value 后 objectref)。
- [x] **INVOKESPECIAL** — 镜像 INVOKESTATIC + receiver 进 slot 0;`java.lang.Object.<init>()V` 符号层特判 no-op。`interpreter_object_test.cpp` 端到端测 NEW→`<init>`→PUTFIELD→GETFIELD。
- [x] **M5 INVOKEVIRTUAL + 虚分派** — `peekRef(arg_slot_count)` 取 receiver → `getKlass()` → `findMethod` 沿继承链选 override → 用 actual 建帧;`OperandStack` 换 `vector` + `peekRef`。测非 override 调用 + Animal/Dog override 分派。
- [x] **M2 最小 String** — `oops/string_pool.h`(intern,结点式 `unordered_set` 稳定地址),LDC/LDC_W → intern + pushRef 稳定 `std::string*`;**A1/A4 随之关闭**。`string_pool_test` + LDC 驻留测试。
- [x] **M3 native 机制** — `engine/native_registry.h`(key `类/方.描述符`→`NativeFn`),INVOKESTATIC 的 isNative 查表分派;`interpreter_native_test`。
- [x] **M6 System.out/println 符号拦截** — GETSTATIC 认 `java.lang.System.out` 压哨兵、INVOKEVIRTUAL 认 `java.io.PrintStream.println:(Ljava/lang/String;)V`→`std::cout`,均在 resolve 之前。
- [x] **M7 桩接版 hello world 端到端 🎉** — `main.cpp` 加载主类→驱动真 `main`;`interpreter_hello_world_test` 抓 cout 断言 `"Hello, World!\n"`。

---

## B. 小项 / 收尾

- [ ] **`slotCount` switch 后补 `return`** — `-Wreturn-type` 警告(`utilities/descriptor.h`),switch 后加 `return 1;` 或 `std::unreachable()`。
- [ ] **B5.(可选)debug-only tag** — `#ifndef NDEBUG` 下在 `pop*`/`get*` 加 `assert`,开发期类型安全网,Release 零开销。

---

## C. 架构

- [ ] **C2. 拆解释器大 switch** — `engine/interpreter.cpp` 仍是单函数;按指令组拆 TU、抽出 invoke 建帧/return 退帧到 runtime。
- [ ] **C3. constant_pool 命名(可选)** — 命名空间已消歧;可选把 `oops/constant_pool.*` → `runtime_constant_pool.*` 做文件级澄清。

> C4、C5 已完成,见上方 ✅。`method_area.h`/构造仍前置声明 `classfile::ClassFile` 作瞬时参数(良性构建方向,不再处理)。

---

## 🎯 里程碑:桩接版 canonical hello world —— **已达成 🎉**

`HelloWorld.main` 的 `System.out.println("Hello, World!")` 端到端跑通(不加载真 JDK 类)。M1–M7 全部完成,见上方 ✅。

---

## 下一阶段:走向更完整的 JVM

### R. 先重构(在加更多特性前收拾干净)

- [ ] **R1. 抽象符号拦截**(你想做的)—— 把散在 GETSTATIC/INVOKEVIRTUAL/INVOKESPECIAL 里的内联 `if (class_name=="..." && member=="...")` 收进一张**符号拦截表**:`{class,member,descriptor} → handler(op_stack)`,resolve 前统一查表命中即走 handler。可与 M3 的 native 注册表合并成一个"内建方法表"(注意 native 用斜杠 key、符号拦截用点 key,需统一)。Object.<init>/System.out/println 三处都归它。
- [ ] **R2. `Klass::getName()` 的斜杠/点形式统一** —— 现在 `name_` 是斜杠形式,而 loader/resolve 用点形式;native key 与符号拦截 key 因此不一致(R1 会撞上)。定一种规范形式。
- [ ] **C2. 拆解释器大 switch** — 单函数已很长;按指令组拆 TU、抽 invoke 建帧/return 退帧到 runtime。
- [ ] **R3. 极简 `JVM_TRACE`** — `utilities/log.h` 一个 env 门控宏(`JVM_TRACE=1` 时输出到 `std::cerr`),先给解释器循环一条 opcode 执行轨迹(`pc`/op/栈深)+ 类加载/resolve/拦截命中几点。**不引框架、不分 level/category**;错误报告维持异常。VM 长大到多子系统后再演进成 `-Xlog` 式分类日志。
- [ ] **`slotCount` 补 `return`**、**B5 debug tag**(小项,随时)。

### D. 功能(按价值/依赖)

- [ ] **D4. 数组** —— **推荐下一个大特性**。NEWARRAY/ANEWARRAY/\*ALOAD/\*ASTORE/ARRAYLENGTH;解锁真 `String[] args`、真 String(char[]/byte[])、大量普通程序。建在现有 heap 上,自洽。
- [ ] **`<clinit>` 类初始化** — 首次主动使用触发 `<clinit>`(Klass 加 initialized 状态 + 递归守卫);补 INVOKESTATIC/GETSTATIC 的触发点。修正"`static int x = 5;` 仍是 0"这类静态初始化缺口。
- [ ] **D6. 异常处理** — ATHROW + 异常表 + 真 Java 异常,替换会打挂 VM 的 `std::runtime_error`(除零/NPE)。解锁 try/catch。
- [ ] **INVOKEINTERFACE** — 接口方法分派。
- [ ] **D7. GC** — mark-sweep(非移动,直接指针不变);再远才是移动 + oopmap。
- [ ] **旁支:INVOKESTATIC 的 `<clinit>` + native 分支**(现有两处 TODO);`natives.cpp` 进 CMake + `registerBuiltins` 在 main 调。

---

## E. 工程

- [x] **E2. 暂不迁移 Bazel** — 已决策;提速靠 ccache / Ninja / C2 的 TU 拆分。
- [ ] **E3.(可选)`ARCHITECTURE.md`** — 记录分层、slot 选型(64 位裸 union)、引用模型、命名约定、pc 归属、边界检查策略。

---

## 建议下一步

hello world 已达成 🎉。建议:

1. **先重构 R1**:把三处内联符号拦截抽成一张拦截表(你想做的),顺带 R2 统一类名斜杠/点形式——趁只有三处、上下文还热,先收拾干净,再加特性。
2. **再上 D4 数组**:解锁真 `String[]`、真 String、大量普通程序,是最高价值的下一个大特性,且干净地建在现有 heap 上。
3. 之后按需:**`<clinit>` 类初始化**(修静态初始化)→ **D6 异常**(try/catch,别再 crash)→ INVOKEINTERFACE → 最后 **GC(mark-sweep)**。
4. 设计已稳,**E3 `ARCHITECTURE.md`** 是个好时机——把 slot(64 位裸 union)、pc 归属、引用模型、拦截机制、边界检查策略记下来。
