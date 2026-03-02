# 06 - 函数组合

> 用小函数构建大系统。

## 本章目标

学完本章，你将能够：
- 理解函数组合的数学基础
- 熟练使用 compose 和 pipe
- 掌握常用组合子
- 使用点自由风格编程
- 构建数据处理管道

## 什么是函数组合

**函数组合**：将多个简单函数组合成复杂函数。

数学定义：
```
(f ∘ g)(x) = f(g(x))
```

C++ 实现：
```cpp
template<typename F, typename G>
auto compose(F f, G g) {
    return [f, g](auto x) {
        return f(g(x));
    };
}
```

## Compose vs Pipe

### compose：从右到左

```cpp
template<typename F, typename G>
auto compose(F f, G g) {
    return [f, g](auto x) { return f(g(x)); };
}

auto add10 = [](int x) { return x + 10; };
auto double_it = [](int x) { return x * 2; };

auto f = compose(add10, double_it);
f(5);  // add10(double_it(5)) = add10(10) = 20
```

**执行顺序**：`double_it` → `add10`（从右到左）

### pipe：从左到右

```cpp
template<typename F, typename G>
auto pipe(F f, G g) {
    return [f, g](auto x) { return g(f(x)); };
}

auto f = pipe(double_it, add10);
f(5);  // add10(double_it(5)) = 20
```

**执行顺序**：`double_it` → `add10`（从左到右，更直观）

### 选择建议

| 场景 | 推荐 | 原因 |
|------|------|------|
| 数学公式 | compose | 符合 `f ∘ g` 习惯 |
| 数据管道 | pipe | 符合数据流方向 |
| 链式调用 | pipe | 读起来更自然 |

## 多函数组合

### 可变参数 compose

```cpp
// 基础情况：单函数
template<typename F>
auto compose_n(F f) {
    return f;
}

// 递归：多函数
template<typename F, typename G, typename... Rest>
auto compose_n(F f, G g, Rest... rest) {
    return compose(f, compose_n(g, rest...));
}

// 使用
auto square = [](int x) { return x * x; };
auto f = compose_n(add10, double_it, square);
f(3);  // add10(double_it(square(3))) = add10(double_it(9)) = add10(18) = 28
```

### 可变参数 pipe

```cpp
template<typename F>
auto pipe_n(F f) {
    return f;
}

template<typename F, typename G, typename... Rest>
auto pipe_n(F f, G g, Rest... rest) {
    return pipe_n(pipe(f, g), rest...);
}

// 使用
auto f = pipe_n(square, double_it, add10);
f(3);  // 同样结果：28
```

## 点自由风格（Point-Free Style）

不显式提及参数的编程风格。

### 对比

```cpp
// 显式参数风格（point-ful）
auto process = [](int x) {
    return add10(double_it(x));
};

// 点自由风格（point-free）
auto process = compose(add10, double_it);
```

### 优势

1. **更简洁**：不需要命名中间变量
2. **更抽象**：关注操作本身
3. **更易组合**：函数成为"乐高积木"

### 何时使用

✅ **适合**：
- 简单的函数组合
- 清晰的数据管道
- 可读性好的场景

❌ **避免**：
- 过于复杂的组合
- 需要调试的代码
- 多参数需要重排

## 组合子（Combinators）

操作函数的高阶函数。

### 基本组合子

```cpp
// I 组合子：恒等函数
auto I = [](auto x) { return x; };

// K 组合子：常量函数
template<typename T>
auto K(T value) {
    return [value](auto...) { return value; };
}

// B 组合子：就是 compose
template<typename F, typename G>
auto B(F f, G g) {
    return compose(f, g);
}
```

### 实用组合子

```cpp
// flip：翻转参数顺序
template<typename F>
auto flip(F f) {
    return [f](auto a, auto b) {
        return f(b, a);
    };
}

auto subtract = [](int a, int b) { return a - b; };
auto flipped = flip(subtract);
subtract(10, 3);  // 7
flipped(10, 3);   // -7 (3 - 10)

// on：先转换再应用
template<typename F, typename G>
auto on(F f, G g) {
    return [f, g](auto a, auto b) {
        return f(g(a), g(b));
    };
}

auto length = [](const std::string& s) { return s.length(); };
auto less = [](size_t a, size_t b) { return a < b; };
auto compare_by_length = on(less, length);

compare_by_length("hi", "hello");  // true (2 < 5)
```

