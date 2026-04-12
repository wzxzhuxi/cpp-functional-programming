# 05 - 柯里化与偏应用

> 将函数拆解为更小的构建块。

## 本章目标

学完本章，你将能够：
- 理解柯里化的概念和实现
- 掌握偏应用的技巧
- 区分柯里化和偏应用
- 使用 `std::bind` 和 lambda 实现偏应用
- 构建灵活的函数工厂

## 柯里化（Currying）

**柯里化**：将接受多个参数的函数转换为一系列接受单个参数的函数。

```cpp
// 原始函数
int add(int a, int b, int c) {
    return a + b + c;
}
add(1, 2, 3);  // 6

// 柯里化版本
auto curry_add(int a) {
    return [a](int b) {
        return [a, b](int c) {
            return a + b + c;
        };
    };
}
curry_add(1)(2)(3);  // 6
```

### 柯里化的优势

1. **部分应用**：固定部分参数，生成专用函数
2. **延迟计算**：逐步提供参数，最后才计算
3. **函数组合**：单参函数更易于组合

```cpp
auto add1 = curry_add(1);       // 固定 a=1
auto add1_2 = add1(2);          // 固定 b=2
auto result = add1_2(3);        // c=3, 结果 6

// 也可以一步到位
curry_add(1)(2)(3);             // 6
```

## 通用柯里化模板

### 两参数柯里化

```cpp
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
auto curried = curry2(multiply);

auto times5 = curried(5);
times5(10);  // 50
times5(3);   // 15
```

### 三参数柯里化

```cpp
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
auto volume = [](int l, int w, int h) { return l * w * h; };
auto curried = curry3(volume);

curried(2)(3)(4);  // 24
```

### 自动柯里化（C++17/20）

```cpp
// 使用 if constexpr 和折叠表达式
template<typename F, typename... CapturedArgs>
class Curried {
    F f_;
    std::tuple<CapturedArgs...> captured_;

public:
    Curried(F f, CapturedArgs... args)
        : f_(f), captured_(std::move(args)...) {}

    template<typename Arg>
    auto operator()(Arg arg) const {
        auto new_args = std::tuple_cat(captured_, std::make_tuple(arg));

        if constexpr (std::is_invocable_v<F, CapturedArgs..., Arg>) {
            return std::apply(f_, new_args);
        } else {
            return std::apply([&](auto&&... args) {
                return Curried<F, CapturedArgs..., Arg>(
                    f_, std::forward<decltype(args)>(args)..., arg
                );
            }, captured_);
        }
    }
};

template<typename F>
auto curry(F f) {
    return Curried<F>(f);
}
```

## 偏应用（Partial Application）

**偏应用**：固定函数的部分参数，返回接受剩余参数的新函数。

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
powers_of_2(3);   // 2³ = 8
powers_of_2(10);  // 2¹⁰ = 1024
```

### 柯里化 vs 偏应用

| 特性 | 柯里化 | 偏应用 |
|------|--------|--------|
| 参数数量 | 每次接受一个 | 可以接受多个 |
| 返回类型 | 总是返回函数（直到最后） | 可能返回函数或值 |
| 参数顺序 | 从左到右固定 | 可以固定任意位置 |
| 嵌套层数 | 等于参数个数 | 通常一层 |

```cpp
// 柯里化：严格单参数
curry_add(1)(2)(3);  // 必须一个一个传

// 偏应用：可以一次固定多个
auto add_to_10 = partial_n(add, 1, 2);  // 固定前两个
add_to_10(7);  // 1 + 2 + 7 = 10
```

## 使用 std::bind

标准库提供的偏应用工具。

```cpp
#include <functional>

auto multiply = [](int a, int b) { return a * b; };

// 固定第一个参数
auto times5 = std::bind(multiply, 5, std::placeholders::_1);
times5(10);  // 50

// 固定第二个参数
auto double_it = std::bind(multiply, std::placeholders::_1, 2);
double_it(7);  // 14
```

### std::bind 的问题

1. **可读性差**：哪怕只是固定第二个参数，也要先读懂 `_1`, `_2`
2. **类型擦除**：返回复杂类型
3. **性能开销**：可能有间接调用

**推荐使用 lambda**：

```cpp
// std::bind 版本
auto times5 = std::bind(multiply, 5, std::placeholders::_1);

