# 04 - 高阶函数

> 函数作为一等公民，是函数式编程的核心。

## 本章目标

学完本章，你将能够：
- 理解高阶函数的概念
- 熟练使用 map、filter、reduce
- 了解柯里化、偏应用和函数组合的概念（详见 Ch05/Ch06）

## 什么是高阶函数

**高阶函数（Higher-Order Function）** 满足以下至少一个条件：

1. **接受函数作为参数**
2. **返回函数作为结果**

```cpp
// 接受函数作为参数
template<typename F>
int apply(int x, F f) {
    return f(x);
}

// 返回函数作为结果
auto make_adder(int n) {
    return [n](int x) { return x + n; };
}

// 既接受又返回函数
template<typename F, typename G>
auto compose(F f, G g) {
    return [f, g](auto x) { return f(g(x)); };
}
```

## 函数作为一等公民

C++ 中函数是一等公民：可以像普通值一样传递、存储、返回。

### 函数作为参数

```cpp
// 传递 lambda
auto result = apply(5, [](int x) { return x * 2; });  // 10

// 传递函数对象
struct Square {
    int operator()(int x) const { return x * x; }
};
auto squared = apply(5, Square{});  // 25

// 传递 std::function
void execute(std::function<void()> action) {
    action();
}
execute([]{ std::cout << "Hello\n"; });
```

### 函数作为返回值

```cpp
// 工厂函数：创建特化的函数
auto make_multiplier(int factor) {
    return [factor](int x) { return x * factor; };
}

auto times3 = make_multiplier(3);
auto times10 = make_multiplier(10);

times3(5);   // 15
times10(5);  // 50

// 验证器工厂
auto make_range_validator(int min, int max) {
    return [min, max](int x) {
        return x >= min && x <= max;
    };
}

auto is_valid_age = make_range_validator(0, 150);
auto is_valid_score = make_range_validator(0, 100);
```

### 存储函数

```cpp
// 使用 std::function 存储
std::vector<std::function<int(int)>> pipeline;

pipeline.push_back([](int x) { return x * 2; });
pipeline.push_back([](int x) { return x + 10; });
pipeline.push_back([](int x) { return x * x; });

int result = 5;
for (const auto& op : pipeline) {
    result = op(result);
}
// result = ((5 * 2) + 10)² = 400
```

## Map、Filter、Reduce

三个最基础、最重要的高阶函数。

### Map：转换每个元素

```cpp
template<typename T, typename F>
auto map(const std::vector<T>& vec, F f) {
    using R = std::invoke_result_t<F, const T&>;
    std::vector<R> result;
    result.reserve(vec.size());
    for (const auto& item : vec) {
        result.push_back(f(item));
    }
    return result;
}

// 使用
std::vector<int> nums = {1, 2, 3, 4, 5};
auto squares = map(nums, [](int x) { return x * x; });
// squares = {1, 4, 9, 16, 25}

auto strings = map(nums, [](int x) { return std::to_string(x); });
// strings = {"1", "2", "3", "4", "5"}
```

### Filter：保留满足条件的元素

```cpp
template<typename T, typename Pred>
std::vector<T> filter(const std::vector<T>& vec, Pred pred) {
    std::vector<T> result;
    for (const auto& item : vec) {
        if (pred(item)) {
            result.push_back(item);
        }
    }
    return result;
}

// 使用
std::vector<int> nums = {1, 2, 3, 4, 5};
auto evens = filter(nums, [](int x) { return x % 2 == 0; });
// evens = {2, 4}

auto large = filter(nums, [](int x) { return x > 3; });
// large = {4, 5}
```

### Reduce：归约为单个值

