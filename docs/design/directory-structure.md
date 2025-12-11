# Directory Structure

本项目结构如下:

## 根目录

- **配置文件**:
  - `CMakeLists.txt`: 根 CMake 配置文件，定义项目设置、编译选项和依赖管理
  - `.clang-format`: 代码格式化配置（基于 Google 风格）
  - `.clang-tidy`: 静态代码分析配置
  - `.editorconfig`: 编辑器配置，统一代码风格
  - `.gitignore`: Git 忽略文件配置
  - `LICENSE`: MIT 许可证
  - `README.md`: 项目说明文档

## 主要目录

- `docs/`: 存放开发日志、学习笔记和项目设计文档
  - `design/`: 设计文档
    - `directory-structure.md`: 本文件，描述项目目录结构
  - `dev_logs/`: 开发日志（计划中）
  - `learning/`: 学习笔记（计划中）

- `src/`: 存放源代码
  - `CMakeLists.txt`: 源代码的 CMake 配置
  - `main.cpp`: 主程序入口
  - `common/`: 公共工具和基础类（计划中）
  - 其他模块目录（待添加）

- `tests/`: 存放测试代码
  - `CMakeLists.txt`: 测试的 CMake 配置
  - `sanity/`: 环境检查测试，验证编译环境和工具链
    - `CMakeLists.txt`: 测试子目录配置
    - `env_test.cpp`: 环境检查测试用例
  - `data/`: 测试数据文件（计划中）
  - `modules/`: 各模块的单元测试（计划中）

- `include/`: 存放 JNI 头文件（计划中，仅用于 JNI）

- `build/`: 构建产物目录（由 CMake 生成，已加入 .gitignore）
  - `bin/`: 可执行文件
    - `demo-jvm`: 主程序
    - `test_sanity`: 环境检查测试
  - `lib/`: 库文件
  - `_deps/`: 外部依赖（如 GoogleTest）