// lambda 版本（更清晰）
auto times5 = [](int x) { return multiply(5, x); };

// 或使用自定义 partial
auto times5 = partial(multiply, 5);
```

## 多参数偏应用

```cpp
// 固定多个参数
template<typename F, typename... Args>
auto partial_n(F f, Args... args) {
    return [f, args...](auto... rest) {
        return f(args..., rest...);
    };
}

// 从右侧固定
template<typename F, typename... Args>
auto partial_right(F f, Args... args) {
    return [f, args...](auto... first) {
        return f(first..., args...);
    };
}

// 使用
auto greet = [](std::string greeting, std::string name, std::string punctuation) {
    return greeting + ", " + name + punctuation;
};

auto say_hello = partial_n(greet, "Hello");
say_hello("World", "!");  // "Hello, World!"

auto excited = partial_right(greet, "!");
excited("Hi", "Alice");   // "Hi, Alice!"
```

## 实际应用

### 配置工厂

```cpp
// 日志函数工厂
auto make_logger(std::string level) {
    return [level](std::string message) {
        std::cout << "[" << level << "] " << message << "\n";
    };
}

auto log_info = make_logger("INFO");
auto log_error = make_logger("ERROR");
auto log_debug = make_logger("DEBUG");

log_info("Server started");
log_error("Connection failed");
```

### 价格计算

```cpp
auto calc_price = curry2([](double tax_rate, double price) {
    return price * (1 + tax_rate);
});

auto with_vat = calc_price(0.2);         // 20% 增值税
auto with_sales_tax = calc_price(0.08);  // 8% 销售税

with_vat(100);        // 120
with_sales_tax(100);  // 108
```

### 验证器

```cpp
auto make_range_validator(int min, int max) {
    return [min, max](int value) {
        return value >= min && value <= max;
    };
}

auto is_valid_age = make_range_validator(0, 150);
auto is_valid_score = make_range_validator(0, 100);
auto is_valid_month = make_range_validator(1, 12);

is_valid_age(25);    // true
is_valid_score(105); // false
```

### 字符串格式化

```cpp
// 闭包工厂：标签名 → 包装函数
auto tag = [](std::string name) {
    return [name](std::string content) {
        return "<" + name + ">" + content + "</" + name + ">";
    };
};

auto bold = tag("b");
auto italic = tag("i");

bold("Hello");    // "<b>Hello</b>"
italic("World");  // "<i>World</i>"
```

柯里化更适合参数能直接映射到 prefix/suffix 的场景：

```cpp
auto wrap = curry2([](std::string open, std::string close) {
    return [open, close](std::string content) {
        return open + content + close;
    };
});

auto parens = wrap("(")(")");
auto brackets = wrap("[{")("}]");

parens("hello");    // "(hello)"
brackets("1, 2");   // "[{1, 2}]"
```

## 最佳实践

1. **参数顺序**：把最常固定的参数放前面
2. **命名清晰**：柯里化后的函数要有描述性名称
3. **适度使用**：不要过度柯里化，保持可读性
4. **类型推导**：利用 `auto` 避免复杂类型
5. **Lambda 优先**：比 `std::bind` 更清晰

### 参数顺序设计

```cpp
// 不好：常变参数在前
auto log = [](std::string message, std::string level) { ... };
// 每次都要重复 level

// 好：常固定参数在前
auto log = [](std::string level, std::string message) { ... };
auto log_info = partial(log, "INFO");
// 只需调用 log_info("message")
```

## 代码示例

完整示例：`examples/01_currying_partial.cpp`

## 练习

1. **实现 curry4**：柯里化四参数函数
2. **实现 partial_at**：固定任意位置的参数
3. **配置系统**：用柯里化构建 HTTP 请求工厂
4. **组合柯里化函数**：与函数组合结合使用

练习文件：`exercises/exercises.cpp`
参考答案：`exercises/solutions.cpp`

## 下一章

[06 - 函数组合](../06-composition/README.md)
