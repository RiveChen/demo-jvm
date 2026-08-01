# demo-jvm 整改路线图

> 基线日期：2026-07-30  
> 文档性质：代码审查后的工程整改指导  
> 适用范围：当前 `main` 分支及工作区中的在研数组改动  
> 核心原则：先保证“错误能够被发现”，再保证“已实现语义正确”，然后改善结构，最后继续扩展 JVM 功能。

---

## 1. 整改目标

本轮整改不是为了尽快增加更多字节码，而是为了把仓库从“功能增长较快的学习项目”提升为“行为可信、可以持续演进的系统项目”。

完成本路线图后，仓库应具备以下特征：

1. 对未实现能力明确失败，不产生静默错误结果。
2. 已声明支持的 JVM 指令符合 JVM 8 语义，尤其是数值边界行为。
3. 测试不只覆盖正常路径，也能验证规范边界和历史回归。
4. 编译、格式、静态检查和 Sanitizer 成为自动化门禁。
5. 解释器不再依赖单个超长函数承载所有执行逻辑。
6. 类加载器行为、模块依赖和文档描述与实际实现一致。
7. 新功能必须在上述基础上继续演进。

### 不属于本轮首要目标的内容

- 追求完整兼容标准 JDK。
- 立即实现 GC、线程、反射或 JNI。
- 为了“看起来更像 HotSpot”而提前引入复杂抽象。
- 在正确性问题尚未关闭前继续大规模增加 opcode。

---

## 2. 当前基线

### 已验证结果

| 项目 | 当前结果 | 判断 |
|---|---:|---:|
| Debug 构建 | 成功 | 可开发 |
| 普通测试 | 403/403 通过 | happy path 基础较好 |
| ASan 测试 | 403/403 通过 | 已覆盖路径未发现明显内存错误 |
| **UBSan 测试** | **403/403 通过** | 已配置 `ENABLE_UBSAN`，整数回绕/浮点转换/除零边界已修复 |
| 编译告警 | 存在 | 尚未达到干净构建 |
| `check-format` | 失败 | 质量门禁未闭环 |
| CI | 缺失 | 提交质量依赖开发者本机 |

### 已确认的正确性风险

> 状态标注：`✅ 已修复` / `⏳ 待处理`。

| # | 初始发现 | 当前状态 | 验收证据 |
|---|---|---:|---|
| 1 | `SIPUSH` 以无符号值读取，`-32768` 被执行为 `32768` | ✅ 已修复 | `LDIV_MinByNegativeOneReturnsMin` 等边界测试 |
| 2 | `F2I/F2L/D2I/D2L` 对 infinity 的处理不符合 JVM 规范 | ✅ 已修复 | `interpreter_conversion_test.cpp` 饱和测试 |
| 3 | Java 整数回绕语义依赖 C++ signed overflow（UB） | ✅ 已修复 | UBSan 403/403 通过 |
| 4 | 多个未实现 opcode 只执行 `break`（静默错误） | ✅ 已修复 | `throwUnsupportedOpcode` fail-fast（带类/方法/PC 上下文） |
| 5 | PC 越过方法末尾被当作正常结束 | ✅ 已修复 | `pc >= code.size()` fail-fast + 入口 `IRETURN/LRETURN/ARETURN/RETURN` 返回 `RunOutcome` 协议测试（`interpreter_entry_return_test.cpp`） |
| 6 | Parent loader 失败后不回落到 child classpath | ⏳ 待处理 | — |
| 7 | 部分测试被注释/错误期望 | ⏳ 待处理 | —（IFNULL/IF_ACMP 等测试待恢复） |

### 文档状态

- `ARCHITECTURE.md` 的设计记录较完整，但部分“当前状态”描述可能随代码演进失效。
- `TODO.md` 同时承担历史记录、缺陷列表和功能路线图，信息密度过高。
- `README.md` 的 feature checklist 已明显落后于实现。
- `docs/evaluation_report.md` 是 2026-01-06 的历史评估，不能继续作为当前状态依据。

---

## 3. 整改总顺序

