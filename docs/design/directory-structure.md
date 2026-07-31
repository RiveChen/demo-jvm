# Directory Structure

本项目结构如下：

## 根目录

- **配置文件**:
  - `CMakeLists.txt`: 根 CMake 配置，定义项目设置、编译选项、Sanitizer 开关和 GoogleTest 集成
  - `CMakePresets.json`: CMake 预设（default / asan / ubsan / asan-ubsan，共用 `build/`）
  - `.clang-format`: 代码格式化配置
  - `.clang-tidy`: clang-tidy 静态分析规则
  - `.clangd`: clangd 语言服务器配置（诊断抑制 / clang-tidy 联动 / 第三方目录排除）
  - `.editorconfig`: 跨编辑器风格统一
  - `.gitignore`: Git 忽略规则
  - `ARCHITECTURE.md`: 架构与设计决策
  - `TODO.md`: 待办清单与路线图
  - `README.md`: 项目说明

## 主要目录

### `src/` — 源码

依赖方向：`engine → runtime → oops → utilities`；`classfile → oops → utilities`；`memory → oops → utilities`。

- `src/CMakeLists.txt`: 聚合子模块 + 主程序 `demo-jvm`
- `src/main.cpp`: 主程序入口

| 目录 | CMake target | 职责 |
|------|-------------|------|
| `utilities/` | `jvm_utilities` | 叶子原语：`types`(U1/U2/Jint…)、`Slot`、`descriptor` 解析器、`basic_type`、`access_flags`、`endian`、`logger`。无依赖 |
| `oops/` | `jvm_oops` | 运行期元数据：`Klass`/`Method`/`Field`/`RuntimeConstantPool`/`MethodArea`/`Object`/`StringPool` |
| `classfile/` | `jvm_classfile` | `.class` 解析：`ByteReader`、`ClassFileParser`、`ClassFile`、原始 `ConstantPool`、`ClassLoader` |
| `runtime/` | `jvm_runtime` | 执行状态：`Frame`、`OperandStack`、`LocalVariables`、`Thread`、`Stack` |
| `engine/` | `jvm_engine` | 解释器：字节码分派、`NativeRegistry`、`StubIntercepts` |
| `memory/` | `jvm_memory` | 堆：无 GC 的 bump arena |

### `include/` — JNI 头文件

当前为空（JNI 尚未实现，`ACC_NATIVE` 走 `NativeRegistry` + 符号拦截）。

### `tests/` — 测试

- `tests/CMakeLists.txt`: 聚合测试子目录（含 GoogleTest）
- `tests/data/`: 测试用 Java 源文件 + 编译产物
  - `data/java/`: 各测试模块的 `.java` 源文件
  - `data/java/lang/`: 桩类（如 `java/lang/System`）
- `tests/sanity/`: 环境检查测试
- `tests/modules/`: 按源码模块组织的单元测试
  - `utilities/`、`memory/`、`oops/`、`classfile/`、`runtime/`、`engine/`
- 测试二进制输出到 `build/bin/test_*`（由 CMake 生成）

### `docs/` — 文档

- `docs/design/`: 设计文档（`interpreter.md`、`directory-structure.md` 等）
- `docs/learning/`: 学习笔记（工具链、JVM 字节码、C++ 等）
- `docs/evaluation_report.md`: ⚠️ 历史评估快照（2026-01-06），不代表当前状态
- `docs/remediation-roadmap.md`: 整改路线图（P0–P6）
- `docs/debugging-tests.md`: 测试调试指南

### `build/` — 构建产物（已 .gitignore）

CMake Presets 统一输出到 `build/`：
- `build/bin/`: 可执行文件（`demo-jvm`、`test_*`）
- `build/lib/`: 静态库
- `build/_deps/`: 外部依赖（GoogleTest）
- `build/test_classes/`: 测试用 `.class` 文件