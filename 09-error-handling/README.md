# 09 - 错误处理与 std::expected

> 让错误成为类型系统的一部分。

## 本章目标

学完本章，你将能够：
- 理解函数式错误处理的哲学
- 区分异常与返回值错误处理
- 实现类型安全的 Result<T, E> 类型
- 使用 std::expected (C++23)
- 组合可能失败的操作

## 为什么需要函数式错误处理

### 传统方式的问题

**1. 异常的隐式控制流**

```cpp
// 问题：调用者不知道会抛异常
int parse_int(const std::string& s) {
    return std::stoi(s);  // 可能抛出 std::invalid_argument
}

// 调用者可能忘记处理
int value = parse_int(user_input);  // 危险！
```

**2. 返回码的歧义**

```cpp
// 问题：-1 是错误还是有效值？
int find_index(const std::vector<int>& v, int target);

// 问题：0 可能是成功也可能是失败
int process_file(const char* path);
```

**3. 输出参数的尴尬**

```cpp
// 问题：接口笨拙，容易出错
bool parse_int(const std::string& s, int& result);

int value;
if (parse_int(input, value)) {
    // 使用 value
}
```

### 函数式的解决方案

**核心思想**：让错误成为返回类型的一部分。

```cpp
// 类型签名明确告诉你：这个操作可能失败
Result<int, ParseError> parse_int(const std::string& s);

// 编译器强制你处理错误
auto result = parse_int(input);
if (result.is_ok()) {
    use(result.unwrap());
} else {
    handle_error(result.unwrap_err());
}
```

## std::optional：表达"可能没有"

### 基本用法

```cpp
#include <optional>

std::optional<int> find_first_even(const std::vector<int>& nums) {
    for (int n : nums) {
        if (n % 2 == 0) return n;
    }
    return std::nullopt;  // 明确表示"没有"
}

// 使用
auto result = find_first_even({1, 3, 5});
if (result) {
    std::cout << "Found: " << *result << "\n";
} else {
    std::cout << "Not found\n";
}

// 或使用 value_or
int value = result.value_or(-1);
```

### optional 的局限

```cpp
// 问题：为什么失败？
std::optional<User> find_user(int id);

// 失败原因：
// - 用户不存在？
// - 数据库连接失败？
// - 权限不足？
// optional 无法区分！
```

**结论**：`optional` 适合"可能没有值"，不适合"可能失败并需要知道原因"。

## Result<T, E>：带错误信息的结果

### 基本实现

```cpp
template<typename T, typename E>
class Result {
    std::variant<T, E> data_;

public:
    // 工厂函数
    static Result ok(T value) {
        Result r;
        r.data_ = std::move(value);
        return r;
    }

    static Result err(E error) {
        Result r;
        r.data_.template emplace<1>(std::move(error));
        return r;
    }

    // 查询
    [[nodiscard]] bool is_ok() const {
        return data_.index() == 0;
    }

    [[nodiscard]] bool is_err() const {
        return data_.index() == 1;
    }

    // 访问
    [[nodiscard]] const T& unwrap() const {
        if (is_err()) throw std::runtime_error("unwrap on Err");
        return std::get<0>(data_);
    }

    [[nodiscard]] const E& unwrap_err() const {
        if (is_ok()) throw std::runtime_error("unwrap_err on Ok");
        return std::get<1>(data_);
    }

    [[nodiscard]] T unwrap_or(T default_value) const {
        return is_ok() ? std::get<0>(data_) : default_value;
    }
};
```

### 使用示例

```cpp
enum class DivideError { DivisionByZero };

Result<double, DivideError> safe_divide(double a, double b) {
    if (b == 0.0) {
        return Result<double, DivideError>::err(DivideError::DivisionByZero);
    }
    return Result<double, DivideError>::ok(a / b);
}

// 使用
auto result = safe_divide(10.0, 2.0);
if (result.is_ok()) {
    std::cout << "Result: " << result.unwrap() << "\n";
} else {
    std::cout << "Error: division by zero\n";
}
```

## 函数式组合：map 和 and_then

### map：转换成功值

```cpp
template<typename T, typename E>
class Result {
    // ... 基本实现 ...

    template<typename F>
    [[nodiscard]] auto map(F f) const {
        using NewT = std::invoke_result_t<F, T>;
        if (is_ok()) {
            return Result<NewT, E>::ok(f(unwrap()));
        }
        return Result<NewT, E>::err(unwrap_err());
    }
};

// 使用
auto result = safe_divide(10.0, 2.0)
    .map([](double x) { return x * 2; })      // 10.0
    .map([](double x) { return x + 1; });     // 11.0
```

**类比**：`map` 类似于 `std::transform`，但只在成功时应用。

### and_then：链式可能失败的操作

```cpp
template<typename T, typename E>
class Result {
    template<typename F>
    [[nodiscard]] auto and_then(F f) const {
        // F 返回 Result<NewT, E>
        if (is_ok()) {
            return f(unwrap());
        }
        using NewResult = std::invoke_result_t<F, T>;
        using NewT = typename NewResult::value_type;
        return Result<NewT, E>::err(unwrap_err());
    }
};

// 使用：链式可能失败的操作
auto result = parse_int(input)
    .and_then([](int x) { return safe_divide(100.0, x); })
    .and_then([](double x) { return safe_sqrt(x); });
```

**关键区别**：
- `map(f)`：`f` 返回普通值 `T`
- `and_then(f)`：`f` 返回 `Result<T, E>`

### 图示

```
map:      Result<A, E> ---(A -> B)---> Result<B, E>
and_then: Result<A, E> ---(A -> Result<B, E>)---> Result<B, E>
```

## std::expected (C++23)

