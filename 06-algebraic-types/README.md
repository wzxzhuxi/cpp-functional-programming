# 第六章：代数数据类型 (Algebraic Data Types)

## 核心概念

代数数据类型 (ADT) 是函数式编程中表达数据结构的方式，分为两大类：
- **积类型 (Product Types)**：多个值的组合（如 struct、tuple）
- **和类型 (Sum Types)**：多个可能性之一（如 variant、optional）

## 1. 积类型 (Product Types)

积类型表示"AND"关系：同时拥有多个字段。

### Struct

```cpp
struct Point {
    int x;
    int y;
};
// Point 的可能值数量 = int 的数量 × int 的数量
```

### Tuple

```cpp
std::tuple<int, std::string, double> person{25, "Alice", 1.65};

auto [age, name, height] = person;  // 结构化绑定
```

### Pair

```cpp
std::pair<std::string, int> score{"Alice", 95};
```

## 2. 和类型 (Sum Types)

和类型表示"OR"关系：多个可能性之一。

### std::variant

```cpp
std::variant<int, double, std::string> value;

value = 42;           // 现在是 int
value = 3.14;         // 现在是 double
value = "hello";      // 现在是 string

// 访问
std::visit([](auto&& arg) {
    std::cout << arg << "\n";
}, value);
```

### std::optional

```cpp
std::optional<int> maybe_value;

maybe_value = 42;           // 有值
maybe_value = std::nullopt; // 无值

if (maybe_value) {
    std::cout << *maybe_value;
}
```

## 3. 模式匹配

C++ 没有原生模式匹配，但可以用 `std::visit` 模拟。

### 基础访问

```cpp
std::variant<int, std::string> v = 42;

std::visit([](auto&& arg) {
    using T = std::decay_t<decltype(arg)>;
    if constexpr (std::is_same_v<T, int>) {
        std::cout << "Integer: " << arg << "\n";
    } else if constexpr (std::is_same_v<T, std::string>) {
        std::cout << "String: " << arg << "\n";
    }
}, v);
```

### 重载访问器

```cpp
template<class... Ts>
struct overload : Ts... {
    using Ts::operator()...;
};

template<class... Ts>
overload(Ts...) -> overload<Ts...>;

// 使用
std::visit(overload{
    [](int i) { std::cout << "Int: " << i << "\n"; },
    [](const std::string& s) { std::cout << "String: " << s << "\n"; }
}, v);
```

## 4. Result/Either 类型

用于函数式错误处理。

### 简单实现

```cpp
template<typename T, typename E>
class Result {
public:
    std::variant<T, E> data;

    bool is_ok() const {
        return std::holds_alternative<T>(data);
    }

    const T& unwrap() const {
        return std::get<T>(data);
    }

    const E& unwrap_err() const {
        return std::get<E>(data);
    }
};

// 使用
Result<int, std::string> divide(int a, int b) {
    if (b == 0) {
        return {std::string("Division by zero")};
    }
    return {a / b};
}
```

## 5. 递归类型

### 表达式树

```cpp
struct Expr;

using ExprPtr = std::shared_ptr<Expr>;

struct Literal { int value; };
struct Add { ExprPtr left, right; };
struct Mul { ExprPtr left, right; };

struct Expr {
    std::variant<Literal, Add, Mul> data;
};

// 求值
int eval(const Expr& expr) {
    return std::visit(overload{
        [](const Literal& lit) { return lit.value; },
        [](const Add& add) { return eval(*add.left) + eval(*add.right); },
        [](const Mul& mul) { return eval(*mul.left) * eval(*mul.right); }
    }, expr.data);
}
```

## 6. 实际应用

### 6.1 状态机

```cpp
struct Idle {};
struct Running { int progress; };
struct Completed { std::string result; };
struct Failed { std::string error; };

using State = std::variant<Idle, Running, Completed, Failed>;

void handle_state(const State& state) {
    std::visit(overload{
        [](const Idle&) { std::cout << "Waiting...\n"; },
        [](const Running& r) { std::cout << "Progress: " << r.progress << "%\n"; },
        [](const Completed& c) { std::cout << "Done: " << c.result << "\n"; },
        [](const Failed& f) { std::cout << "Error: " << f.error << "\n"; }
    }, state);
}
```

### 6.2 配置解析

```cpp
struct Config {
    std::variant<int, double, std::string, bool> value;

    template<typename T>
    std::optional<T> get() const {
        if (std::holds_alternative<T>(value)) {
            return std::get<T>(value);
        }
        return std::nullopt;
    }
};
```

### 6.3 API 响应

```cpp
struct Success { std::string data; };
struct NotFound { std::string resource; };
struct ServerError { int code; std::string message; };

using ApiResponse = std::variant<Success, NotFound, ServerError>;

void handle_response(const ApiResponse& resp) {
    std::visit(overload{
        [](const Success& s) {
            std::cout << "Data: " << s.data << "\n";
        },
        [](const NotFound& nf) {
            std::cout << "404: " << nf.resource << " not found\n";
        },
        [](const ServerError& se) {
            std::cout << "Error " << se.code << ": " << se.message << "\n";
        }
    }, resp);
}
```

## 7. 优势

### 类型安全

使用 variant 而非 enum + union，编译器保证类型安全。

### 穷举检查

编译器会警告未处理的情况。

### 不可能状态

❌ **使用布尔标记（允许非法状态）**：
```cpp
struct Task {
    bool is_running;
    bool is_completed;
    // 可能同时 is_running=true && is_completed=true（非法！）
};
```

✅ **使用 variant（只允许合法状态）**：
```cpp
using TaskState = std::variant<Idle, Running, Completed>;
// 不可能同时处于多个状态
```

## 8. 最佳实践

1. **优先 variant 而非 void* 或 union**：类型安全
2. **使用 optional 而非空指针**：明确表达"可能没有值"
3. **Result 类型而非异常**：显式错误处理
4. **不可变 ADT**：所有字段 const
5. **使用 overload 模式**：简化 visit 代码

## 总结

代数数据类型提供：
- **类型安全**：编译器保证正确性
- **表达力**：精确建模领域
- **可组合性**：积类型 + 和类型 = 任意复杂结构
- **模式匹配**：优雅处理不同情况

## 练习

参见：`exercises/06_algebraic_types_exercises.cpp`
