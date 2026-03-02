/**
 * @file 01_error_handling.cpp
 * @brief 错误处理示例
 *
 * 本文件演示：
 * - Result<T, E> 类型实现
 * - map 和 and_then 组合
 * - 错误类型设计
 * - 实战模式
 */

#include <cmath>
#include <functional>
#include <iostream>
#include <string>
#include <variant>
#include <vector>

// ============================================
// overload 辅助工具
// ============================================

template<class... Ts>
struct overload : Ts... {
    using Ts::operator()...;
};

template<class... Ts>
overload(Ts...) -> overload<Ts...>;

// ============================================
// 1. Result<T, E> 完整实现
// ============================================

template<typename T, typename E>
class Result {
    std::variant<T, E> data_;

    // 私有构造，强制使用工厂函数
    Result() = default;

public:
    using value_type = T;
    using error_type = E;

    // 工厂函数
    [[nodiscard]] static Result ok(T value) {
        Result r;
        r.data_.template emplace<0>(std::move(value));
        return r;
    }

    [[nodiscard]] static Result err(E error) {
        Result r;
        r.data_.template emplace<1>(std::move(error));
        return r;
    }

    // 查询状态
    [[nodiscard]] bool is_ok() const noexcept {
        return data_.index() == 0;
    }

    [[nodiscard]] bool is_err() const noexcept {
        return data_.index() == 1;
    }

    // 布尔转换
    explicit operator bool() const noexcept {
        return is_ok();
    }

    // 获取值（失败时抛异常）
    [[nodiscard]] const T& unwrap() const {
        if (is_err()) {
            throw std::runtime_error("Called unwrap() on Err value");
        }
        return std::get<0>(data_);
    }

    [[nodiscard]] T& unwrap() {
        if (is_err()) {
            throw std::runtime_error("Called unwrap() on Err value");
        }
        return std::get<0>(data_);
    }

    // 获取错误（成功时抛异常）
    [[nodiscard]] const E& unwrap_err() const {
        if (is_ok()) {
            throw std::runtime_error("Called unwrap_err() on Ok value");
        }
        return std::get<1>(data_);
    }

    // 带默认值获取
    [[nodiscard]] T unwrap_or(T default_value) const {
        return is_ok() ? std::get<0>(data_) : std::move(default_value);
    }

    // 带函数获取默认值
    template<typename F>
    [[nodiscard]] T unwrap_or_else(F f) const {
        return is_ok() ? std::get<0>(data_) : f();
    }

    // map: 转换成功值
    template<typename F>
    [[nodiscard]] auto map(F f) const {
        using NewT = std::invoke_result_t<F, const T&>;
        if (is_ok()) {
            return Result<NewT, E>::ok(f(std::get<0>(data_)));
        }
        return Result<NewT, E>::err(std::get<1>(data_));
    }

    // map_err: 转换错误值
    template<typename F>
    [[nodiscard]] auto map_err(F f) const {
        using NewE = std::invoke_result_t<F, const E&>;
        if (is_err()) {
            return Result<T, NewE>::err(f(std::get<1>(data_)));
        }
        return Result<T, NewE>::ok(std::get<0>(data_));
    }

    // and_then: 链式可能失败的操作
    template<typename F>
    [[nodiscard]] auto and_then(F f) const {
        using NewResult = std::invoke_result_t<F, const T&>;
        if (is_ok()) {
            return f(std::get<0>(data_));
        }
        return NewResult::err(std::get<1>(data_));
    }

    // or_else: 错误时尝试恢复
    template<typename F>
    [[nodiscard]] auto or_else(F f) const {
        using NewResult = std::invoke_result_t<F, const E&>;
        if (is_err()) {
            return f(std::get<1>(data_));
        }
        return NewResult::ok(std::get<0>(data_));
    }
};

// ============================================
// 2. 基本使用示例
// ============================================

enum class MathError {
    DivisionByZero,
    NegativeSqrt,
    Overflow
};

[[nodiscard]] std::string math_error_to_string(MathError e) {
    switch (e) {
        case MathError::DivisionByZero: return "除零错误";
        case MathError::NegativeSqrt: return "负数开方";
        case MathError::Overflow: return "溢出";
    }
    return "未知错误";
}

[[nodiscard]] Result<double, MathError> safe_divide(double a, double b) {
    if (b == 0.0) {
        return Result<double, MathError>::err(MathError::DivisionByZero);
    }
    return Result<double, MathError>::ok(a / b);
}

[[nodiscard]] Result<double, MathError> safe_sqrt(double x) {
    if (x < 0.0) {
        return Result<double, MathError>::err(MathError::NegativeSqrt);
    }
    return Result<double, MathError>::ok(std::sqrt(x));
}

void basic_usage_demo() {
    std::cout << "=== 基本使用 ===\n";

    // 成功案例
    auto r1 = safe_divide(10.0, 2.0);
    if (r1.is_ok()) {
        std::cout << "10 / 2 = " << r1.unwrap() << "\n";
    }

    // 失败案例
    auto r2 = safe_divide(10.0, 0.0);
    if (r2.is_err()) {
        std::cout << "10 / 0 错误: " << math_error_to_string(r2.unwrap_err()) << "\n";
    }

    // unwrap_or
    auto r3 = safe_sqrt(-4.0);
    double value = r3.unwrap_or(0.0);
    std::cout << "sqrt(-4) with default: " << value << "\n";

    std::cout << "\n";
}

