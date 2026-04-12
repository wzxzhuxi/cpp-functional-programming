# 07 - C++20 Ranges

> 现代 C++ 的函数式革命。

## 本章目标

学完本章，你将能够：
- 理解 Ranges 库的核心概念
- 使用管道语法组合操作
- 掌握常用视图适配器
- 利用惰性求值优化性能
- 创建自定义视图

## 什么是 Ranges

**Ranges** 是 C++20 引入的革命性库，将函数式编程风格带入 STL。

核心优势：
1. **管道语法**：`data | filter(...) | transform(...)`
2. **惰性求值**：只在需要时计算
3. **可组合**：视图可以任意组合
4. **安全**：减少手动迭代器操作的错误

```cpp
#include <ranges>
#include <vector>

std::vector<int> nums = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

// 传统方式
std::vector<int> result;
for (int n : nums) {
    if (n % 2 == 0) {
        result.push_back(n * n);
    }
}

// Ranges 方式
auto result = nums
    | std::views::filter([](int n) { return n % 2 == 0; })
    | std::views::transform([](int n) { return n * n; });
```

## Views（视图）

视图是 Ranges 的核心，它们是**惰性**的范围适配器。

### 基本视图

```cpp
#include <ranges>
namespace views = std::views;

std::vector<int> nums = {1, 2, 3, 4, 5};

// filter：过滤
auto evens = nums | views::filter([](int n) { return n % 2 == 0; });
// 惰性！只在迭代时计算

// transform：转换
auto squares = nums | views::transform([](int n) { return n * n; });

// take：取前 N 个
auto first3 = nums | views::take(3);

// drop：跳过前 N 个
auto after2 = nums | views::drop(2);

// reverse：反转
auto reversed = nums | views::reverse;
```

### 管道组合

```cpp
// 组合多个视图
auto result = nums
    | views::filter([](int n) { return n > 2; })
    | views::transform([](int n) { return n * 2; })
    | views::take(3);

// 等价于：取 n>2 的元素，翻倍，取前3个
// {3,4,5} -> {6,8,10} -> {6,8,10}
```

### 惰性求值

视图不会立即计算，只有在遍历时才执行：

```cpp
auto expensive_view = nums
    | views::transform([](int n) {
        std::cout << "Computing: " << n << "\n";
        return n * n;
    })
    | views::take(2);

// 此时没有任何输出！

for (int x : expensive_view) {
    std::cout << "Result: " << x << "\n";
}
// 输出：
// Computing: 1
// Result: 1
// Computing: 2
// Result: 4
// （只计算了2个元素）
```

## 常用视图

### views::iota

生成序列：

```cpp
// 0, 1, 2, 3, 4
auto zero_to_four = views::iota(0, 5);

// 无限序列：0, 1, 2, ...
auto naturals = views::iota(0);

// 配合 take 使用
auto first10 = views::iota(0) | views::take(10);
```

### views::enumerate (C++23)

带索引迭代：

```cpp
std::vector<std::string> names = {"Alice", "Bob", "Charlie"};

for (auto [index, name] : names | views::enumerate) {
    std::cout << index << ": " << name << "\n";
}
```

### views::zip (C++23)

合并多个范围：

```cpp
std::vector<int> ids = {1, 2, 3};
std::vector<std::string> names = {"A", "B", "C"};

for (auto [id, name] : views::zip(ids, names)) {
    std::cout << id << " -> " << name << "\n";
}
```

### views::chunk (C++23)

分块：

```cpp
std::vector<int> nums = {1, 2, 3, 4, 5, 6, 7};

for (auto chunk : nums | views::chunk(3)) {
    // chunk: {1,2,3}, {4,5,6}, {7}
}
```

### views::split

分割：

```cpp
std::string csv = "a,b,c,d";
for (auto part : csv | views::split(',')) {
    // part: "a", "b", "c", "d"
}
```

### views::join

展平嵌套：

```cpp
std::vector<std::vector<int>> nested = {{1, 2}, {3, 4}, {5}};
auto flat = nested | views::join;
// flat: 1, 2, 3, 4, 5
```

### views::keys / views::values

处理键值对：

```cpp
std::map<int, std::string> m = {{1, "a"}, {2, "b"}};

auto keys = m | views::keys;     // 1, 2
auto values = m | views::values; // "a", "b"
```