```text
P0 建立可信基线
  ↓
P1 关闭规范正确性问题
  ↓
P2 建立自动化质量门禁
  ↓
P3 渐进拆分解释器
  ↓
P4 修正类加载与模块边界
  ↓
P5 同步文档与项目定位
  ↓
P6 恢复功能开发
```

除非出现阻塞性缺陷，否则不建议跳过 P1/P2 直接进入数组、异常或 GC。

---

## 4. P0：建立可信基线

### 目标

让后续每项整改都有可重复的输入、失败证据和验收方式，避免“一边修、一边改变判断标准”。

### 工作项

1. 整理当前工作区。
   - 明确 `ARCHITECTURE.md`、`TODO.md`、`src/oops/object.hpp` 的未提交改动属于哪个功能批次。
   - 数组在研代码与正确性整改尽量分开提交。
   - 不把格式化全仓库和功能修复混进同一个提交。

2. 记录标准验证命令。
   - Debug 构建。
   - 全量测试。
   - ASan 测试。
   - 格式检查。
   - 后续加入 UBSan 和 clang-tidy。

3. 为已确认问题先建立失败用例。
   - 负数 `SIPUSH`。
   - 正负 infinity 和超范围有限浮点数转 int/long。
   - `MAX_VALUE + 1`、`MIN_VALUE - 1`、乘法溢出和取负溢出。
   - `MIN_VALUE / -1`、`MIN_VALUE % -1`。
   - 每个未实现 opcode 至少验证会明确失败。

4. 区分三种测试。
   - 规范测试：验证 JVM 规定的行为。
   - 回归测试：绑定曾经出现过的缺陷。
   - 实现测试：验证内部容器、布局和辅助类。

### 退出条件

- 每个已知语义问题都有一个修复前会失败的测试。
- 测试名称描述规范行为，而不是描述当前实现。
- 当前在研数组改动与基础整改的提交边界清晰。
- 团队或个人能够在干净目录中重复执行相同验证流程。

---

## 5. P1：关闭规范正确性问题

> 进度更新（2026-07-31）：
> - **P1.1（未实现 opcode fail-fast）✅ 已完成** —— `throwUnsupportedOpcode` 统一带类/方法/PC 上下文失败。
> - **P1.2（立即数读取）✅ 已完成** —— 符号扩展（BIPUSH/SIPUSH/IINC/分支偏移）已覆盖。
> - **P1.3（整数算术）✅ 已完成** —— 16 处 C++ UB 修复，详见 `docs/design/interpreter.md`「C++ UB 与 JVM 语义」。
> - **P1.4（浮点转整数）✅ 已完成** —— `F2I`/`F2L`/`D2I`/`D2L` 饱和范围检查。
> - **P1.5（结束条件）✅ 已完成** —— `pc = code.size()` fail-fast；入口返回 `RunOutcome`，`executeStaticMethod` 改为单入口 Frame 薄封装（不再依赖伪 caller Frame / 残留 operand stack）。验收见 `interpreter_entry_return_test.cpp`。

P1 是本轮最高优先级。这里没有完成之前，不应把“403 个测试通过”当作实现正确的证据。

### P1.1 未实现 opcode 必须 fail-fast

#### 范围

- 数组 load/store。
- 数组创建与长度。
- `INVOKEDYNAMIC`。
- `ATHROW`。
- `MONITORENTER/MONITOREXIT`。
- 其他当前只包含 TODO 和 `break` 的指令。

#### 指导原则

- 未实现与非法指令是不同错误，应能区分。
- 错误中至少包含 opcode 名称、数值、类名、方法名和 PC。
- 未实现指令不得继续执行其后的 operand bytes。
- 暂不要求构造真正的 Java 异常对象；当前阶段允许使用 VM 层异常终止执行。

#### 验收标准

- 所有未实现 opcode 都有统一且可诊断的失败行为。
- 不再存在“TODO 后直接 break”的解释器分支。
- 测试能够证明失败发生在准确的 opcode 和 PC。

### P1.2 修正立即数读取

#### 范围

