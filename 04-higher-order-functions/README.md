# 04 - 高阶函数

> 函数作为一等公民，是函数式编程的核心。

## 本章目标

学完本章，你将能够：
- 理解高阶函数的概念
- 熟练使用 map、filter、reduce
- 掌握柯里化和偏应用
- 实现函数组合

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
    std::copy_if(vec.begin(), vec.end(),
                 std::back_inserter(result), pred);
    return result;
}

// 使用
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

## 柯里化（Currying）

将多参数函数转换为一系列单参数函数。

```cpp
// 原始函数
int add3(int a, int b, int c) {
    return a + b + c;
}

// 柯里化版本
auto curry_add3(int a) {
    return [a](int b) {
        return [a, b](int c) {
            return a + b + c;
        };
    };
}

// 使用
auto step1 = curry_add3(1);     // 固定 a=1
auto step2 = step1(2);          // 固定 b=2
int result = step2(3);          // c=3，结果 6

// 或一步到位
int r = curry_add3(1)(2)(3);    // 6
```

### 通用柯里化模板

```cpp
// 两参数柯里化
template<typename F>
auto curry2(F f) {
    return [f](auto a) {
        return [f, a](auto b) {
            return f(a, b);
        };
    };
}

// 三参数柯里化
template<typename F>
auto curry3(F f) {
    return [f](auto a) {
        return [f, a](auto b) {
            return [f, a, b](auto c) {
                return f(a, b, c);
            };
        };
    };
}

// 使用
auto multiply = [](int a, int b) { return a * b; };
auto times5 = curry2(multiply)(5);
times5(10);  // 50
```

### 柯里化的实际应用

```cpp
// 价格计算：通用 -> 特化
auto calc_price = curry2([](double tax_rate, double price) {
    return price * (1 + tax_rate);
});

auto with_vat = calc_price(0.2);        // 20% 增值税
auto with_sales_tax = calc_price(0.08); // 8% 销售税

with_vat(100);        // 120
with_sales_tax(100);  // 108

// 日志：通用 -> 特化
auto make_logger = curry2([](std::string level, std::string msg) {
    std::cout << "[" << level << "] " << msg << "\n";
});

auto log_error = make_logger("ERROR");
auto log_info = make_logger("INFO");

log_error("Something went wrong");
log_info("All good");
```

## 偏函数应用

固定函数的部分参数，返回新函数。

```cpp
template<typename F, typename Arg>
auto partial(F f, Arg arg) {
    return [f, arg](auto... rest) {
        return f(arg, rest...);
    };
}

// 使用
auto power = [](int base, int exp) {
    return static_cast<int>(std::pow(base, exp));
};

auto powers_of_2 = partial(power, 2);
powers_of_2(3);  // 2³ = 8
powers_of_2(10); // 2¹⁰ = 1024

auto cube = partial([](int exp, int base) {
    return static_cast<int>(std::pow(base, exp));
}, 3);
cube(2);  // 2³ = 8
cube(5);  // 5³ = 125
```

## 函数组合

### compose：从右到左

```cpp
template<typename F, typename G>
auto compose(F f, G g) {
    return [f, g](auto x) { return f(g(x)); };
}

auto add10 = [](int x) { return x + 10; };
auto double_it = [](int x) { return x * 2; };

auto double_then_add = compose(add10, double_it);
double_then_add(5);  // (5 * 2) + 10 = 20
```

### pipe：从左到右

```cpp
template<typename F, typename G>
auto pipe(F f, G g) {
    return [f, g](auto x) { return g(f(x)); };
}

auto add_then_double = pipe(add10, double_it);
add_then_double(5);  // (5 + 10) * 2 = 30
```

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