// ============================================
// 3. map 和 and_then 组合
// ============================================

void composition_demo() {
    std::cout << "=== 函数式组合 ===\n";

    // map: 转换成功值
    auto result1 = safe_divide(10.0, 2.0)
        .map([](double x) { return x * 2; })     // 10.0
        .map([](double x) { return x + 1; });    // 11.0

    std::cout << "(10/2) * 2 + 1 = " << result1.unwrap() << "\n";

    // 错误会传播
    auto result2 = safe_divide(10.0, 0.0)
        .map([](double x) { return x * 2; })
        .map([](double x) { return x + 1; });

    std::cout << "(10/0) * 2 + 1: "
              << (result2.is_err() ? "错误传播" : "意外成功") << "\n";

    // and_then: 链式可能失败的操作
    // 计算 sqrt(100 / x)
    auto compute = [](double x) {
        return safe_divide(100.0, x)
            .and_then([](double y) { return safe_sqrt(y); });
    };

    auto r3 = compute(4.0);  // sqrt(100/4) = sqrt(25) = 5
    std::cout << "sqrt(100/4) = " << r3.unwrap() << "\n";

    auto r4 = compute(0.0);  // 除零错误
    std::cout << "sqrt(100/0): " << math_error_to_string(r4.unwrap_err()) << "\n";

    auto r5 = compute(-4.0);  // sqrt(-25) 负数开方
    std::cout << "sqrt(100/-4): " << math_error_to_string(r5.unwrap_err()) << "\n";

    std::cout << "\n";
}

// ============================================
// 4. 丰富的错误类型
// ============================================

// 使用 variant 表达不同错误
struct NotFound {
    std::string resource;
};

struct PermissionDenied {
    std::string path;
    int required_permission;
};

struct ParseError {
    int line;
    int column;
    std::string message;
};

using FileError = std::variant<NotFound, PermissionDenied, ParseError>;

[[nodiscard]] std::string file_error_to_string(const FileError& e) {
    return std::visit(overload{
        [](const NotFound& nf) {
            return "未找到: " + nf.resource;
        },
        [](const PermissionDenied& pd) {
            return "权限不足: " + pd.path + " (需要权限 " +
                   std::to_string(pd.required_permission) + ")";
        },
        [](const ParseError& pe) {
            return "解析错误 [" + std::to_string(pe.line) + ":" +
                   std::to_string(pe.column) + "]: " + pe.message;
        }
    }, e);
}

// 模拟文件操作
[[nodiscard]] Result<std::string, FileError> read_file(const std::string& path) {
    if (path == "not_found.txt") {
        return Result<std::string, FileError>::err(NotFound{path});
    }
    if (path == "secret.txt") {
        return Result<std::string, FileError>::err(
            PermissionDenied{path, 0644}
        );
    }
    return Result<std::string, FileError>::ok("文件内容: " + path);
}

[[nodiscard]] Result<int, FileError> parse_config(const std::string& content) {
    if (content.find("error") != std::string::npos) {
        return Result<int, FileError>::err(
            ParseError{10, 5, "unexpected token"}
        );
    }
    return Result<int, FileError>::ok(42);
}

void rich_error_demo() {
    std::cout << "=== 丰富错误类型 ===\n";

    std::vector<std::string> files = {
        "config.txt",
        "not_found.txt",
        "secret.txt"
    };

    for (const auto& file : files) {
        auto result = read_file(file);
        if (result.is_ok()) {
            std::cout << "  读取成功: " << result.unwrap() << "\n";
        } else {
            std::cout << "  读取失败: " << file_error_to_string(result.unwrap_err()) << "\n";
        }
    }

    // 链式操作
    auto process = [](const std::string& path) {
        return read_file(path)
            .and_then([](const std::string& content) {
                return parse_config(content);
            });
    };

    auto r = process("config.txt");
    std::cout << "处理 config.txt: " << r.unwrap() << "\n";

    std::cout << "\n";
}

// ============================================
// 5. map_err 转换错误
// ============================================

void map_err_demo() {
    std::cout << "=== 错误转换 ===\n";

    // 将 MathError 转换为 string
    auto result = safe_divide(10.0, 0.0)
        .map_err([](MathError e) {
            return std::string("数学错误: ") + math_error_to_string(e);
        });

    if (result.is_err()) {
        std::cout << result.unwrap_err() << "\n";
    }

    std::cout << "\n";
}

// ============================================
// 6. or_else 错误恢复
// ============================================

[[nodiscard]] Result<double, MathError> get_from_cache(int key) {
    // 模拟缓存未命中
    if (key != 42) {
        return Result<double, MathError>::err(MathError::Overflow);
    }
    return Result<double, MathError>::ok(100.0);
}

[[nodiscard]] Result<double, MathError> compute_fresh(int key) {
    return Result<double, MathError>::ok(static_cast<double>(key * 2));
}