C++23 引入了标准库的 `std::expected`，功能类似于 `Result`。

### 基本用法

```cpp
#include <expected>  // C++23

std::expected<int, std::string> parse_int(const std::string& s) {
    try {
        return std::stoi(s);
    } catch (const std::exception& e) {
        return std::unexpected(std::string("Parse error: ") + e.what());
    }
}

// 使用
auto result = parse_int("42");
if (result) {
    std::cout << "Value: " << *result << "\n";
} else {
    std::cout << "Error: " << result.error() << "\n";
}

// value_or
int value = result.value_or(-1);
```

### transform 和 and_then

```cpp
// C++23 expected 支持函数式组合
auto result = parse_int(input)
    .transform([](int x) { return x * 2; })        // map 的别名
    .and_then([](int x) -> std::expected<double, std::string> {
        if (x == 0) return std::unexpected("Cannot divide by zero");
        return 100.0 / x;
    });
```

### 与 Result 的对比

| 特性 | 自定义 Result | std::expected |
|------|---------------|---------------|
| 标准 | 需自己实现 | C++23 标准库 |
| transform | 需自己实现 | 内置 |
| and_then | 需自己实现 | 内置 |
| monadic 接口 | 按需实现 | 完整支持 |
| 编译器支持 | C++17 可用 | 需要 C++23 |

## 错误类型设计

### 使用枚举

```cpp
enum class FileError {
    NotFound,
    PermissionDenied,
    InvalidFormat,
    IoError
};

Result<std::string, FileError> read_config(const std::string& path);
```

### 使用 variant 表达丰富错误

```cpp
struct NotFound { std::string path; };
struct PermissionDenied { std::string path; int required_mode; };
struct ParseError { int line; std::string message; };

using ConfigError = std::variant<NotFound, PermissionDenied, ParseError>;

Result<Config, ConfigError> load_config(const std::string& path);

// 处理错误
if (result.is_err()) {
    std::visit(overload{
        [](const NotFound& e) {
            std::cerr << "File not found: " << e.path << "\n";
        },
        [](const PermissionDenied& e) {
            std::cerr << "Permission denied: " << e.path << "\n";
        },
        [](const ParseError& e) {
            std::cerr << "Parse error at line " << e.line << ": " << e.message << "\n";
        }
    }, result.unwrap_err());
}
```

## 实战模式

### 1. 提前返回模式

```cpp
Result<User, DbError> create_user(const UserInput& input) {
    // 验证
    auto validated = validate_input(input);
    if (validated.is_err()) {
        return Result<User, DbError>::err(validated.unwrap_err());
    }

    // 检查重复
    auto existing = find_user_by_email(input.email);
    if (existing.is_ok()) {
        return Result<User, DbError>::err(DbError::DuplicateEmail);
    }

    // 创建
    return insert_user(validated.unwrap());
}
```

### 2. 收集所有错误

```cpp
struct ValidationErrors {
    std::vector<std::string> errors;
    bool has_errors() const { return !errors.empty(); }
};

Result<ValidatedForm, ValidationErrors> validate_form(const Form& form) {
    ValidationErrors errors;

    if (form.name.empty()) {
        errors.errors.push_back("Name is required");
    }
    if (form.email.find('@') == std::string::npos) {
        errors.errors.push_back("Invalid email format");
    }
    if (form.age < 0 || form.age > 150) {
        errors.errors.push_back("Invalid age");
    }

    if (errors.has_errors()) {
        return Result<ValidatedForm, ValidationErrors>::err(std::move(errors));
    }
    return Result<ValidatedForm, ValidationErrors>::ok(
        ValidatedForm{form.name, form.email, form.age}
    );
}
```

### 3. 重试模式

```cpp
template<typename F>
auto retry(F operation, int max_attempts) {
    using ResultType = std::invoke_result_t<F>;

    for (int attempt = 1; attempt <= max_attempts; ++attempt) {
        auto result = operation();
        if (result.is_ok() || attempt == max_attempts) {
            return result;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100 * attempt));
    }

    return operation();  // 最后一次尝试
}

// 使用
auto result = retry([] { return fetch_data(url); }, 3);
```

## 异常 vs Result

### 何时使用异常

- 真正的异常情况（不应该发生）
- 构造函数失败
- 与现有异常代码集成
- 错误需要跨越多层传播

### 何时使用 Result

- 预期的失败（如用户输入验证）
- 性能敏感路径
- 需要强制调用者处理错误
- 函数式组合和链式调用

### 混合使用

```cpp
// 内部使用异常，边界转换为 Result
Result<int, std::string> parse_int_safe(const std::string& s) noexcept {
    try {
        return Result<int, std::string>::ok(std::stoi(s));
    } catch (const std::exception& e) {
        return Result<int, std::string>::err(e.what());
    }
}
```

## 最佳实践

1. **明确表达可能失败**：函数签名应该清楚表明是否可能失败
2. **选择合适的错误类型**：简单场景用枚举，复杂场景用 variant
3. **优先 unwrap_or**：避免忘记处理错误
4. **使用 map/and_then 组合**：避免嵌套 if-else
5. **错误应该是值**：可以存储、传递、检查

## 代码示例

完整示例：`examples/01_error_handling.cpp`

## 练习

1. **完善 Result 类型**：添加 map_err、or_else 方法
2. **文件操作链**：实现读取、解析、验证的链式操作
3. **HTTP 响应处理**：用 Result 处理各种响应状态
4. **批量操作**：收集多个操作的错误
5. **自定义 expected**：实现简化版 std::expected

练习文件：`exercises/exercises.cpp`
参考答案：`exercises/solutions.cpp`

## 下一章

[10 - Functor 与 Monad 入门](../10-functors-monads/README.md)
