# demo-jvm 待办清单

按优先级与阶段排列。`[ ]` 待办,引用处标注了 `文件:行`。

---

## A. 确定性 bug(先修,影响正确性 / UB)

- [ ] **A1. LDC 字符串压栈是悬垂指针** — `interpreter.cpp:178-179`
  `getConstant()` 按值返回 `RtCpInfo`(`oops/constant_pool.h:44`),`c_str()` 指向 case 块内临时变量,块结束即悬垂。需指向生命周期稳定的对象(等 String/字符串常量池),或暂存到 method area。

- [ ] **A2. long/double 作方法参数落错槽** — `interpreter.cpp:1337-1343`
  操作数栈把值放"高格(栈顶)"、局部表放"低格",逐槽平移导致值进了 `index+1`,被调方 `LLOAD_0` 读到 padding。**统一 category-2 的低/高格约定**(建议:值固定放低格,高格恒 padding),push/pop/传参三处一致。

- [ ] **A3. `getSize()` 把 `size_t` 截断成 `U2`** — `operand_stack.h:28`、`local_variables.h:29`
  改返回 `size_t`。

- [ ] **A4. LDC 与 LDC_W 对字符串行为不一致** — `interpreter.cpp:168-194`
  一个压指针、一个压 nullptr,统一处理。

- [ ] **A5. `calculateArgSlotCount` descriptor 解析无越界保护** — `interpreter.cpp:27,39`
  `while (descriptor[i] != ';')` 遇畸形 descriptor 会读越界,加边界检查。

---

## B. Slot 重构(已确定方向:删 tag,物理槽 = 机器字)

- [ ] **B1. 静态字段改为类准备阶段 eager 默认初始化** — `oops/klass.cpp:148-171`
  resize 后按 descriptor 写默认值(`I→0` / `F→0.0f` / `J→0L` / `D→0.0` / 引用→null)。这是规范 preparation 语义,也是删 tag 的前提。

- [ ] **B2. GETSTATIC/PUTSTATIC 改为纯按 descriptor 派发** — `interpreter.cpp:1249-1301`
  去掉所有 `slot.tag` 判断与懒初始化。

- [ ] **B3. `getStaticSlot` 去掉 PADDING 抛异常** — `oops/klass.h:45-53`
  换成纯越界检查;"long/double 占两槽"交给 `slot_index_` 计数(`klass.cpp:161` 已 +2)。

- [ ] **B4. 删除 `Slot.tag`,退化为裸 union(一个机器字)** — `utilities/slot.h`
  物理槽宽保持 64 位机器字(与 HotSpot 一致,引用天然放得下)。同步简化 `operand_stack.h` / `local_variables.h` 的 push/set。

- [ ] **B5.(可选)保留 debug-only tag** — `#ifndef NDEBUG` 下在 `pop*/get*` 加 `assert(tag==...)`,Release 零开销,仅作开发期类型安全网。

- [ ] **B6. long/double 的"两个 slot"统一当作计数/索引规则**,不再用运行时 tag 识别 padding。

---

## C. 架构 / 模块划分

- [ ] **C1. PC 收进 Frame** — `runtime/frame.h`、`runtime/thread.h:18-20`、`interpreter.cpp:66`
  删除 Thread 全局 `pc_` 与死代码 `incrementPC`,单一来源,为异常栈展开铺路。

- [ ] **C2. 拆解巨型解释器 switch** — `interpreter.cpp`(~1400 行单函数)
  按指令组(loads/stores/arith/control/invoke…)拆成多个 TU,函数表或 computed-goto 派发;把 invoke 建帧/传参、return 退帧抽进 runtime。提升编译并行度与可测性。

- [ ] **C3. 两套 constant_pool 命名消歧** — `classfile/constant_pool.*` vs `oops/constant_pool.*`
  概念分开是对的,命名上显式区分(raw cp_info vs RuntimeConstantPool)。

- [ ] **C4. 降低 Klass→ClassFile 回指耦合** — `oops/klass.h:57`
  link 阶段把所需数据"烘焙"进 Klass,减少生命周期纠缠。

---

## D. 功能缺口(里程碑,按依赖顺序)

- [ ] **D1. 实现 heap / Object 模型** — `memory/heap.h`(目前空)
  当前最大缺口,解锁字段与对象;同时定义 `Jref` 真实类型(现为 `void*`,`utilities/types.h:22`)。

- [ ] **D2. 引用表示选型:先用句柄表**(移动 GC 最简单正确的路径),物理槽仍保持机器字宽。

- [ ] **D3. 实例字段与对象创建** — GETFIELD/PUTFIELD/NEW(`interpreter.cpp:1302-1373`)。

- [ ] **D4. 数组** — NEWARRAY/ANEWARRAY/*ALOAD/*ASTORE/ARRAYLENGTH(`interpreter.cpp:315-338,450-473,1407-1418`)。

- [ ] **D5. 其余 invoke** — INVOKEVIRTUAL/INVOKESPECIAL/INVOKEINTERFACE(需对象模型 + 方法解析/分派)。

- [ ] **D6. 异常处理** — ATHROW + 异常表 + 真正的 Java 异常,替换现在的 `std::runtime_error`(如除零 `interpreter.cpp:625` 会直接打挂 VM)。

- [ ] **D7. GC** — 实现 oopmap(由 StackMapTable / 抽象解释推导);**不要**复活 per-slot tag。

---

## E. 构建 / 工程卫生

- [ ] **E1. `.gitignore` 加入 `.DS_Store`** — `src/.DS_Store`、`tests/.DS_Store` 已被纳入版本管理。

- [ ] **E2. 暂不迁移 Bazel**。当前 CMake 已足够干净;想提速优先:CI 加 ccache、继续用 Ninja、靠 C2 的 TU 拆分提并行度。迁移信号:引入第二语言 / 需远程缓存 / 仓库大到增量构建明显变慢。

- [ ] **E3.(可选)补一份 `ARCHITECTURE.md`** 记录分层、slot 选型与引用模型决策。

---

## 建议执行顺序

`A2 → B1 → B2 → B3 → B4 → C1`(先把 slot/PC 这层地基理顺并修掉确定性 bug)→ `D1 → D2 → D3/D4/D5`(打通对象与堆)→ `D6 → D7`(异常与 GC)。`A1/A3/A4/A5`、`E1` 可随时穿插。
