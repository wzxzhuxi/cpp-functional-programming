# 04 - 高阶函数

## 什么是高阶函数

**高阶函数（Higher-Order Function）**：满足以下至少一个条件的函数。

1. **接受函数作为参数**
2. **返回函数作为结果**

```cpp
// 高阶函数：接受函数作为参数
int apply(int x, int (*f)(int)) {
    return f(x);
}

// 高阶函数：返回函数
auto make_adder(int n) {
    return [n](int x) { return x + n; };
}

// 既接受又返回函数
auto compose(auto f, auto g) {
    return [f, g](auto x) { return f(g(x)); };
}
```

高阶函数是函数式编程的核心工具。

## 函数作为一等公民

C++ 中函数是一等公民：可以像普通值一样使用。

### 函数作为参数

```cpp
// 传递函数指针
int apply_twice(int x, int (*f)(int)) {
    return f(f(x));
}

int square(int x) { return x * x; }
apply_twice(3, square);  // square(square(3)) = 81

// 传递 lambda
auto result = apply_twice(3, [](int x) { return x + 1; });  // 5

// 传递 std::function
void execute(std::function<void()> action) {
    action();
}

execute([]{ std::cout << "Hello\n"; });
```

### 函数作为返回值

```cpp
// 返回 lambda
auto make_multiplier(int factor) {
    return [factor](int x) { return x * factor; };
}

auto times3 = make_multiplier(3);
times3(10);  // 30

// 返回复杂函数
auto make_validator(int min, int max) {
    return [min, max](int x) {
        return x >= min && x <= max;
    };
}

auto is_valid_age = make_validator(0, 150);
is_valid_age(25);   // true
is_valid_age(200);  // false
```

### 存储函数

```cpp
// 使用 std::function 存储
std::vector<std::function<int(int)>> operations;

operations.push_back([](int x) { return x * 2; });
operations.push_back([](int x) { return x + 10; });
operations.push_back([](int x) { return x * x; });

int result = 5;
for (const auto& op : operations) {
    result = op(result);
}
// result = ((5 * 2) + 10)^2 = 400
```

## Map、Filter、Reduce

三个最基础的高阶函数。

### Map：转换

将函数应用到每个元素，返回新集合。

```cpp
template<typename T, typename Func>
auto map(const std::vector<T>& vec, Func f) {
    using R = decltype(f(std::declval<T>()));
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
```

### Filter：过滤

