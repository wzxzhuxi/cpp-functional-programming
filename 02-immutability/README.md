# 02 - 不可变性与const

> 可变状态是bug的温床。

## 本章目标

学完本章，你将能够：
- 理解为什么不可变性对代码质量如此重要
- 正确使用 `const` 的各种形式
- 设计不可变类
- 理解持久化数据结构的基本原理

## 为什么需要不可变性

先看一个问题：

```cpp
int balance = 1000;
withdraw(200);
deposit(100);
transfer_to(other, 300);
withdraw(50);
// 现在 balance 是多少？
```

要知道 `balance` 的值，你必须追踪每一步操作，理解每个函数做了什么。这就是**可变状态**的问题——状态随时可能被任何代码修改。

用不可变的方式：

```cpp
const auto b1 = Account{1000};
const auto b2 = b1.withdraw(200);   // b1 不变
const auto b3 = b2.deposit(100);    // b2 不变
const auto b4 = b3.transfer(300);   // b3 不变
const auto b5 = b4.withdraw(50);    // b4 不变
// b5 = 550，每个中间状态都保留
```

**不可变性的好处：**

| 好处 | 说明 |
|------|------|
| 无副作用 | 函数不会意外修改你的数据 |
| 线程安全 | 不可变数据天然线程安全，无需加锁 |
| 易于调试 | 可以检查任意时刻的状态 |
| 易于测试 | 相同输入永远得到相同输出 |
| 可共享 | 多处代码可以安全共享同一数据 |

## const 基础

`const` 是 C++ 中表达不可变性的核心工具。

### 变量的 const

```cpp
// 常量变量
const int x = 42;
// x = 43;  // 编译错误！

// const 引用：不拷贝，不能通过引用修改
void print(const std::string& s) {
    std::cout << s;
    // s += "!";  // 编译错误！
}
```

### 指针的 const

指针的 const 有两种含义，记住口诀：**const 在 * 左边修饰数据，在 * 右边修饰指针**。

```cpp
int value = 10;

const int* p1 = &value;    // 指向 const int（不能通过 p1 修改 value）
int* const p2 = &value;    // const 指针（p2 不能指向别处）
const int* const p3 = &value;  // 都不能改
```

### 成员函数的 const

```cpp
class Point {
    int x_, y_;
public:
    Point(int x, int y) : x_(x), y_(y) {}

    // const 成员函数：承诺不修改对象状态
    int x() const { return x_; }
    int y() const { return y_; }

    double distance() const {
        return std::sqrt(x_ * x_ + y_ * y_);
    }

    // 非 const 函数：可以修改状态
    void set_x(int x) { x_ = x; }
};

const Point p{3, 4};
p.x();        // OK
p.distance(); // OK
// p.set_x(5);   // 编译错误：const 对象不能调用非 const 成员函数
```

**原则：不修改状态的成员函数都应该标记 `const`。**

## constexpr：编译期常量

`constexpr` 比 `const` 更强——值在编译期就确定了。

```cpp
// constexpr 变量
constexpr int max_size = 100;  // 编译期常量

// constexpr 函数
constexpr int square(int x) {
    return x * x;
}

constexpr int s = square(5);  // 编译期计算：s = 25
static_assert(s == 25);       // 编译期断言

// 运行时也能调用
int n;
std::cin >> n;
int result = square(n);  // 运行时计算
```

### C++20 constexpr 增强

```cpp
// C++20: constexpr std::vector
constexpr auto make_squares() {
    std::vector<int> v;
    for (int i = 1; i <= 5; ++i) {
        v.push_back(i * i);
    }
    return v;
}

// C++20: consteval 强制编译期
consteval int compile_time_only(int x) {
    return x * x;
}

constexpr int a = compile_time_only(5);  // OK
// int b = compile_time_only(n);  // 错误：必须编译期可求值
```

## 设计不可变类

不可变类的核心原则：**所有"修改"操作都返回新对象**。

### 简单不可变类

```cpp
class ImmutablePoint {
    const int x_;
    const int y_;

public:
    constexpr ImmutablePoint(int x, int y) : x_(x), y_(y) {}

    [[nodiscard]] constexpr int x() const { return x_; }
    [[nodiscard]] constexpr int y() const { return y_; }

    // "修改"操作返回新对象
    [[nodiscard]] constexpr ImmutablePoint move(int dx, int dy) const {
        return ImmutablePoint{x_ + dx, y_ + dy};
    }

    [[nodiscard]] constexpr ImmutablePoint scale(int factor) const {
        return ImmutablePoint{x_ * factor, y_ * factor};
    }

    // C++20: 默认比较
    auto operator<=>(const ImmutablePoint&) const = default;
};

// 使用
constexpr auto p1 = ImmutablePoint{3, 4};
constexpr auto p2 = p1.move(1, 1);   // p1 不变，p2 = (4, 5)
constexpr auto p3 = p2.scale(2);     // p2 不变，p3 = (8, 10)

static_assert(p1.x() == 3);  // p1 确实没变
static_assert(p3.x() == 8);
```

### 不可变容器

