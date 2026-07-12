# demo-jvm 待办清单

_更新于 2026-07-12。`[x]` 已完成,`[ ]` 待办,引用处标注 `文件:行`。_

---

## ✅ 已完成

- [x] **descriptor 模块 + 单测**(`utilities/descriptor.*`、`tests/modules/utilities/descriptor_test.cpp`);带游标原语 + 边界检查,原 A5 越界问题随之解决。
- [x] **Slot 去 tag 重构**:`slot.h` 退化为裸 union(B4);GETSTATIC/PUTSTATIC 改按 descriptor + `pushWide`/`popWide` 分派、不再判 tag(B2);`getStaticSlot` 去掉 PADDING 抛异常(B3);long/double 两槽仅作计数(B6)。
- [x] **静态字段默认值**:经 `statics_.resize()` 值初始化落地(全零 = 所有 Java 默认值)(B1)。
- [x] **Method/Field 身份与缓存分离**:`getDescriptor()` 返回字符串做**身份**(find\* 比字符串,重载解析无损),另存缓存的 `MethodType`/`TypeKind` 供执行期用。
- [x] **命名空间/目录/CMake target 三向对齐**:`interpreter/`→`engine/`(`jvm_engine`)、ns `class_loader`→`classfile`、测试 `common/`→`utilities/`、`jvm::common` 折入 `jvm`、`jvm_utilities` INTERFACE→STATIC。
- [x] **`.DS_Store` 入 `.gitignore`**(E1)。
- [x] **A2 long/double 传参已修** — INVOKESTATIC 改单趟反向 + `setWide`/`popWide` typed 转移,`arg_slot_count` 缓存进 `MethodType`。⚠️ 见下方"待补测试"——修复尚未被测试验证。

---

## A. 确定性 bug(仍开放)

- [ ] **A1. LDC 悬垂指针** — 仍在(`engine/interpreter.cpp:135`,`c_str()` 指向按值返回的临时 CP 项)。已知因 String 对象未实现暂缓。停靠方案:`getConstant` 改按引用返回、指向 CP owned 字符串,先去 UB。
- [ ] **A3. `getSize()` 截断 `size_t`→`U2`** — `runtime/operand_stack.h:28`、`runtime/local_variables.h:29`,改返回 `size_t`。
- [ ] **A4. LDC 与 LDC_W 字符串处理不一致** — LDC 压 `c_str`、LDC_W 压 `nullptr`(`engine/interpreter.cpp:135,148`);随 A1 一起处理。

### 待补测试(验证缺口)

- [ ] **long/double 静态调用测试** — A2 修复(typed 传参)目前**无测试覆盖**,现有 invoke 测试仅 int(factorial),旧的裸槽版也能过。需在 `tests/data/java/MethodInvocationTest.java` 加 long/double/混合参数的静态方法 + 用例并重编 `.class`。**建议随 A2 修复一起 commit,让该提交自带验证。**

---

## B. Slot 重构收尾(小项)

- [ ] **B5.(可选)debug-only tag** — `#ifndef NDEBUG` 下在 `pop*`/`get*` 加 `assert`,作开发期类型安全网,Release 零开销。
- [ ] **`slotCount` switch 后补 `return`** — `-Wreturn-type` 警告(`utilities/descriptor.h`),switch 后加 `return 1;` 或 `std::unreachable()`。
- [ ] **`getStaticSlot` 加越界检查** — 去掉 PADDING 抛异常后目前直接 `return statics_[index]`,无边界保护(`oops/klass.h`)。

---

## C. 架构

