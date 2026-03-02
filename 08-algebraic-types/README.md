# 08 - 代数数据类型

> 用类型来表达领域逻辑。

## 本章目标

学完本章，你将能够：
- 理解积类型与和类型的区别
- 熟练使用 std::variant 和 std::optional
- 掌握 overload 模式进行模式匹配
- 用 ADT 建模状态机
- 实现递归数据结构

## 什么是代数数据类型

**代数数据类型 (ADT)** 是函数式编程中表达数据结构的核心方式。

为什么叫"代数"？因为类型的可能值数量可以用代数运算：

| 类型 | 运算 | 示例 |
|------|------|------|
| 积类型 | 乘法 | `struct {int x; bool b}` = int的值数 × 2 |
| 和类型 | 加法 | `variant<int, bool>` = int的值数 + 2 |

## 积类型 (Product Types)

积类型表示 **"AND"关系**：同时拥有多个字段。

### struct

```cpp
struct Point {
    int x;
    int y;
};
// Point 的可能值 = int可能值 × int可能值
```

### std::tuple

```cpp
// 匿名积类型
std::tuple<int, std::string, double> person{25, "Alice", 1.65};

// 结构化绑定
auto [age, name, height] = person;
```

### std::pair

```cpp
std::pair<std::string, int> score{"Alice", 95};
std::cout << score.first << ": " << score.second;
```

### C++20 改进

```cpp
// C++20: 默认比较运算符
struct Point {
    int x;
    int y;
    auto operator<=>(const Point&) const = default;
};

Point p1{1, 2}, p2{1, 2};
bool eq = (p1 == p2);  // true
```

## 和类型 (Sum Types)

和类型表示 **"OR"关系**：多个可能性之一。

### std::variant

```cpp
std::variant<int, double, std::string> value;

value = 42;       // 现在是 int
value = 3.14;     // 现在是 double
value = "hello";  // 现在是 string

// 类型安全访问
if (std::holds_alternative<int>(value)) {
    std::cout << std::get<int>(value);
}
```

### std::optional

```cpp
std::optional<int> maybe_value;

maybe_value = 42;            // 有值
maybe_value = std::nullopt;  // 无值

// 安全访问
if (maybe_value) {
    std::cout << *maybe_value;
}

// 带默认值
int x = maybe_value.value_or(0);
```

## 模式匹配

### std::visit

```cpp
std::variant<int, std::string> v = 42;

std::visit([](auto&& arg) {
    using T = std::decay_t<decltype(arg)>;
    if constexpr (std::is_same_v<T, int>) {
        std::cout << "整数: " << arg << "\n";
    } else {
        std::cout << "字符串: " << arg << "\n";
    }
}, v);
```

### overload 模式

更优雅的方式：

```cpp
// 重载辅助工具
template<class... Ts>
struct overload : Ts... {
    using Ts::operator()...;
};

// 使用
std::visit(overload{
    [](int i) { std::cout << "整数: " << i << "\n"; },
    [](double d) { std::cout << "浮点: " << d << "\n"; },
    [](const std::string& s) { std::cout << "字符串: " << s << "\n"; }
}, value);
```

## 实际应用

### 状态机

用 variant 建模互斥状态：

```cpp
// 定义状态（C++20 默认比较）
struct Idle {
    auto operator<=>(const Idle&) const = default;
};

struct Running {
    int progress;
    auto operator<=>(const Running&) const = default;
};

struct Completed {
    std::string result;
    auto operator<=>(const Completed&) const = default;
};

struct Failed {
    std::string error;
    auto operator<=>(const Failed&) const = default;
};

using TaskState = std::variant<Idle, Running, Completed, Failed>;

void handle_state(const TaskState& state) {
    std::visit(overload{
        [](const Idle&) { std::cout << "等待中...\n"; },
        [](const Running& r) { std::cout << "进度: " << r.progress << "%\n"; },
        [](const Completed& c) { std::cout << "完成: " << c.result << "\n"; },
        [](const Failed& f) { std::cout << "失败: " << f.error << "\n"; }
    }, state);
}
```

**优势**：不可能同时处于多个状态（编译时保证）。

### API 响应