- `BIPUSH` 符号扩展。
- `SIPUSH` 符号扩展。
- `IINC` 普通与 WIDE 形式的有符号常量。
- 分支 offset 的有符号读取。

#### 验收标准

- 覆盖每种立即数的最小值、`-1`、`0`、`1` 和最大值。
- 使用 `javap -c` 确认测试 class 确实生成了目标 opcode。
- 不再以“编译器可能不用该指令”为理由禁用可由 javac 稳定生成的测试。

### P1.3 修正整数算术语义

> **状态：✅ 已完成**（2026-07-31），16 处 C++ UB 修复，详见 `docs/design/interpreter.md`。

#### 风险

Java 的 `int` 和 `long` 算术要求按固定宽度二进制补码回绕；C++ signed overflow 是未定义行为，不能依赖编译器“刚好回绕”。

#### 范围

- 加、减、乘。
- 取负。
- `IINC`。
- `MIN_VALUE / -1`。
- `MIN_VALUE % -1`。
- 左移、算术右移和逻辑右移。
- long 到 int 的截断。

#### 指导原则

- 需要明确建立“JVM 有符号值”和“用于无定义行为回绕的无符号位模式”之间的转换边界。
- 不要通过关闭编译器优化或依赖特定编译器行为规避问题。
- 对实现定义行为，例如负数 signed right shift，也应采用显式、可移植的位语义。

#### 验收标准

- GCC 与 Clang 下结果一致。
- UBSan 不报告整数相关未定义行为。
- 覆盖所有零点、符号边界和最大/最小值。

### P1.4 修正浮点到整数转换

> **状态：✅ 已完成**（2026-07-31），`F2I`/`F2L`/`D2I`/`D2L` 范围检查已实现。

#### JVM 语义

- NaN 转换为 0。
- 可表示范围内先向零截断。
- 正方向超出范围或正 infinity 饱和到目标类型最大值。
- 负方向超出范围或负 infinity 饱和到目标类型最小值。

#### 范围

- `F2I`。
- `F2L`。
- `D2I`。
- `D2L`。

#### 验收标准

- 覆盖 NaN、正负零、正负小数、边界附近值、超范围有限值、正负 infinity。
- 删除或修正当前把 infinity 期望为 0 的错误测试。
- C++ 浮点转整数前完成范围判断，避免越界转换本身触发未定义行为。

### P1.5 收紧解释器结束条件

#### 当前风险

PC 到达或越过 code 末尾时，解释器直接返回。这会把缺少 RETURN、错误分支和未消费 operand 等问题伪装为正常结束。

#### 目标行为

- 正常方法结束只能来自合法 return 或 throw/unwind 路径。
- PC 越界应报告 VM 执行错误。
- 测试辅助框架不应要求生产解释器容忍非法方法结束。

#### 验收标准

- 测试 helper 与生产执行语义解耦。
- 缺少 return 的方法会明确失败。
- 非法 branch target 会明确失败。

### P1 完成定义

- 所有新增边界测试通过。
- 普通测试、ASan、UBSan 全部通过。
- 不存在已知“测试通过但规范错误”的用例。
- 对支持与不支持的 opcode 有明确清单。

---

## 6. P2：建立自动化质量门禁

### 目标

将“开发者记得运行检查”升级为“仓库不允许绕过检查”。

### P2.1 零告警构建

> 进度更新（2026-07-31）：`size_t → Jint` 窄化告警已修复（`getSize()` → `size_t` 等）。

#### 工作项

- ~~修复 `size_t` 到 `Jint` 的窄化告警。~~ ✅ 已完成
- 处理未使用字段：实现、移除、标注明确的在研用途，三者选一。
- 第三方依赖告警与本项目告警分离。
- 最终对项目源代码启用 warnings-as-errors。

#### 验收标准

- Debug 和 Release 构建均无项目告警。
- GCC 和 Clang 至少各验证一次。

### P2.2 格式与静态检查

#### 工作项

- 固定或记录 clang-format 主版本。
- 先执行一次独立的全仓库格式化提交。
- 让 `check-format` 在干净分支上通过。
- 修正 `.clang-tidy` 中“Zero Tolerance”注释与实际配置不一致的问题。
- 为 clang-tidy 建立正式 CMake target 或 CI 命令。

