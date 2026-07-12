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
- [x] **INVOKESPECIAL** — 镜像 INVOKESTATIC + receiver 进 slot 0(`pos = arg_slot_count + 1`);`java.lang.Object.<init>()V` 符号层特判 no-op(未加载,resolve 前 peek SymRef)。`tests/modules/engine/interpreter_object_test.cpp` 端到端测 NEW→`<init>`→PUTFIELD→GETFIELD(int/long/double/两对象独立)。

---

## A. 确定性 bug(仍开放)

- [ ] **A1. LDC 悬垂指针** — `c_str()` 指向按值返回的临时 CP 项(`engine/interpreter.cpp` LDC 处)。因 String 对象未实现暂缓;停靠方案:`getConstant` 改按引用返回、指向 CP owned 字符串,先去 UB。随 M2 一起做。
- [ ] **A4. LDC 与 LDC_W 字符串处理不一致** — LDC 压 `c_str`、LDC_W 压 `nullptr`;随 A1/M2 一起处理。

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

## D. 功能缺口(里程碑,按依赖顺序)

- [ ] **D1. heap / Object 模型(先无 GC)** — bump-pointer arena,只分配不回收;对象头 = `Klass*` + 预留 GC 元数据字 + `instance_slot_count` 个字段槽(零初始化)。`memory/heap.h` 仍空,当前最大缺口。
- [ ] **D2. 引用表示** — 无 GC/非移动阶段用**直接指针**指进 arena;`Jref` 保持 typedef、解引用走小 API。仅当选移动式 GC 才换句柄/oopmap。
- [ ] **D3. 实例字段与对象创建** — GETFIELD/PUTFIELD/NEW。
- [ ] **D4. 数组** — NEWARRAY/ANEWARRAY/\*ALOAD/\*ASTORE/ARRAYLENGTH。
- [ ] **D5. 其余 invoke** — INVOKEVIRTUAL/INVOKESPECIAL/INVOKEINTERFACE。
- [ ] **D6. 异常处理** — ATHROW + 异常表 + 真正的 Java 异常,替换 `std::runtime_error`。
- [ ] **D7. GC** — oopmap(StackMapTable / 抽象解释推导);不复活 per-slot tag。

---

## 🎯 里程碑:桩接版 canonical hello world

跑通 `HelloWorld.main` 的 `System.out.println("Hello, World!")`,用 C++ 把字打到 stdout(不加载真 JDK 类)。按序:

- [x] **M1. 无 GC 堆 + Object** — 见上方 ✅。外加 NEW/GETFIELD/PUTFIELD/INVOKESPECIAL 已通,`new` + 字段读写端到端可跑。
- [ ] **M5. INVOKEVIRTUAL + 虚分派** —— **下一步**。receiver 进 slot 0(镜像 INVOKESPECIAL),但按 `obj->getKlass()` 的**实际类**沿继承链 findMethod 分派。可用真实对象的实例方法(如 `obj.add(1,2)`)端到端测,无需桩。
- [ ] **M2. 最小 String** — LDC 产出 String 对象(内部挂 `std::string`,跳过真 char[]);修 A1/A4。
- [ ] **M3. native 方法机制** — invoke 流水线 native 分支 + 注册表(`类.方法.描述符` → C++ 函数)。
- [ ] **M6. GETSTATIC `System.out` 拦截 + `PrintStream.println` 桩接** — 返回哨兵 PrintStream,println 走 M3 native → `std::cout`(在 INVOKEVIRTUAL 里拦截)。
- [ ] **M7. 跑通 HelloWorld** — 端到端,输出 `Hello, World!`。
- [ ] **(旁支)INVOKESTATIC 补全** — `<clinit>` 触发 + native 分支(`engine/interpreter.cpp` 两处 TODO);hello world 不强依赖,择机补。

可跳过(桩接版不需要):真 JDK java.lang/java.io、完整 `<clinit>`、异常、通用数组、char[] 内部。

---

## E. 工程

- [x] **E2. 暂不迁移 Bazel** — 已决策;提速靠 ccache / Ninja / C2 的 TU 拆分。
- [ ] **E3.(可选)`ARCHITECTURE.md`** — 记录分层、slot 选型(64 位裸 union)、引用模型、命名约定、pc 归属、边界检查策略。

---

## 建议下一步

M1(堆/Object)+ NEW/字段/INVOKESPECIAL 已通、全绿。接下来:

1. **M5 INVOKEVIRTUAL + 虚分派**(最后一个"正常"invoke)—— receiver 进 slot 0(照 INVOKESPECIAL),但按 `obj->getKlass()` 实际类沿继承链 findMethod。用真实对象的实例方法端到端测,干净、无桩。
2. 然后进"打印层":**M2 最小 String**(LDC 出 String,顺带修 A1/A4)→ **M3 native 机制** → **M6 GETSTATIC `System.out` + `println` 拦截到 `std::cout`** → **M7 跑通 HelloWorld**。

`slotCount return` 小项可随时穿插。
