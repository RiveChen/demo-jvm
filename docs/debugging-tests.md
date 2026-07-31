# 调试单元测试

本文介绍如何在本项目中调试单元测试（GoogleTest）。

## 快速开始

1. **构建**：确认 `build/` 已配置并构建成功
   ```sh
   cmake --preset default
   cmake --build build
   ```
2. **设置断点**：在测试代码或源代码中点击行号左侧
3. **选择调试配置**：按 `F5`，选择对应的测试配置

## 测试可执行文件

CMake 按模块生成测试二进制，输出到 `build/bin/`：

| 二进制 | 覆盖模块 |
|--------|---------|
| `test_utilities` | `utilities/`（descriptor、endian、access_flags 等） |
| `test_oops` | `oops/`（klass、method_area、constant_pool、string_pool、array_klass） |
| `test_memory` | `memory/`（heap） |
| `test_class_loader` | `classfile/`（byte_reader、class_file、class_loader） |
| `test_runtime` | `runtime/`（local_variables、operand_stack） |
| `test_sanity` | `sanity/`（环境检查） |
| `test_interpreter_*` | `engine/interpreter` 各专题测试 |

## 可用的调试配置（`.vscode/launch.json`）

- **Debug Showcase** / **Debug HelloWorld**：运行主程序 `demo-jvm`
- **Debug Single Test**：选择任意 `test_*` 二进制 + 输入 `--gtest_filter`
- **Debug Runtime Tests** / **Debug ClassLoader Tests** / **Debug Oops Tests** / **Debug Memory Tests** / **Debug Sanity Tests**：运行对应模块测试套件

### 示例：调试单个测试用例

1. 按 `F5` 选择 **Debug Single Test**
2. 选择测试二进制（如 `test_interpreter_arithmetic`）
3. 输入测试过滤器，例如：
   - `InterpreterArithmeticTest.IADD_MaxPlusOneWrapsToMin`
   - `*ClinitTest.*`
4. 断点会在目标测试处命中

> `TEST_CLASS_PATH` 已由 launch.json 注入为 `${workspaceFolder}/build/test_classes`，无需手动设置。

## 用 C++ TestMate 在 Testing 面板调试

项目已配置 C++ TestMate：

- **发现测试**：Testing 面板自动列出 `build/bin/*test*` 中的 GoogleTest 用例
- **运行/调试**：点击用例前的 ▶ / 虫子图标
- **切换 build**：通过 CMake Tools 状态栏切换 preset（default/asan/ubsan）后，TestMate 扫描相应 build 目录

## 调试技巧

### 断点
- **条件断点**：右键断点 → Edit Breakpoint，如 `pc == 42`
- **日志断点**：右键 → "Logpoint"，输出变量而不中断

### 常用键
- **F5**：继续；**F10**：单步跳过；**F11**：单步进入；**Shift+F11**：跳出；**Shift+F5**：停止

### 监视
- 变量面板可查看 `klass`、`method`、`op_stack` 等
- 调试控制台可输入表达式（如 `method->getName()`）

## 常见问题

### 测试 class 文件找不到
- 确认已构建测试数据：`cmake --build build --target compile_test_classes`
- 确认 `build/test_classes/` 存在且有 `.class` 文件

### 断点不生效
- 使用 Debug 配置（preset 默认 `CMAKE_BUILD_TYPE=Debug`）
- 确认未开启优化（sanitizer preset 含 `-g`）
- 清理重建：`cmake --build build --target clean && cmake --build build`

### 调试器无法启动
- 确认安装了 CodeLLDB 插件（`vadimcn.vscode-lldb`）
- 确认 `build/bin/test_*` 存在且有执行权限