#### 验收标准

- `check-format` 通过。
- clang-tidy 对 `src/` 通过。
- 测试代码可以采用单独、合理的规则集，但不能完全绕过检查。

### P2.3 Sanitizer

> 进度更新（2026-07-31）：UBSan 已配置（`ENABLE_UBSAN` + CMakePresets），ASan 已可用（`ENABLE_ASAN`）。
> 注意：UBSan 不启用 `-fno-sanitize-recover`，避免首个 UB 中断整个测试进程导致同文件后续用例丢失。

#### 最小矩阵

| 配置 | 目的 |
|---|---|
| ASan | 越界、use-after-free、部分生命周期问题 |
| UBSan | signed overflow、非法转换、无效 shift 等 |
| Debug assertions | VM 内部不变量 |
| Release | 防止只在 Debug 正常 |

TSan 暂不要求；当前运行时仍为单线程。

### P2.4 CI

> 进度更新（2026-08-01）：最小 Linux CI 已加入 `.github/workflows/ci.yml`：
> - `build-and-test` job：Debug 构建 + 403 测试全量运行（push 与 PR 到 main 均触发）。
> - `sanitizers` job：`asan-ubsan` preset 构建 + 测试（PR 与 main 触发，避免 throwaway 分支每次跑双 Sanitizer）。
> - 额外提供 `workflow_dispatch` 手动触发，开发者可在 feature 分支上从 Actions 页直接运行完整流水线（含 sanitizers），无需先开 PR。
> - 依赖：`ninja-build` + `default-jdk`；GTest 通过 FetchContent 在 CI runner 上联机拉取。
> - 测试 Java class 改用 `javac --release 8`（比 `-source/-target` 更严格，锁定平台 API）。
> - 尚未加入 `check-format`/clang-tidy 门禁：待全仓库格式化并达成格式基线后，再追加 format job。

#### 建议流水线

1. Configure。
2. Build Debug。
3. Test。
4. Check format。
5. Clang-tidy。
6. ASan。
7. UBSan。
8. Build Release。

#### 建议矩阵

- Linux + Clang。
- Linux + GCC。
- macOS 可作为补充，而非唯一平台。
- Java 测试 class 统一使用 `--release 8`，避免只设置 `-source/-target`。

#### 验收标准

- PR 或主分支提交自动运行质量检查。
- 任意必需检查失败时不能被标记为通过。
- CI 从干净环境拉取依赖并成功构建。
- 构建说明与 CI 使用相同命令。

---

## 7. P3：渐进拆分解释器

### 目标

降低新增功能时修改 1600 行主循环的风险，同时保留直观的 switch dispatch，不进行无收益的“大爆炸重写”。

### 重构原则

1. 每次只抽取已经有充分测试覆盖的逻辑。
2. 重构提交不混入新 opcode。
3. 主 switch 最终只负责：
   - 取指。
   - 分派。
   - 更新执行状态。
   - 统一错误上下文。
4. 不急于引入 computed goto、模板元编程或复杂 handler 注册系统。

### 建议抽取顺序

#### 第一步：执行上下文

建立明确的 execution context，集中持有当前 thread、frame、method、constant pool、operand stack、locals 和 PC。

目标是减少每个分支重复取对象，并明确哪些引用会在 push/pop frame 后失效。

#### 第二步：方法调用

统一以下公共逻辑：

- receiver 定位与空检查。
- 参数从 operand stack 搬运到 locals。
- category-2 参数处理。
- caller PC spill。
- callee frame 创建。
- static/special/virtual/interface 的差异点。

#### 第三步：方法返回

统一：

- 各返回类型取值。
- callee frame 弹出。
- 返回值推入 caller。
- caller PC 恢复。
- `<clinit>` 完成状态更新。

#### 第四步：字段与类初始化触发

统一：

- active use 检查。
- 触发指令 PC 回退。
- static/instance 字段的宽值分派。
- NPE 和类型检查。

#### 第五步：纯指令族

按低耦合程度迁移：