```cpp
template<typename T, typename Acc, typename F>
Acc reduce(const std::vector<T>& vec, Acc init, F f) {
    Acc result = init;
    for (const auto& item : vec) {
        result = f(result, item);
    }
    return result;
}

// 使用
std::vector<int> nums = {1, 2, 3, 4, 5};
auto sum = reduce(nums, 0, [](int acc, int x) { return acc + x; });
// sum = 15

auto product = reduce(nums, 1, [](int acc, int x) { return acc * x; });
// product = 120

auto max = reduce(nums, nums[0], [](int a, int b) { return a > b ? a : b; });
// max = 5
```

### 链式调用

```cpp
// 过滤偶数 -> 平方 -> 求和
auto result = reduce(
    map(
        filter(nums, [](int x) { return x % 2 == 0; }),
        [](int x) { return x * x; }
    ),
    0,
    [](int acc, int x) { return acc + x; }
);
// 2² + 4² = 4 + 16 = 20
```

### STL 等价物

```cpp
#include <algorithm>
#include <numeric>

// map → std::transform
std::vector<int> squares;
std::transform(nums.begin(), nums.end(),
               std::back_inserter(squares),
               [](int x) { return x * x; });

// filter → std::copy_if
std::vector<int> evens;
std::copy_if(nums.begin(), nums.end(),
             std::back_inserter(evens),
             [](int x) { return x % 2 == 0; });

// reduce → std::accumulate
int sum = std::accumulate(nums.begin(), nums.end(), 0);
```

## 概念预告

在掌握了高阶函数的基础后，接下来两章将深入三个重要概念：

- **柯里化（Currying）**：将多参数函数转换为一系列单参数函数，实现灵活的函数特化。详见 [Ch05 - 柯里化与偏应用](../05-currying-partial/README.md)。

- **偏函数应用（Partial Application）**：固定函数的部分参数，生成新的专用函数。与柯里化密切相关但有本质区别。详见 [Ch05](../05-currying-partial/README.md)。

- **函数组合（Function Composition）**：将多个小函数组合成一个大函数（`compose` 从右到左，`pipe` 从左到右），是构建数据处理管道的核心技术。详见 [Ch06 - 函数组合](../06-composition/README.md)。

这三个概念与高阶函数紧密相关——它们都建立在"函数是一等公民"的基础之上。

## 实用高阶函数

### any / all

```cpp
template<typename T, typename Pred>
bool any(const std::vector<T>& vec, Pred pred) {
    return std::any_of(vec.begin(), vec.end(), pred);
}

template<typename T, typename Pred>
bool all(const std::vector<T>& vec, Pred pred) {
    return std::all_of(vec.begin(), vec.end(), pred);
}

any(nums, [](int x) { return x > 10; });  // false
all(nums, [](int x) { return x > 0; });   // true
```

### take_while / drop_while

```cpp
template<typename T, typename Pred>
std::vector<T> take_while(const std::vector<T>& vec, Pred pred) {
    std::vector<T> result;
    for (const auto& item : vec) {
        if (!pred(item)) break;
        result.push_back(item);
    }
    return result;
}

std::vector<int> nums = {1, 2, 3, 4, 5, 6};
auto taken = take_while(nums, [](int x) { return x < 4; });
// taken = {1, 2, 3}
```

## 实践建议

1. **优先用 STL 算法**——`std::transform`、`std::accumulate` 等已优化
2. **lambda 简洁**——短小的 lambda 比命名函数更清晰
3. **用 `auto`**——避免写复杂的函数类型
4. **性能敏感用模板**——`std::function` 有虚调用开销
5. **避免过度抽象**——高阶函数很强大，但别滥用

## 代码示例

完整示例：`examples/01_higher_order_functions.cpp`

## 练习

1. **实现 zip**：合并两个数组为 pair 数组
2. **实现 flat_map**：map 后展平嵌套数组
3. **实现 group_by**：按条件分组
4. **数据处理管道**：用高阶函数处理复杂数据

练习文件：`exercises/exercises.cpp`
参考答案：`exercises/solutions.cpp`

## 下一章

[05 - 柯里化与偏应用](../05-currying-partial/README.md)