保留满足条件的元素。

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
auto evens = filter(nums, [](int x) { return x % 2 == 0; });
// evens = {2, 4}
```

### Reduce：归约

将集合归约为单个值。

```cpp
template<typename T, typename Acc, typename Func>
Acc reduce(const std::vector<T>& vec, Acc init, Func f) {
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
```

### STL 等价物

C++ 标准库已经提供了这些功能：

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
int product = std::accumulate(nums.begin(), nums.end(), 1,
                               std::multiplies<>());
```

## 柯里化（Currying）

将多参数函数转换为一系列单参数函数。

```cpp
// 原始函数：多参数
int add(int a, int b, int c) {
    return a + b + c;
}

// 柯里化版本
auto curry_add(int a) {
    return [a](int b) {
        return [a, b](int c) {
            return a + b + c;
        };
    };
}

// 使用
auto step1 = curry_add(1);      // 部分应用 a=1
auto step2 = step1(2);          // 部分应用 b=2
int result = step2(3);          // 完全应用 c=3，结果 6

// 或一次性调用
int result2 = curry_add(1)(2)(3);  // 6
```

### 通用柯里化

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

// 使用
auto multiply = [](int a, int b) { return a * b; };
auto curried_mult = curry2(multiply);

auto times5 = curried_mult(5);
times5(10);  // 50
times5(3);   // 15
```

### 为什么柯里化有用

1. **部分应用** - 固定部分参数，创建新函数
2. **函数组合** - 更容易组合单参数函数
3. **代码复用** - 通用函数 → 特化函数

```cpp
// 通用计算价格函数
auto calc_price = curry2([](double rate, double price) {
    return price * (1 + rate);
});

// 特化：不同税率
auto with_vat = calc_price(0.2);      // 20% 税率
auto with_sales_tax = calc_price(0.1);  // 10% 税率

with_vat(100);         // 120
with_sales_tax(100);   // 110
```

## 偏函数应用（Partial Application）

固定函数的部分参数，返回新函数。

```cpp
// 使用 std::bind
auto multiply = [](int a, int b) { return a * b; };

auto double_it = std::bind(multiply, 2, std::placeholders::_1);
double_it(5);  // 10

auto add_three = [](int a, int b, int c) { return a + b + c; };
auto add_to_10 = std::bind(add_three, std::placeholders::_1, std::placeholders::_2, 10);
add_to_10(5, 3);  // 18
```

### 手动实现偏应用

```cpp
// 简单的偏应用
template<typename Func, typename Arg>
auto partial(Func f, Arg arg) {
    return [f, arg](auto... rest) {
        return f(arg, rest...);
    };
}

// 使用
auto power = [](int base, int exp) {
    int result = 1;
    for (int i = 0; i < exp; ++i) result *= base;
    return result;
};

auto square = partial(power, 2);  // 固定 base=2
square(3);  // 2^3 = 8
square(4);  // 2^4 = 16

auto cube_of = partial([](int exp, int base) {
    return std::pow(base, exp);
}, 3);
cube_of(2);  // 2^3 = 8
cube_of(5);  // 5^3 = 125
```

## 函数组合

组合多个函数形成新函数。

```cpp
// compose: (f ∘ g)(x) = f(g(x))
template<typename F, typename G>
auto compose(F f, G g) {
    return [f, g](auto x) { return f(g(x)); };
}

// 使用
auto add10 = [](int x) { return x + 10; };
auto double_it = [](int x) { return x * 2; };

auto double_then_add10 = compose(add10, double_it);
double_then_add10(5);  // (5 * 2) + 10 = 20

// 多层组合
auto square = [](int x) { return x * x; };
auto complex = compose(compose(add10, double_it), square);
complex(3);  // ((3^2) * 2) + 10 = 28
```

### Pipe：反向组合

```cpp
// pipe: x |> f |> g = g(f(x))
template<typename F, typename G>
auto pipe(F f, G g) {
    return [f, g](auto x) { return g(f(x)); };
}

// 更自然的流式调用
auto process = pipe(
    pipe(square, double_it),
    add10
);
process(3);  // 28，同上

// C++20 ranges 提供了 | 语法
// nums | std::views::transform(f) | std::views::filter(pred)
```

## 实用高阶函数

### ForEach

```cpp
template<typename T, typename Action>
void for_each(const std::vector<T>& vec, Action action) {
    for (const auto& item : vec) {
        action(item);
    }
}

for_each(nums, [](int x) {
    std::cout << x << " ";
});
```

### Any / All

```cpp
template<typename T, typename Pred>
bool any(const std::vector<T>& vec, Pred pred) {
    for (const auto& item : vec) {
        if (pred(item)) return true;
    }
    return false;
}

template<typename T, typename Pred>
bool all(const std::vector<T>& vec, Pred pred) {
    for (const auto& item : vec) {
        if (!pred(item)) return false;
    }
    return true;
}

any(nums, [](int x) { return x > 10; });  // false
all(nums, [](int x) { return x > 0; });   // true
```

### TakeWhile / DropWhile

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

1. **优先用 STL** - `std::transform`、`std::accumulate` 等已优化
2. **lambda 简洁** - 短小的 lambda 比命名函数更清晰
3. **类型推导** - 用 `auto` 和模板，避免写复杂类型
4. **避免过度抽象** - 高阶函数很强大，但别滥用
5. **性能注意** - `std::function` 有开销，性能敏感用模板

## 代码示例

完整示例见：`examples/basic/04_higher_order_functions.cpp`

## 练习

1. 实现 `map`、`filter`、`reduce` 的通用版本
2. 实现三参数柯里化 `curry3`
3. 实现 `compose` 支持任意多个函数组合
4. 用高阶函数实现复杂数据处理管道

练习题在：`exercises/04_higher_order_functions_exercises.cpp`