- constants。
- local load/store。
- arithmetic/conversion。
- branch/switch。
- stack manipulation。

### 验收标准

- 主解释函数显著缩短，主要表现为薄分派。
- invoke/return 不再存在多份近似复制实现。
- 重构前后的规范测试结果一致。
- 每一步都可以独立提交和回滚。

---

## 8. P4：修正类加载与模块边界

### P4.1 Parent-first delegation

#### 目标流程

1. 检查当前 loader 已加载缓存。
2. 尝试委托 parent。
3. parent 明确表示找不到时，回落到当前 loader 的 classpath。
4. 当前 loader 成功定义后写入自己的缓存。
5. 链接或初始化失败时维护一致状态。

#### 需要验证的场景

- parent 有类、child 也有同名类：parent 优先。
- parent 没有、child 有：child 成功定义。
- parent 和 child 都没有：明确失败。
- 同一个 class name 被不同定义 loader 加载：类型身份不同。
- 循环加载和重复加载不产生半初始化悬挂对象。

### P4.2 依赖方向

当前设计文档声明单向依赖，但 `oops` 的实现和头文件仍直接接触 `classfile` 类型，CMake target 又没有完整表达反向依赖。

#### 整改目标

- CMake target 声明与真实 include/link 依赖一致。
- 不依赖某个 target 偶然导出的根 include path 掩盖隐藏依赖。
- 区分：
  - class 文件解析期数据。
  - link 阶段输入。
  - 运行时长期元数据。

#### 评估方向

- 将 link/build 过程放在 classfile 或独立 linker/builder 层。
- 让 `InstanceKlass` 更接近纯运行时结构。
- `MethodArea` 头文件尽可能使用前置声明，减少重型 include。

这里不要求一次彻底重构；先让依赖真实、可见、可解释。

### 验收标准

- parent-first fallback 行为有完整测试。
- CMake 依赖图与架构文档一致。
- 单个模块不会因为其他 target 偶然提供 include path 才能编译。

---

## 9. P5：同步文档与项目定位

### README

README 应回答新读者最先关心的问题：

1. 这是教育型 JVM，不是生产 JVM。
2. 当前真正支持哪些功能。
3. 当前明确不支持哪些功能。
4. 如何使用推荐工具链构建和测试。
5. 如何运行 canonical Hello World。
6. 如何运行 format、tidy 和 Sanitizer。
7. 项目当前主线是什么。

### ARCHITECTURE

只记录相对稳定的设计决策：

- 模块边界。
- 对象和 slot 表示。
- 类加载、链接、初始化状态机。
- 解释器执行模型。
- 已接受的长期约束。

短期 bug 列表和每日进度不应长期留在架构文档中。

### TODO

建议拆成三类：

- Known issues：已确认缺陷。
- Roadmap：未来功能。
- Completed milestones：压缩后的历史里程碑。

避免在同一文档中反复描述已经完成、又被后续提交修正的旧缺陷。

### 历史评估报告

`docs/evaluation_report.md` 应明确标注为历史快照，避免读者把其中“尚未实现 NEW/INVOKEVIRTUAL/native”等内容当作当前状态。

### 验收标准

- README feature 状态与当前测试和实现一致。
- 文档中的命令在干净环境可执行。
- 架构文档不再列出已经修复的“当前 bug”。
- 历史报告与当前路线图有明确时间边界。

---

## 10. P6：恢复功能开发

只有 P1 和 P2 完成后，才建议把主力重新投入 JVM 功能。

### 功能顺序

1. 数组 Phase B：对象布局与堆分配。
2. 数组 Phase C：创建与长度指令。
3. 数组 Phase D：load/store 与边界检查。
4. 数组 Phase E：协变、store check、main args 等集成。
5. Java 异常模型：`ATHROW`、异常表、栈展开。
6. 最小真实类库，逐步烧掉 stub intercept。
7. 非移动 mark-sweep GC。

### 每个新 opcode 的完成定义

一个 opcode 不能只以“主路径能跑”为完成，至少应包含：