void or_else_demo() {
    std::cout << "=== 错误恢复 ===\n";

    // 缓存命中
    auto r1 = get_from_cache(42)
        .or_else([](MathError) { return compute_fresh(42); });
    std::cout << "Key 42 (cached): " << r1.unwrap() << "\n";

    // 缓存未命中，回退计算
    auto r2 = get_from_cache(10)
        .or_else([](MathError) { return compute_fresh(10); });
    std::cout << "Key 10 (computed): " << r2.unwrap() << "\n";

    std::cout << "\n";
}

// ============================================
// 7. 收集多个错误
// ============================================

struct ValidationResult {
    bool valid = true;
    std::vector<std::string> errors;

    void add_error(const std::string& msg) {
        valid = false;
        errors.push_back(msg);
    }
};

struct UserInput {
    std::string name;
    std::string email;
    int age;
};

[[nodiscard]] Result<UserInput, ValidationResult> validate_user(const UserInput& input) {
    ValidationResult result;

    if (input.name.empty()) {
        result.add_error("姓名不能为空");
    } else if (input.name.length() < 2) {
        result.add_error("姓名至少2个字符");
    }

    if (input.email.find('@') == std::string::npos) {
        result.add_error("邮箱格式无效");
    }

    if (input.age < 0 || input.age > 150) {
        result.add_error("年龄无效");
    }

    if (!result.valid) {
        return Result<UserInput, ValidationResult>::err(std::move(result));
    }
    return Result<UserInput, ValidationResult>::ok(input);
}

void validation_demo() {
    std::cout << "=== 多错误收集 ===\n";

    UserInput bad_input{"", "invalid-email", -5};
    auto result = validate_user(bad_input);

    if (result.is_err()) {
        std::cout << "验证失败:\n";
        for (const auto& err : result.unwrap_err().errors) {
            std::cout << "  - " << err << "\n";
        }
    }

    UserInput good_input{"Alice", "alice@example.com", 25};
    auto result2 = validate_user(good_input);
    if (result2.is_ok()) {
        std::cout << "验证通过: " << result2.unwrap().name << "\n";
    }

    std::cout << "\n";
}

// ============================================
// 8. 批量操作
// ============================================

template<typename T, typename E>
[[nodiscard]] Result<std::vector<T>, E> collect_results(
    const std::vector<Result<T, E>>& results
) {
    std::vector<T> values;
    values.reserve(results.size());

    for (const auto& r : results) {
        if (r.is_err()) {
            return Result<std::vector<T>, E>::err(r.unwrap_err());
        }
        values.push_back(r.unwrap());
    }

    return Result<std::vector<T>, E>::ok(std::move(values));
}

void batch_demo() {
    std::cout << "=== 批量操作 ===\n";

    // 全部成功
    std::vector<Result<double, MathError>> results1 = {
        safe_divide(10.0, 2.0),
        safe_divide(20.0, 4.0),
        safe_divide(15.0, 3.0)
    };

    auto collected1 = collect_results(results1);
    if (collected1.is_ok()) {
        std::cout << "全部成功: ";
        for (double v : collected1.unwrap()) {
            std::cout << v << " ";
        }
        std::cout << "\n";
    }

    // 包含失败
    std::vector<Result<double, MathError>> results2 = {
        safe_divide(10.0, 2.0),
        safe_divide(20.0, 0.0),  // 失败
        safe_divide(15.0, 3.0)
    };

    auto collected2 = collect_results(results2);
    if (collected2.is_err()) {
        std::cout << "批量失败: " << math_error_to_string(collected2.unwrap_err()) << "\n";
    }

    std::cout << "\n";
}

// ============================================
// 9. 实际应用：解析整数
// ============================================

[[nodiscard]] Result<int, std::string> parse_int(const std::string& s) {
    if (s.empty()) {
        return Result<int, std::string>::err("空字符串");
    }

    try {
        size_t pos;
        int value = std::stoi(s, &pos);
        if (pos != s.length()) {
            return Result<int, std::string>::err("包含非数字字符");
        }
        return Result<int, std::string>::ok(value);
    } catch (const std::out_of_range&) {
        return Result<int, std::string>::err("数值超出范围");
    } catch (const std::invalid_argument&) {
        return Result<int, std::string>::err("无效的数字格式");
    }
}

void parse_demo() {
    std::cout << "=== 解析示例 ===\n";

    std::vector<std::string> inputs = {"42", "-10", "abc", "12abc", "", "99999999999999"};

    for (const auto& input : inputs) {
        auto result = parse_int(input);
        std::cout << "  parse(\"" << input << "\"): ";
        if (result.is_ok()) {
            std::cout << result.unwrap() << "\n";
        } else {
            std::cout << "错误 - " << result.unwrap_err() << "\n";
        }
    }

    std::cout << "\n";
}

// ============================================
// main
// ============================================

int main() {
    basic_usage_demo();
    composition_demo();
    rich_error_demo();
    map_err_demo();
    or_else_demo();
    validation_demo();
    batch_demo();
    parse_demo();

    return 0;
}
