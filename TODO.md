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
- [x] **R1 符号拦截抽离** — 独立 `StubIntercepts` 表 + `RuntimeConstantPool::symbolicKey` + `tryStubIntercept`,三站点统一;与 NativeRegistry 分开;顶部 burn-down 列表。修了 symbolicKey 对已解析目标类的 `bad_variant_access`。
- [x] **R2 类名斜杠统一** — 见下(斜杠为唯一内部规范,loadClass 入口归一化)。
- [x] **R3 logger** — `utilities/logger.h`:5 级 + `JVM_LOG_LEVEL` 运行期 + `JVM_LOG_MAX_LEVEL` 编译期裁剪 + `file:line`;VM 生命周期已铺日志。
- [x] **WIDE 指令** — 宽索引前缀。
- [x] **D5 invoke 全齐** — INVOKESTATIC/SPECIAL/VIRTUAL/**INTERFACE** 全实现(接口按 receiver 实际类分派);修了接口解析/链接索引顺序。
- [x] **CHECKCAST / INSTANCEOF** — `isInstanceOf` 沿继承/接口链判断。
- [x] **stub 方法家桶(Tier 1 部分)** — `System.err` 哨兵;`PrintStream.print/println` 各原始类型(I/J/F/D/…)+ String,按 receiver 哨兵选 cout/cerr。

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

### R. 重构 / 收尾(R1/R2/R3 已完成,见上方 ✅)

- [ ] **C2. 拆解释器大 switch** — 单函数已很长;方案:保留 switch 做薄分派,执行态放 Interpreter 成员(或 ExecContext),handler 按类别分散到 `engine/ops/*.cpp` 并参数化指令族;**先抽 invoke 建帧 + return 退帧两块**(最肥、最重复),再搬机械组。不 computed-goto。
- [ ] **(远期)NativeRegistry 向 JNI 演进** — native fn 签名从裸 `OperandStack&` 走向 `JNIEnv*` + 参数编组。独立于"删拦截"。
- [ ] **logger 小改**:`LOG_*` 宏在判等级**之前**短路(`do{ if(enabled) log(...);}while(0)`),运行期关闭时不求值实参(热路径 opcode trace 有用)。
- [ ] **`slotCount` 补 `return`**、**B5 debug tag**(小项,随时)。

### D. 功能(按价值/依赖)

- [ ] **D4. 数组** —— **推荐下一个大特性**。NEWARRAY/ANEWARRAY/\*ALOAD/\*ASTORE/ARRAYLENGTH;解锁真 `String[] args`、真 String(char[])、大量普通程序。建在现有 heap 上,自洽。也是"自写真类"路线的硬前提。
- [ ] **`<clinit>` 类初始化** —— **已可实现(自洽,前提就绪)**。加 Klass 初始化状态(`uninit→in-progress→inited`)+ 递归守卫;NEW/GETSTATIC/PUTSTATIC/INVOKESTATIC + main 入口前触发;super 先于 sub;复用现有 invoke/帧机制跑 `<clinit>()V`。**唯一设计决定**:如何"指令中途"跑 clinit(嵌套 interpret 调用 vs 压帧后回退 pc 重执行触发指令)。不依赖数组/异常。修正"`static int x=5` 仍是 0"缺口,也是"自写真类"的前提。
- [ ] **D6. 异常处理** — ATHROW + 异常表 + 真 Java 异常,替换会打挂 VM 的 `std::runtime_error`(除零/NPE/越界)。解锁 try/catch,也是真 String 方法错误路径的前提。
- [ ] **stub 家桶续做(Tier 1/2)** — String 只读方法(length/charAt/equals/hashCode…)、静态工具(Integer.parseInt、Math.\*、System.currentTimeMillis;需 INVOKESTATIC 加 `tryStubIntercept`)、返回 String 的方法(intern 结果)、`+` 拼接(StringBuilder 桩 + NEW 拦截)。纯加 handler,不碰 switch。
- [ ] **自写最小真类** — 编译自己的最小 `Object/String/StringBuilder.java`(char[] 后端、无 Unsafe),加载真字节码逐条烧掉对应 stub。前提:D4 数组 + `<clinit>` + 一小撮 native(arraycopy)+(可选)D6。
- [ ] **D7. GC** — mark-sweep(非移动,直接指针不变);再远才是移动 + oopmap。

---

## E. 工程

- [x] **E2. 暂不迁移 Bazel** — 已决策;提速靠 ccache / Ninja / C2 的 TU 拆分。
- [x] **E3. `ARCHITECTURE.md`** — 已记录分层、slot(64 位裸 union)、pc 归属、引用/堆、descriptor、常量池(C4)、类加载、native/拦截、String 桩、边界检查与索引类型、错误处理、已知限制。

---

## 建议下一步

hello world + R1/R2/R3 + 四种 invoke + CHECKCAST/INSTANCEOF + WIDE + print 家桶都已完成 🎉。接下来两条主线,可并行/择一:

1. **广度(能跑更多程序,纯加 handler,不碰 switch)**:续做 **stub 家桶 Tier 1/2**(String 只读方法、Integer/Math 静态、`+` 拼接)。ROI 高、低风险。
2. **深度(走向真类/正确性)**:**`<clinit>`**(已可做、自洽,修静态初始化)→ **D4 数组**(硬前提)→ **D6 异常** → **自写最小真类**(逐条烧 stub)→ 最后 **GC(mark-sweep)**。

**`<clinit>` 现在就能做**(不依赖数组/异常),若你的测试程序开始用 `static int x=5;` 这类初始化,它就是最该补的正确性缺口。**C2 拆 switch** 建议放在这些特性之后(handler 更多、分组更有料);`slotCount return`、logger 短路那两个小项随时穿插。