- [ ] **C1. PC 收进 Frame —— 未做** — `runtime/thread.h` 仍有全局 `pc_`/`getPC`/`setPC`/`incrementPC`;`Frame` 虽有 `caller_pc_`,但双源仍在。删全局 pc,单一来源,为异常展开铺路。
- [ ] **C2. 拆解释器大 switch** — `engine/interpreter.cpp` 仍是单函数;按指令组拆 TU、抽出 invoke 建帧/return 退帧到 runtime。
- [ ] **C3. constant_pool 命名** — 命名空间已消歧(`jvm::classfile` vs `jvm::oops`);可选:`oops/constant_pool.*` → `runtime_constant_pool.*` 做文件级澄清。
- [ ] **C4. Klass→ClassFile 反指针耦合** — 仍在(`oops/klass.h`、`method_area.h` 前置声明 `jvm::classfile`)。link 期把所需数据烘焙进 Klass 后解除;这也会断开 oops↔classfile 的源码纠缠。
- [ ] **C5.(新)迁移 oops 测试** — `klass_test`/`method_area_test`/`constant_pool_test` 现寄在 `tests/modules/runtime/`,实测 `oops` 模块。新建 `tests/modules/oops/`(`test_oops`,link `jvm_oops jvm_classfile` + `compile_test_classes`),`test_runtime` 只留 `local_variables`/`operand_stack`(纯单元、无需 `.class` 机制)。

---

## D. 功能缺口(里程碑,按依赖顺序)

- [ ] **D1. heap / Object 模型(先无 GC)** — bump-pointer arena,只分配不回收;对象头 = `Klass*` + 预留一个 GC 元数据字,后接 `instance_slot_count` 个字段槽(零初始化)。`memory/heap.h` 仍空,是当前最大缺口。
- [ ] **D2. 引用表示** — 无 GC/非移动阶段用**直接指针**指进 arena(不移动即稳定,最简单);`Jref` 保持 typedef、解引用走小 API,以便日后可换。仅当选择移动式 GC 才回头换句柄/oopmap。
- [ ] **D3. 实例字段与对象创建** — GETFIELD/PUTFIELD/NEW。
- [ ] **D4. 数组** — NEWARRAY/ANEWARRAY/\*ALOAD/\*ASTORE/ARRAYLENGTH。
- [ ] **D5. 其余 invoke** — INVOKEVIRTUAL/INVOKESPECIAL/INVOKEINTERFACE(需对象模型 + 方法分派)。
- [ ] **D6. 异常处理** — ATHROW + 异常表 + 真正的 Java 异常,替换 `std::runtime_error`(如除零 crash VM)。
- [ ] **D7. GC** — oopmap(由 StackMapTable / 抽象解释推导);不复活 per-slot tag。

---

## 🎯 里程碑:桩接版 canonical hello world

跑通 `HelloWorld.main` 里的 `System.out.println("Hello, World!")`,用 C++ 把字打到 stdout(不加载真 JDK 类)。按序:

- [ ] **M1. 无 GC 堆 + Object**(= D1/D2):bump arena + 直接指针 + 对象头 `Klass*`。
- [ ] **M2. 最小 String** — LDC 产出 String 对象(可内部挂 `std::string`,暂跳过真 char[]);修 A1。
- [ ] **M3. native 方法机制** — invoke 流水线的 native 分支 + 注册表(`类.方法.描述符` → C++ 函数)。
- [ ] **M4. INVOKESTATIC 补全** — typed 传参已完成(A2);剩:加类初始化(`<clinit>`)触发 + native 分支(`engine/interpreter.cpp:1245,1251` 两处 TODO)。
- [ ] **M5. INVOKEVIRTUAL + 虚分派** — locals[0] 放 receiver,按实际类分派;复用 M4 骨架。
- [ ] **M6. GETSTATIC `System.out` 拦截 + `PrintStream.println` 桩接** — 返回哨兵 PrintStream,println 走 M3 native → `std::cout`。
- [ ] **M7. 跑通 HelloWorld** — 端到端,输出 `Hello, World!`。

可跳过(桩接版不需要):真 JDK java.lang/java.io、完整 `<clinit>`、异常、通用数组、char[] 内部。

---

## E. 工程

- [x] **E2. 暂不迁移 Bazel** — 已决策;提速优先靠 ccache / Ninja / C2 的 TU 拆分。
- [ ] **E3.(可选)`ARCHITECTURE.md`** — 记录分层、slot 选型(64 位裸 union)、引用模型、命名约定。

---

## 建议下一步

`A2`(typed 传参,已有 `getSignature` 可用)→ `C1`(PC 进 Frame)→ `C5`(迁 oops 测试,顺手清理)→ 再进入 `D1` 打通 heap/Object。`A3`/`A4`/两个 B 小项/`slotCount return` 可随时穿插。