```cpp
struct Success {
    std::string data;
};

struct NotFound {
    std::string resource;
};

struct ServerError {
    int code;
    std::string message;
};

using ApiResponse = std::variant<Success, NotFound, ServerError>;

void handle_response(const ApiResponse& resp) {
    std::visit(overload{
        [](const Success& s) {
            std::cout << "数据: " << s.data << "\n";
        },
        [](const NotFound& nf) {
            std::cout << "404: " << nf.resource << " 未找到\n";
        },
        [](const ServerError& se) {
            std::cout << "错误 " << se.code << ": " << se.message << "\n";
        }
    }, resp);
}
```

### 表达式树

递归 ADT：

```cpp
struct Expr;
using ExprPtr = std::shared_ptr<Expr>;

struct Literal { double value; };
struct Add { ExprPtr left, right; };
struct Mul { ExprPtr left, right; };
struct Div { ExprPtr left, right; };

struct Expr {
    std::variant<Literal, Add, Mul, Div> data;
};

// 构建辅助函数
ExprPtr lit(double value);
ExprPtr add(ExprPtr left, ExprPtr right);
ExprPtr mul(ExprPtr left, ExprPtr right);
ExprPtr div(ExprPtr left, ExprPtr right);

// 求值函数
double evaluate(const Expr& expr) {
    return std::visit(overload{
        [](const Literal& l) { return l.value; },
        [](const Add& a) { return evaluate(*a.left) + evaluate(*a.right); },
        [](const Mul& m) { return evaluate(*m.left) * evaluate(*m.right); },
        [](const Div& d) { return evaluate(*d.left) / evaluate(*d.right); }
    }, expr.data);
}
```

### 配置值

```cpp
using ConfigValue = std::variant<int, double, std::string, bool>;

template<typename T>
std::optional<T> get_as(const ConfigValue& v) {
    if (std::holds_alternative<T>(v)) {
        return std::get<T>(v);
    }
    return std::nullopt;
}

std::string to_string(const ConfigValue& v) {
    return std::visit(overload{
        [](int i) { return std::to_string(i); },
        [](double d) { return std::to_string(d); },
        [](const std::string& s) { return s; },
        [](bool b) { return std::string(b ? "true" : "false"); }
    }, v);
}
```

## 不可能状态

### 反例：布尔标记

```cpp
// 危险：允许非法状态
struct Connection {
    bool is_connecting;
    bool is_connected;
    bool is_failed;
    // 可能同时 is_connecting=true && is_connected=true！
};
```

### 正确方式：variant

```cpp
// 安全：状态互斥
using ConnectionState = std::variant<
    Disconnected,
    Connecting,
    Connected,
    Failed
>;
// 不可能同时处于多个状态
```

## 与继承对比

| 特性 | variant | 继承 |
|------|---------|------|
| 添加新类型 | 需修改 variant | 只需新增子类 |
| 添加新操作 | 只需新增 visit | 需修改所有子类 |
| 穷尽检查 | 编译器警告 | 需手动处理 |
| 开闭原则 | 对操作开放 | 对类型开放 |

选择建议：
- **类型固定，操作多变**：使用 variant（如：AST、协议消息）
- **类型多变，操作固定**：使用继承（如：插件系统）

## 最佳实践

1. **优先 variant 而非 void***：类型安全
2. **使用 optional 而非空指针**：明确表达"可能没有值"
3. **用 variant 建模状态机**：避免非法状态组合
4. **使用 overload 模式**：简化 visit 代码
5. **C++20：使用默认比较**：`auto operator<=>(const T&) const = default;`

## 代码示例

完整示例：`examples/01_algebraic_types.cpp`

## 练习

1. **Result 类型**：实现带 map/and_then 的 Result<T, E>
2. **状态机**：用 variant 实现连接状态机
3. **表达式树**：实现表达式求值和打印
4. **Maybe 类型**：用 variant 实现 Maybe<T>
5. **配置系统**：实现多类型配置值

练习文件：`exercises/exercises.cpp`
参考答案：`exercises/solutions.cpp`

## 下一章

[09 - 错误处理与 std::expected](../09-error-handling/README.md)
