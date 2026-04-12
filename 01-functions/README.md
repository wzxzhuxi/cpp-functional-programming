# 01 - C++中的函数

> 函数是一等公民。

## 本章目标

学完本章，你将能够：
- 理解什么是"函数作为一等公民"
- 熟练使用 lambda 表达式
- 正确处理闭包和变量捕获
- 知道 `auto` lambda 和 `std::function` 的区别

## 什么是函数式编程

先看两段代码，做同样的事情——求偶数之和：

```cpp
// 方式A：命令式
int sum = 0;
for (int i = 0; i < numbers.size(); i++) {
    if (numbers[i] % 2 == 0) {
        sum += numbers[i];
    }
}

// 方式B：函数式
auto sum = std::accumulate(numbers.begin(), numbers.end(), 0,
    [](int acc, int x) { return x % 2 == 0 ? acc + x : acc; });
```

方式A告诉计算机**怎么做**（循环、判断、累加）。
方式B告诉计算机**做什么**（累加满足条件的元素）。

函数式风格：
- 更简洁
- 更不容易出bug（没有可变的 `sum` 和 `i`）
- 更容易并行化

## Lambda 表达式

Lambda 是匿名函数，C++11 引入，C++14/17/20 持续增强。

### 基本语法

```cpp
[捕获列表](参数列表) -> 返回类型 { 函数体 }
```

大多数情况返回类型可以省略：

```cpp
// 最简单的 lambda
auto add = [](int a, int b) { return a + b; };

std::cout << add(2, 3);  // 输出 5
```

### 捕获外部变量

Lambda 可以"捕获"外部作用域的变量：

```cpp
int factor = 10;

// 值捕获：复制一份 factor
auto f1 = [factor](int x) { return x * factor; };

// 引用捕获：使用 factor 的引用
auto f2 = [&factor](int x) { return x * factor; };

// 捕获所有（值）
auto f3 = [=](int x) { return x * factor; };

// 捕获所有（引用）- 危险，尽量避免
auto f4 = [&](int x) { return x * factor; };

// 捕获所有（值），但值捕获 factor
auto f5 = [=, &factor](int x) { return x * factor; };

// 捕获所有（引用）, 但引用捕获 factor
auto f6 = [&, factor](int x) { return x * factor; };
```

**黄金法则**：优先使用**值捕获**，除非你确定引用的生命周期。

### 捕获陷阱

```cpp
auto make_bad_closure() {
    int x = 42;
    return [&x]() { return x; };  // 危险！
}

auto f = make_bad_closure();
f();  // 未定义行为 - x 已被销毁
```

`x` 是局部变量，函数返回后就销毁了。引用捕获的 `&x` 变成悬空引用。

**正确做法**：

```cpp
auto make_good_closure() {
    int x = 42;
    return [x]() { return x; };  // 值捕获，安全
}
```

## 闭包

Lambda 捕获外部变量后，形成**闭包**（closure）= 函数 + 环境。

```cpp
auto make_adder(int n) {
    return [n](int x) { return x + n; };
}

auto add5 = make_adder(5);
auto add10 = make_adder(10);

std::cout << add5(3);   // 8
std::cout << add10(3);  // 13
```

`add5` 和 `add10` 各自"记住"了创建时 `n` 的值。这就是闭包的威力。

## std::function

`std::function` 是类型擦除的函数包装器：

```cpp
#include <functional>

// 声明一个接受两个 int、返回 int 的函数类型
std::function<int(int, int)> op;

// 可以存储 lambda
op = [](int a, int b) { return a + b; };

// 可以存储普通函数
int multiply(int a, int b) { return a * b; }
op = multiply;

// 可以存储函数对象
struct Divide {
    int operator()(int a, int b) const { return a / b; }
};
op = Divide{};
```

### auto vs std::function

| 特性 | `auto` lambda | `std::function` |
|------|---------------|-----------------|
| 类型 | 唯一的匿名类型 | 统一的包装类型 |
| 性能 | 零开销 | 有虚函数调用开销 |
| 存储 | 不能放容器 | 可以放容器 |
| 递归 | 不能直接递归 | 可以递归 |

**原则**：
- 局部使用：用 `auto`
- 需要存储或传递多态函数：用 `std::function`

## C++14/17/20 新特性

### 泛型 Lambda (C++14)

```cpp
// C++11：必须指定类型
auto add11 = [](int a, int b) { return a + b; };

// C++14：auto 参数
auto add14 = [](auto a, auto b) { return a + b; };

add14(1, 2);      // int
add14(1.5, 2.5);  // double
add14("a"s, "b"s);  // string
```

### 初始化捕获 (C++14)

```cpp
auto ptr = std::make_unique<int>(42);

// 移动捕获
auto f = [p = std::move(ptr)]() { return *p; };
```

### constexpr Lambda (C++17)

```cpp
constexpr auto square = [](int x) { return x * x; };

static_assert(square(5) == 25);  // 编译期计算
```

### 模板 Lambda (C++20)

```cpp
// C++20：显式模板参数
auto print = []<typename T>(const std::vector<T>& vec) {
    for (const auto& item : vec) {
        std::cout << item << " ";
    }
};

print(std::vector{1, 2, 3});
print(std::vector{"a"s, "b"s, "c"s});
```

## 与 STL 算法配合

Lambda 最常见的用途是配合 STL 算法：

```cpp
#include <algorithm>
#include <vector>

std::vector<int> nums = {5, 2, 8, 1, 9, 3};

// 过滤：保留偶数
std::vector<int> evens;
std::copy_if(nums.begin(), nums.end(), std::back_inserter(evens),
             [](int x) { return x % 2 == 0; });

// 变换：平方
std::vector<int> squares;
std::transform(nums.begin(), nums.end(), std::back_inserter(squares),
               [](int x) { return x * x; });

// 排序：降序
std::sort(nums.begin(), nums.end(),
          [](int a, int b) { return a > b; });

// 查找：第一个大于 5 的
auto it = std::find_if(nums.begin(), nums.end(),
                       [](int x) { return x > 5; });

// 累加
auto sum = std::accumulate(nums.begin(), nums.end(), 0,
                           [](int acc, int x) { return acc + x; });
```

## 实践建议

1. **用 `auto` 声明 lambda**，除非需要类型擦除
2. **优先值捕获**，避免生命周期问题
3. **lambda 保持简短**，超过 5 行就抽成命名函数
4. **显式捕获**，`[=]` 和 `[&]` 隐藏了捕获的内容
5. **C++14+ 用泛型 lambda**，更灵活

## 代码示例

完整示例：`examples/01_lambdas.cpp`

## 练习

1. **filter_long_strings**：接收字符串数组，返回长度大于指定值的字符串
2. **make_multiplier**：返回一个"乘以 n"的闭包
3. **sort_by_field**：用 lambda 对结构体数组按指定字段排序
4. **chain_operations**：组合 filter → transform → reduce 操作
5. **counter_factory**：返回一个每次调用自增的计数器闭包

练习文件：`exercises/exercises.cpp`
参考答案：`exercises/solutions.cpp`

## 下一章

[02 - 不可变性与const](../02-immutability/README.md)