### both / either

```cpp
// both：两个谓词都满足
template<typename P, typename Q>
auto both(P p, Q q) {
    return [p, q](auto x) {
        return p(x) && q(x);
    };
}

// either：任一谓词满足
template<typename P, typename Q>
auto either(P p, Q q) {
    return [p, q](auto x) {
        return p(x) || q(x);
    };
}

auto is_positive = [](int x) { return x > 0; };
auto is_even = [](int x) { return x % 2 == 0; };

auto is_positive_even = both(is_positive, is_even);
is_positive_even(4);   // true
is_positive_even(-2);  // false
is_positive_even(3);   // false
```

## 实际应用

### 文本处理管道

```cpp
auto trim = [](std::string s) {
    s.erase(0, s.find_first_not_of(" \t\n\r"));
    s.erase(s.find_last_not_of(" \t\n\r") + 1);
    return s;
};

auto to_upper = [](std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), ::toupper);
    return s;
};

auto remove_spaces = [](std::string s) {
    s.erase(std::remove(s.begin(), s.end(), ' '), s.end());
    return s;
};

auto normalize = pipe_n(trim, to_upper, remove_spaces);

normalize("  Hello World  ");  // "HELLOWORLD"
```

### 价格计算管道

```cpp
auto apply_discount = [](double rate) {
    return [rate](double price) {
        return price * (1.0 - rate);
    };
};

auto apply_tax = [](double rate) {
    return [rate](double price) {
        return price * (1.0 + rate);
    };
};

auto round_cents = [](double price) {
    return std::round(price * 100) / 100.0;
};

auto calculate_price = pipe_n(
    apply_discount(0.1),   // 10% 折扣
    apply_tax(0.2),        // 20% 税
    round_cents
);

calculate_price(100.0);  // 108.00
```

### 数据验证

```cpp
auto make_validator = [](auto pred, std::string error) {
    return [pred, error](auto value) -> std::optional<std::string> {
        if (!pred(value)) {
            return error;
        }
        return std::nullopt;
    };
};

auto not_empty = make_validator(
    [](const std::string& s) { return !s.empty(); },
    "不能为空"
);

auto max_length = [](size_t n) {
    return make_validator(
        [n](const std::string& s) { return s.length() <= n; },
        "超出长度限制"
    );
};

// 组合验证器
auto validate_username = [](const std::string& s)
    -> std::vector<std::string>
{
    std::vector<std::string> errors;

    if (auto e = not_empty(s)) errors.push_back(*e);
    if (auto e = max_length(20)(s)) errors.push_back(*e);

    return errors;
};
```

## 性能考虑

### 组合深度

每层组合创建新的 lambda，可能影响性能：

```cpp
// 深度组合（可能有开销）
auto f = compose(a, compose(b, compose(c, compose(d, e))));

// 手动内联（更快但不灵活）
auto f = [](auto x) { return a(b(c(d(e(x))))); };
```

### 优化建议

1. **使用 -O2/-O3**：编译器通常能内联简单 lambda
2. **避免过深组合**：3-5 层通常没问题
3. **热路径手动内联**：性能关键代码手动合并
4. **测量再优化**：不要过早优化

## 最佳实践

1. **保持简单**：每个函数做一件事
2. **命名清晰**：`calculate_tax` 比 `f` 好
3. **类型安全**：让编译器捕获类型不匹配
4. **可测试**：单独测试每个组件
5. **适度使用**：不要为了函数式而函数式

## 代码示例

完整示例：`examples/01_composition.cpp`

## 练习

1. **实现 compose_all**：接受函数列表，返回组合
2. **实现 when**：条件组合子
3. **文本格式化器**：用组合构建 Markdown 格式化器
4. **验证管道**：组合多个验证器

练习文件：`exercises/exercises.cpp`
参考答案：`exercises/solutions.cpp`

## 下一章

[07 - C++20 Ranges](../07-ranges/README.md)
