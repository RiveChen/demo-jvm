# C++ Constructor

## rule of five

**C++ Rule of Five（五法则）** 是 C++11 引入的一项现代 C++ 编程原则，它是对经典 C++98 "Rule of Three"（三法则）的扩展。

简单来说，**如果你需要显式定义以下五个特殊成员函数中的任何一个，那么你通常也需要显式定义（或显式删除/默认）其他四个。**

这通常发生在你的类**手动管理资源**（如原始指针、文件句柄、网络连接等）的时候。

-----

### 五个特殊成员函数是什么？

这五个函数负责对象的生命周期管理：

1.  **析构函数 (Destructor):** `~MyClass()`
2.  **拷贝构造函数 (Copy Constructor):** `MyClass(const MyClass&)`
3.  **拷贝赋值运算符 (Copy Assignment Operator):** `MyClass& operator=(const MyClass&)`
4.  **移动构造函数 (Move Constructor):** `MyClass(MyClass&&)` —— *C++11 新增*
5.  **移动赋值运算符 (Move Assignment Operator):** `MyClass& operator=(MyClass&&)` —— *C++11 新增*

-----

### 为什么会有这个规则？

#### 1\. 从“三法则”到“五法则”

在 C++98 中，只有前三个（析构、拷贝构造、拷贝赋值）。如果你管理一个原始指针（比如 `new char[]`），你需要写析构函数来 `delete` 它。如果你写了析构函数，默认的拷贝构造函数只会进行“浅拷贝”（只复制指针地址），这会导致两个对象指向同一块内存，析构时发生 **Double Free**（重复释放）错误。因此，你需要自定义深拷贝。

#### 2\. C++11 引入的移动语义 (Move Semantics)

C++11 引入了**移动语义**以提高性能。对于临时对象或即将销毁的对象，我们不应该“复制”资源（深拷贝，慢），而应该“窃取”资源（移动指针的所有权，快）。

**规则的核心逻辑是：**

> 如果你的类拥有一个资源（非 RAII 包装），你需要析构函数来释放它，这意味你需要拷贝函数来复制它。既然你需要自定义拷贝逻辑，那么为了性能，编译器**不会**自动为你生成默认的移动操作。如果你想支持高效的移动（例如放入 `std::vector` 并重新分配大小时），你必须手动实现移动操作。

-----

### 代码示例：手动管理内存的类

这是一个典型的遵循 Rule of Five 的类，它管理一个 `int` 数组：

```cpp
#include <utility> // for std::move
#include <algorithm> // for std::copy

class Buffer {
private:
    int* data;
    size_t size;

public:
    // 构造函数
    Buffer(size_t s) : size(s), data(new int[s]) {}

    // ==========================================
    // 1. 析构函数 (Destructor)
    // 负责释放资源
    // ==========================================
    ~Buffer() {
        delete[] data;
    }

    // ==========================================
    // 2. 拷贝构造函数 (Copy Constructor)
    // 负责深拷贝 (Deep Copy)
    // ==========================================
    Buffer(const Buffer& other) : size(other.size), data(new int[other.size]) {
        std::copy(other.data, other.data + other.size, data);
    }

    // ==========================================
    // 3. 拷贝赋值运算符 (Copy Assignment Operator)
    // 也就是 copy-and-swap 惯用法的变体
    // ==========================================
    Buffer& operator=(const Buffer& other) {
        if (this != &other) {
            int* newData = new int[other.size];
            std::copy(other.data, other.data + other.size, newData);
            delete[] data; // 释放旧资源
            data = newData;
            size = other.size;
        }
        return *this;
    }

    // ==========================================
    // 4. 移动构造函数 (Move Constructor)
    // 负责窃取资源 (Steal resource)，非常快
    // ==========================================
    Buffer(Buffer&& other) noexcept : data(other.data), size(other.size) {
        // 将源对象置于“有效但未定义”的状态 (通常是空)
        other.data = nullptr; 
        other.size = 0;
    }

    // ==========================================
    // 5. 移动赋值运算符 (Move Assignment Operator)
    // ==========================================
    Buffer& operator=(Buffer&& other) noexcept {
        if (this != &other) {
            delete[] data;       // 释放自己的旧资源
            data = other.data;   // 窃取对方的资源
            size = other.size;
            
            // 重置源对象
            other.data = nullptr;
            other.size = 0;
        }
        return *this;
    }
};
```

-----

### 进阶：Rule of Zero (零法则)

在现代 C++ 中，最推荐的做法其实是 **"Rule of Zero"**。

如果不直接手动管理资源（不使用 `new`/`delete`），而是使用现成的 RAII 包装器（如 `std::unique_ptr`、`std::shared_ptr`、`std::vector`、`std::string`），那么你**不需要**写这五个函数中的任何一个。编译器生成的默认版本会自动正确处理成员变量的生命周期。

**Rule of Zero 示例：**

```cpp
#include <vector>

class ModernBuffer {
    // std::vector 已经实现了 Rule of Five，它会自动管理内存
    std::vector<int> data; 

public:
    // 不需要写析构、拷贝或移动，编译器生成的默认版本完美工作
    ModernBuffer(size_t s) : data(s) {} 
};
```

### 总结

  * **Rule of Three (C++98):** 如果你需要析构函数，你也需要拷贝构造和拷贝赋值。
  * **Rule of Five (C++11):** 为了性能，如果你实现了前三个，你最好也实现移动构造和移动赋值。
  * **Rule of Zero (现代推荐):** 尽量不要手动管理资源，使用标准库容器和智能指针，这样你就一个都不用写。
