# 第五章：函数组合 (Functional Composition)

## 核心概念

函数组合是将多个简单函数组合成复杂函数的技术。这是函数式编程的核心思想之一。

### 数学定义

在数学中，函数组合定义为：
```
(f ∘ g)(x) = f(g(x))
```

在 C++ 中，我们可以实现类似的机制。

## 1. Compose 和 Pipe

### Compose（从右到左）

```cpp
template<typename F, typename G>
auto compose(F f, G g) {
    return [f, g](auto x) {
        return f(g(x));
    };
}

// 使用
auto add10 = [](int x) { return x + 10; };
auto double_it = [](int x) { return x * 2; };

auto f = compose(add10, double_it);
// f(5) = add10(double_it(5)) = add10(10) = 20
```

**执行顺序**：从右到左（符合数学习惯）

### Pipe（从左到右）

```cpp
template<typename F, typename G>
auto pipe(F f, G g) {
    return [f, g](auto x) {
        return g(f(x));
    };
}

// 使用
auto f = pipe(double_it, add10);
// f(5) = add10(double_it(5)) = 20
```

**执行顺序**：从左到右（符合数据流动方向）

### 多函数组合

```cpp
// 可变参数模板实现
template<typename F>
auto compose_many(F f) {
    return f;
}

template<typename F, typename... Rest>
auto compose_many(F f, Rest... rest) {
    return [f, rest...](auto x) {
        return f(compose_many(rest...)(x));
    };
}

// 使用
auto f = compose_many(add10, double_it, square);
// f(3) = add10(double_it(square(3)))
//      = add10(double_it(9))
//      = add10(18)
//      = 28
```

## 2. 数据流转换

### 管道思维

将数据处理看作流水线：

```cpp
// 不使用组合
auto result = step3(step2(step1(data)));  // 难读

// 使用 pipe
auto pipeline = pipe(step1, step2, step3);
auto result = pipeline(data);  // 清晰
```

### 实际案例：文本处理

```cpp
auto trim = [](std::string s) {
    // 去除首尾空格
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

// 组合成处理管道
auto normalize = pipe(trim, to_upper, remove_spaces);

std::string input = "  Hello World  ";
auto result = normalize(input);  // "HELLOWORLD"
```

## 3. 点自由风格 (Point-Free Style)

点自由风格：不显式提及函数参数。

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

1. **更简洁**：不需要命名临时变量
2. **更抽象**：关注操作本身，而非数据
3. **更易组合**：函数成为"乐高积木"

### 何时使用

✅ **适合使用点自由风格**：
- 简单的函数组合
- 逻辑清晰的管道
- 可读性不受影响

❌ **避免使用点自由风格**：
- 过于复杂的组合（难以理解）
- 需要中间调试
- 多个参数需要重排

## 4. 函数组合的实际应用

### 4.1 数据验证管道

```cpp
struct User {
    std::string name;
    std::string email;
    int age;
};

// 各种验证函数
auto validate_name = [](const User& u) -> std::optional<User> {
    if (u.name.empty()) return std::nullopt;
    return u;
};

auto validate_email = [](const User& u) -> std::optional<User> {
    if (u.email.find('@') == std::string::npos) return std::nullopt;
    return u;
};

auto validate_age = [](const User& u) -> std::optional<User> {
    if (u.age < 0 || u.age > 150) return std::nullopt;
    return u;
};

// 组合验证器（需要处理 optional）
auto validate_user = [](const User& u) {
    return validate_name(u)
        .and_then(validate_email)
        .and_then(validate_age);
};
```

### 4.2 数据转换链

```cpp
// 产品价格计算
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

auto round_to_cents = [](double price) {
    return std::round(price * 100) / 100.0;
};

// 价格计算管道
auto calculate_final_price = pipe(
    apply_discount(0.1),   // 10% 折扣
    apply_tax(0.2),        // 20% 税
    round_to_cents         // 四舍五入到分
);

double original_price = 100.0;
double final_price = calculate_final_price(original_price);
// 100 -> 90 -> 108 -> 108.00
```

