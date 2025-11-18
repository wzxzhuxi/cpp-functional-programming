# 01 - 函数式编程基础

## 什么是函数式编程

函数式编程（FP）核心思想：**把计算看作函数求值，避免状态变化和可变数据**。

### 核心特征

1. **函数是一等公民** - 函数可以作为参数、返回值、赋值给变量
2. **纯函数** - 相同输入永远产生相同输出，无副作用
3. **不可变数据** - 数据一旦创建就不再改变
4. **声明式** - 描述"做什么"而非"怎么做"
5. **组合性** - 小函数组合成复杂功能

### 命令式 vs 函数式

```cpp
// 命令式 - 告诉计算机怎么做
int sum = 0;
for (int i = 0; i < numbers.size(); i++) {
    if (numbers[i] % 2 == 0) {
        sum += numbers[i];
    }
}

// 函数式 - 告诉计算机做什么
auto sum = std::accumulate(
    numbers.begin(), numbers.end(), 0,
    [](int acc, int x) { return x % 2 == 0 ? acc + x : acc; }
);
```

第二种更清晰：求和、过滤偶数，一眼看穿。

## C++ 中的函数式特性

C++ 不是纯函数式语言，但从 C++11 开始支持够用的特性：

| 特性 | 版本 | 说明 |
|------|------|------|
| Lambda 表达式 | C++11 | 匿名函数 |
| `std::function` | C++11 | 通用函数包装器 |
| `constexpr` | C++11/14/17 | 编译期计算 |
| `std::optional` | C++17 | Maybe monad |
| `std::variant` | C++17 | Sum types |
| Ranges | C++20 | 惰性求值的函数式风格 |
| Concepts | C++20 | 类型约束 |

## Lambda 表达式

Lambda 是匿名函数，语法：

```cpp
[capture](parameters) -> return_type { body }
```

### 基本用法

```cpp
// 最简单的 lambda
auto add = [](int a, int b) { return a + b; };
std::cout << add(2, 3); // 5

// 自动推导返回类型
auto multiply = [](int a, int b) { return a * b; };

// 显式返回类型
auto divide = [](int a, int b) -> double {
    return static_cast<double>(a) / b;
};
```

### 捕获外部变量

```cpp
int x = 10;
int y = 20;

// 值捕获
auto f1 = [x]() { return x * 2; };

// 引用捕获
auto f2 = [&x]() { x += 5; };

// 捕获所有（值）
auto f3 = [=]() { return x + y; };

// 捕获所有（引用）
auto f4 = [&]() { x += y; };

// 混合捕获
auto f5 = [x, &y]() { return x + y; };
```

**警告**：引用捕获容易导致悬空引用，值捕获更安全。

### 实际应用

```cpp
#include <algorithm>
#include <vector>
#include <iostream>

int main() {
    std::vector<int> nums = {1, 2, 3, 4, 5, 6};

    // 过滤偶数
    std::vector<int> evens;
    std::copy_if(nums.begin(), nums.end(),
                 std::back_inserter(evens),
                 [](int x) { return x % 2 == 0; });

    // 转换：每个元素平方
    std::vector<int> squares;
    std::transform(nums.begin(), nums.end(),
                   std::back_inserter(squares),
                   [](int x) { return x * x; });

    // 排序：降序
    std::sort(nums.begin(), nums.end(),
              [](int a, int b) { return a > b; });

    return 0;
}
```

## 闭包

Lambda 捕获外部变量后形成**闭包** - 函数 + 环境。

```cpp
auto make_adder(int n) {
    // 返回的 lambda 捕获了 n
    return [n](int x) { return x + n; };
}

auto add5 = make_adder(5);
auto add10 = make_adder(10);

std::cout << add5(3);   // 8
std::cout << add10(3);  // 13
```

闭包记住了创建时的环境（`n` 的值）。

### 陷阱：引用捕获的生命周期

```cpp
auto make_bad_closure() {
    int x = 42;
    return [&x]() { return x; };  // 危险！x 已销毁
}

auto f = make_bad_closure();
f();  // 未定义行为 - x 已经不存在了
```

**规则**：返回 lambda 时，用值捕获，不用引用。

## std::function

`std::function` 是类型擦除的函数包装器，可以存储任何可调用对象：

```cpp
#include <functional>

std::function<int(int, int)> op;

// 存储 lambda
op = [](int a, int b) { return a + b; };
op(2, 3);  // 5

// 存储函数指针
int multiply(int a, int b) { return a * b; }
op = multiply;
op(2, 3);  // 6

// 存储函数对象
struct Divide {
    int operator()(int a, int b) const { return a / b; }
};
op = Divide{};
op(6, 3);  // 2
```

有性能开销（虚函数调用），但灵活。

## 实践建议

1. **默认用 auto lambda** - 除非需要存储或类型擦除
2. **优先值捕获** - 避免生命周期问题
3. **保持 lambda 简短** - 超过 3 行就抽成命名函数
4. **用 const** - 不改变捕获的值就加 `mutable`

## 代码示例

完整示例见：`examples/basic/01_lambdas.cpp`

## 练习

1. 写一个 lambda，接收字符串数组，返回长度大于 5 的字符串
2. 实现 `make_multiplier(n)` 返回一个"乘以 n"的函数
3. 用 lambda 和 `std::sort` 对结构体数组按某字段排序

练习题在：`exercises/01_basics_exercises.cpp`
