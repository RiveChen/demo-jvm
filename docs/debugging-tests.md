# 在 Cursor IDE 中调试单元测试

## 快速开始

1. **设置断点**：在测试代码或源代码中点击行号左侧设置断点
2. **选择调试配置**：按 `F5` 或点击调试面板，选择相应的测试配置
3. **开始调试**：调试器会启动并停在断点处

## 可用的调试配置

### 1. Debug Runtime Tests
- 运行所有 runtime 模块的测试
- 可执行文件：`build/bin/test_runtime`
- 包含：`klass_test`, `method_area_test`, `constant_pool_test`, `operand_stack_test`, `local_variables_test`

### 2. Debug ClassLoader Tests
- 运行所有 class_loader 模块的测试
- 可执行文件：`build/bin/test_class_loader`
- 包含：`class_loader_test`, `class_file_test`, `byte_reader_test`

### 3. Debug Single Test (Runtime)
- 运行单个 runtime 测试用例
- 启动时会提示输入测试过滤器（test filter）
- 示例过滤器：
  - `KlassTest.FindMethod` - 运行 KlassTest 的 FindMethod 测试
  - `ConstantPoolTest.*` - 运行所有 ConstantPoolTest 测试
  - `*Test.Resolve*` - 运行所有包含 "Resolve" 的测试

### 4. Debug Single Test (ClassLoader)
- 运行单个 class_loader 测试用例
- 使用方式同上

## 调试技巧

### 设置断点
- **普通断点**：点击行号左侧
- **条件断点**：右键点击断点，设置条件（如 `klass != nullptr`）
- **日志断点**：右键点击断点，选择 "Logpoint"，可以输出变量值而不停止执行

### 调试控制
- **F5**：继续执行
- **F10**：单步跳过（Step Over）
- **F11**：单步进入（Step Into）
- **Shift+F11**：单步跳出（Step Out）
- **Shift+F5**：停止调试

### 查看变量
- **变量面板**：自动显示当前作用域的变量
- **监视面板**：添加表达式进行监视
- **调试控制台**：可以输入表达式查看值（如 `klass->getClassFile()`）

### 调试特定测试用例

#### 方法 1：使用测试过滤器
1. 选择 "Debug Single Test" 配置
2. 输入测试过滤器，例如：
   ```
   KlassTest.FindMethod
   ConstantPoolTest.ResolveClassCachesResult
   ```

#### 方法 2：在代码中设置断点
1. 在测试用例的第一行设置断点
2. 选择对应的测试套件配置（如 "Debug Runtime Tests"）
3. 运行调试，程序会在断点处停止
4. 使用 "Continue" 跳过不需要的测试

## 常见问题

### 测试类文件找不到
- 确保已运行 `cmake --build build --target compile_test_classes`
- 检查 `build/test_classes` 目录是否存在

### 断点不生效
- 确保使用 Debug 配置构建：`cmake --build build --config Debug`
- 检查可执行文件是否是最新的
- 尝试清理并重新构建：`cmake --build build --target clean && cmake --build build`

### 调试器无法启动
- 确保已安装 Xcode Command Line Tools
- 检查 `build/bin/test_*` 文件是否存在且有执行权限

## 示例：调试 KlassTest.FindMethod

1. 打开 `tests/modules/runtime/klass_test.cpp`
2. 在 `TEST_F(KlassTest, FindMethod)` 的第一行设置断点
3. 选择 "Debug Single Test (Runtime)" 配置
4. 输入过滤器：`KlassTest.FindMethod`
5. 按 `F5` 开始调试
6. 程序会在断点处停止，可以查看变量和执行流程

## 环境变量

测试使用的 `TEST_CLASS_PATH` 在编译时通过 CMake 定义，指向 `build/test_classes`。调试配置中不需要额外设置环境变量。