- 正常行为。
- operand stack 前后状态。
- local/constant-pool 交互。
- null、边界和类型错误。
- category-2 情况。
- PC 更新。
- 与方法调用、类初始化或异常路径的交互。
- `javap` 对测试字节码的确认。
- Debug、Release、ASan、UBSan 通过。
- 支持矩阵与文档同步。

---

## 11. 推荐迭代拆分

### Iteration 1：停止静默错误

- 为未实现 opcode 建立统一失败策略。
- 修复 PC 越界被视作正常完成的问题。
- 增加诊断上下文。

**交付结果**：VM 遇到能力边界时可信地失败。

### Iteration 2：数值语义

- 修复 `SIPUSH`。
- 修复浮点转整数。
- 修复整数回绕和除法边界。
- 补齐规范边界测试。

**交付结果**：核心数值指令不再依赖错误测试或 C++ 未定义行为。

### Iteration 3：质量门禁

- 零告警。
- format/tidy。
- ASan/UBSan。
- CI。

**交付结果**：后续回归能够在提交阶段被自动拦截。

### Iteration 4：调用与返回重构

- 执行上下文。
- invoke 公共路径。
- return 公共路径。

**交付结果**：解释器主循环开始变薄，数组和异常实现的结构风险下降。

### Iteration 5：加载器与依赖

- parent fallback。
- loader identity 测试。
- CMake/include 依赖显式化。

**交付结果**：类加载行为和模块结构可解释、可扩展。

### Iteration 6：文档收口

- README。
- ARCHITECTURE。
- TODO。
- 历史报告标识。

**交付结果**：新读者看到的项目状态与代码一致。

---

## 12. 优先级与风险表

| 项目 | 优先级 | 风险 | 原因 |
|---|---|---|---|
| 未实现 opcode 静默继续 | P0 | 极高 | 产生错误结果而非明确失败 |
| JVM 数值语义 | P0 | 极高 | 已确认错误，且包含 C++ UB |
| 错误测试期望 | P0 | 高 | 测试会阻止未来正确修复 |
| PC 非法结束 | P0 | 高 | 掩盖解释器和字节码问题 |
| CI/Sanitizer | P1 | 高 | 无法持续防止回归 |
| 解释器超长函数 | P1 | 中高 | 每个新功能都会放大修改风险 |
| Parent delegation | P1 | 中 | 限制真实 class loader 行为 |
| 模块隐藏依赖 | P1 | 中 | 构建和架构演进脆弱 |
| README/TODO 漂移 | P2 | 中 | 降低项目可信度和维护效率 |
| 数组/异常/GC 新功能 | 后续 | 高 | 应建立在正确性和门禁之上 |

---

## 13. Senior Review 检查清单

每完成一个整改批次，提交前回答：

- 这次修改修复的是规范行为、内部不变量，还是单纯实现偏好？
- 是否先有能够复现问题的测试？
- 测试是否验证 JVM 规范，而不是验证当前实现？
- 是否覆盖最小值、最大值、零、null 和异常路径？
- 是否引入或依赖 C++ 未定义/实现定义行为？
- 未实现的分支是否明确失败？
- 是否同时更新支持矩阵和文档？
- 是否能在 GCC、Clang、Debug、Release、ASan、UBSan 下成立？
- 这个提交是否只承担一个可以清楚描述的目的？
- 如果回滚这个提交，影响边界是否清晰？

---

## 14. 最终完成标准

本轮整改完成时，应同时满足：

- 已支持 opcode 的规范边界测试全部通过。
- 未支持 opcode 统一 fail-fast。
- Debug 与 Release 零项目告警。
- 格式检查和 clang-tidy 通过。
- ASan 与 UBSan 全量测试通过。
- CI 自动执行所有必要门禁。
- 解释器调用与返回逻辑完成去重。
- parent-first class loading 具备 fallback。
- 模块依赖与 CMake 声明一致。
- README、ARCHITECTURE、TODO 与实现同步。
- 后续数组功能可以在不扩大现有技术债的前提下继续开发。

达到这些标准后，这个仓库才真正从“测试很多的学习实现”迈入“行为可信、可以持续演进的系统项目”阶段。