### 4.3 函数式错误处理

```cpp
template<typename T>
using Result = std::variant<T, std::string>;

// 安全除法
auto safe_divide = [](double a, double b) -> Result<double> {
    if (b == 0) return std::string("Division by zero");
    return a / b;
};

// 安全平方根
auto safe_sqrt = [](double x) -> Result<double> {
    if (x < 0) return std::string("Negative sqrt");
    return std::sqrt(x);
};

// 组合（需要 monadic bind）
auto safe_operation = [](double a, double b) {
    auto div_result = safe_divide(a, b);
    if (std::holds_alternative<std::string>(div_result)) {
        return div_result;
    }
    return safe_sqrt(std::get<double>(div_result));
};
```

## 5. 组合子 (Combinators)

组合子是操作函数的高阶函数。

### 常用组合子

```cpp
// K 组合子：常量函数
template<typename T>
auto K(T value) {
    return [value](auto...) { return value; };
}

// I 组合子：恒等函数
auto I = [](auto x) { return x; };

// S 组合子：应用组合
template<typename F, typename G>
auto S(F f, G g) {
    return [f, g](auto x) {
        return f(x)(g(x));
    };
}

// B 组合子：就是 compose
template<typename F, typename G>
auto B(F f, G g) {
    return compose(f, g);
}
```

### 实用组合子

```cpp
// flip：翻转二元函数的参数
template<typename F>
auto flip(F f) {
    return [f](auto a, auto b) {
        return f(b, a);
    };
}

// on：在应用函数前先转换
template<typename F, typename G>
auto on(F f, G g) {
    return [f, g](auto a, auto b) {
        return f(g(a), g(b));
    };
}

// 使用示例
auto subtract = [](int a, int b) { return a - b; };
auto flipped_sub = flip(subtract);
// flipped_sub(5, 3) = 3 - 5 = -2

auto compare_length = on(
    [](size_t a, size_t b) { return a < b; },
    [](const std::string& s) { return s.length(); }
);
// compare_length("hi", "hello") = true
```

## 6. 函数组合的陷阱

### 过度组合

❌ **糟糕**：
```cpp
auto f = compose(a, compose(b, compose(c, compose(d, compose(e, f)))));
// 难以阅读和调试
```

✅ **改进**：
```cpp
auto step1 = compose(b, a);
auto step2 = compose(d, c);
auto step3 = compose(f, e);
auto final = compose(step3, compose(step2, step1));
// 分步骤，易于理解
```

### 类型不匹配

```cpp
auto add10 = [](int x) { return x + 10; };
auto to_string = [](int x) { return std::to_string(x); };

// 错误：add10 接受 int，但 to_string 返回 string
// auto bad = compose(add10, to_string);  // 编译错误
```

### 性能考虑

每层组合都会创建新的 lambda，可能影响性能。对性能敏感的代码，考虑：
1. 手动内联
2. 使用编译器优化（`-O3`）
3. 避免过深的组合链

## 最佳实践

1. **保持简单**：每个函数做一件事
2. **命名清晰**：组合的函数应有描述性名称
3. **类型安全**：利用 C++ 类型系统捕获错误
4. **可测试**：分别测试每个组件，再测试组合
5. **文档化**：复杂组合需要注释说明数据流

## 总结

函数组合是构建复杂系统的基础：
- **compose/pipe**：基本组合工具
- **点自由风格**：更抽象的代码
- **管道思维**：数据流转换
- **组合子**：操作函数的函数

掌握函数组合，就能用小函数构建大系统。

## 相关资源

- 第 4 章：高阶函数（compose 的基础）
- 第 6 章：代数数据类型（更好的错误处理）
- 第 7 章：Monad（组合带副作用的函数）

## 练习

参见：`exercises/05_functional_composition_exercises.cpp`