```cpp
template<typename T>
class ImmutableVector {
    std::shared_ptr<const std::vector<T>> data_;

public:
    ImmutableVector()
        : data_(std::make_shared<const std::vector<T>>()) {}

    explicit ImmutableVector(std::vector<T> vec)
        : data_(std::make_shared<const std::vector<T>>(std::move(vec))) {}

    // 只读访问
    const T& operator[](size_t i) const { return (*data_)[i]; }
    size_t size() const { return data_->size(); }
    bool empty() const { return data_->empty(); }

    // "修改"操作返回新容器
    [[nodiscard]] ImmutableVector push_back(T value) const {
        auto new_data = *data_;  // 复制
        new_data.push_back(std::move(value));
        return ImmutableVector{std::move(new_data)};
    }

    [[nodiscard]] ImmutableVector set(size_t i, T value) const {
        auto new_data = *data_;
        new_data[i] = std::move(value);
        return ImmutableVector{std::move(new_data)};
    }

    // 函数式操作
    template<typename F>
    [[nodiscard]] auto map(F f) const {
        using R = std::invoke_result_t<F, const T&>;
        std::vector<R> result;
        result.reserve(size());
        for (const auto& item : *data_) {
            result.push_back(f(item));
        }
        return ImmutableVector<R>{std::move(result)};
    }

    template<typename Pred>
    [[nodiscard]] ImmutableVector filter(Pred pred) const {
        std::vector<T> result;
        std::copy_if(data_->begin(), data_->end(),
                     std::back_inserter(result), pred);
        return ImmutableVector{std::move(result)};
    }
};
```

## 可变状态的危害

### 危害1：多线程竞态条件

```cpp
// 危险：多线程修改同一变量
class BadCounter {
    int count_ = 0;
public:
    void increment() { ++count_; }  // 多线程调用 = 数据竞争！
    int get() const { return count_; }
};

// 安全：不可变版本
class SafeCounter {
    const int count_;
public:
    explicit SafeCounter(int count = 0) : count_(count) {}
    [[nodiscard]] SafeCounter increment() const {
        return SafeCounter{count_ + 1};
    }
    int get() const { return count_; }
};
// 每个线程得到自己的新 Counter，无竞争
```

### 危害2：意外修改

```cpp
// 危险：暴露可变引用
class BadContainer {
    std::vector<int> data_{1, 2, 3};
public:
    std::vector<int>& data() { return data_; }
};

BadContainer c;
c.data().clear();  // 外部代码清空了内部数据！

// 安全：只暴露 const 引用
class SafeContainer {
    std::vector<int> data_{1, 2, 3};
public:
    const std::vector<int>& data() const { return data_; }
};
```

### 危害3：难以追踪的副作用

```cpp
// 危险：修改输入参数
void bad_sort(std::vector<int>& vec) {
    std::sort(vec.begin(), vec.end());  // 调用者的数据被改了
}

// 安全：返回新值
[[nodiscard]] std::vector<int> good_sort(std::vector<int> vec) {
    std::sort(vec.begin(), vec.end());
    return vec;  // 返回排序后的副本
}
```

## 持久化数据结构

持久化数据结构是不可变的，但通过共享内存来避免完整复制。

### 持久化链表

```cpp
template<typename T>
class PersistentList {
    struct Node {
        T value;
        std::shared_ptr<const Node> next;
    };

    std::shared_ptr<const Node> head_;

    explicit PersistentList(std::shared_ptr<const Node> head)
        : head_(std::move(head)) {}

public:
    PersistentList() = default;

    // O(1) 在头部添加
    [[nodiscard]] PersistentList cons(T value) const {
        return PersistentList{
            std::make_shared<const Node>(Node{std::move(value), head_})
        };
    }

    [[nodiscard]] const T& head() const { return head_->value; }

    [[nodiscard]] PersistentList tail() const {
        return PersistentList{head_->next};
    }

    [[nodiscard]] bool empty() const { return head_ == nullptr; }
};

// 使用：多版本共存
const auto list1 = PersistentList<int>{}.cons(1).cons(2).cons(3);  // [3,2,1]
const auto list2 = list1.cons(4);   // [4,3,2,1]
const auto list3 = list1.tail();    // [2,1]
// list1、list2、list3 共享大部分内存，互不影响
```

**内存共享示意：**

```
list1: 3 -> 2 -> 1 -> null
            ↑
list2: 4 ---┘

list3:      2 -> 1 -> null
```

## 实践原则

1. **默认用 const** —— 变量、参数、成员函数都默认 const
2. **返回新值** —— "修改"操作返回新对象
3. **不暴露可变引用** —— getter 返回 const 引用或值
4. **用 [[nodiscard]]** —— 提醒调用者使用返回值
5. **考虑共享** —— 大对象用 shared_ptr 避免拷贝

## 代码示例

完整示例：`examples/01_immutability.cpp`

## 练习

1. **ImmutableRectangle**：实现不可变矩形类，包含 `resize()` 和 `move()` 方法
2. **ImmutableVector::reduce**：为 ImmutableVector 添加 reduce 方法
3. **性能对比**：比较可变和不可变实现的性能差异
4. **ImmutableMap**：实现简单的不可变 Map

练习文件：`exercises/exercises.cpp`
参考答案：`exercises/solutions.cpp`

## 下一章

[03 - 纯函数](../03-pure-functions/README.md)