## 范围算法

C++20 也更新了算法，支持直接使用范围：

```cpp
std::vector<int> nums = {3, 1, 4, 1, 5, 9, 2, 6};

// 传统
std::sort(nums.begin(), nums.end());

// Ranges
std::ranges::sort(nums);

// 带投影
struct Person { std::string name; int age; };
std::vector<Person> people = {...};

std::ranges::sort(people, {}, &Person::age);  // 按年龄排序
```

### 常用范围算法

```cpp
#include <algorithm>

std::vector<int> nums = {1, 2, 3, 4, 5};

// find
auto it = std::ranges::find(nums, 3);

// any_of / all_of / none_of
bool has_even = std::ranges::any_of(nums, [](int n) { return n % 2 == 0; });

// count_if
auto even_count = std::ranges::count_if(nums, [](int n) { return n % 2 == 0; });

// for_each
std::ranges::for_each(nums, [](int n) { std::cout << n << " "; });

// min / max
auto [min, max] = std::ranges::minmax(nums);
```

## 收集结果

视图是惰性的，要收集结果需要转换：

```cpp
// C++20：手动收集
auto view = nums | views::filter(...) | views::transform(...);
std::vector<int> result(view.begin(), view.end());

// 或使用范围构造
std::vector<int> result;
std::ranges::copy(view, std::back_inserter(result));

// C++23：to<Container>()
auto result = nums
    | views::filter(...)
    | views::transform(...)
    | std::ranges::to<std::vector>();
```

## 实际应用

### 数据处理管道

```cpp
struct Employee {
    std::string name;
    std::string department;
    double salary;
};

std::vector<Employee> employees = {...};

// 计算 IT 部门平均薪资
auto it_salaries = employees
    | views::filter([](const Employee& e) {
        return e.department == "IT";
    })
    | views::transform(&Employee::salary);

double total = 0;
int count = 0;
for (double s : it_salaries) {
    total += s;
    count++;
}
double avg = count > 0 ? total / count : 0;
```

### 文本处理

```cpp
std::string text = "hello world this is a test";

// 获取长度大于3的单词
auto long_words = text
    | views::split(' ')
    | views::filter([](auto word) {
        return std::ranges::distance(word) > 3;
    });
```

### 无限序列

```cpp
// 斐波那契序列（使用生成器模式）
auto fib = views::iota(0)
    | views::transform([cache = std::vector<long long>{0, 1}](int n) mutable {
        if (n < 2) return cache[n];
        while (cache.size() <= static_cast<size_t>(n)) {
            cache.push_back(cache.back() + cache[cache.size() - 2]);
        }
        return cache[n];
    });

// 取前10个斐波那契数
for (auto n : fib | views::take(10)) {
    std::cout << n << " ";
}
```

## 自定义视图适配器

创建可复用的视图适配器：

```cpp
// 简单的自定义适配器
inline constexpr auto square = views::transform([](auto x) { return x * x; });
inline constexpr auto double_it = views::transform([](auto x) { return x * 2; });

auto result = nums | square | double_it;
```

## 性能考虑

### 惰性求值的优势

```cpp
// 传统方式：创建多个临时容器
std::vector<int> step1;
std::copy_if(nums.begin(), nums.end(), std::back_inserter(step1),
             [](int n) { return n > 5; });

std::vector<int> step2;
std::transform(step1.begin(), step1.end(), std::back_inserter(step2),
               [](int n) { return n * 2; });

// Ranges：零中间容器
auto result = nums
    | views::filter([](int n) { return n > 5; })
    | views::transform([](int n) { return n * 2; });
```

### 注意事项

1. **视图不拥有数据**：确保底层范围有效
2. **多次迭代可能重复计算**：需要时先收集到容器
3. **某些操作是 O(n)**：如 `views::reverse` 需要知道终点

## 代码示例

完整示例：`examples/01_ranges.cpp`

## 练习

1. **单词统计**：用 Ranges 统计文本中单词频率
2. **数据过滤管道**：处理结构化数据
3. **自定义视图**：实现 `sliding_window` 视图
4. **性能对比**：比较 Ranges vs 传统循环

练习文件：`exercises/exercises.cpp`
参考答案：`exercises/solutions.cpp`

## 下一章

[08 - 代数数据类型](../08-algebraic-types/README.md)
