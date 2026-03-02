/**
 * @file solutions.cpp
 * @brief 第08章练习题参考答案
 */

#include <cassert>
#include <cmath>
#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <variant>
#include <vector>

// overload 辅助工具
template<class... Ts>
struct overload : Ts... { using Ts::operator()...; };

template<class... Ts>
overload(Ts...) -> overload<Ts...>;

// ============================================
// 练习 1: Result 类型 - 答案
// ============================================

template<typename T, typename E>
class Result {
public:
    std::variant<T, E> data;

    [[nodiscard]] bool is_ok() const {
        return std::holds_alternative<T>(data);
    }

    [[nodiscard]] bool is_err() const {
        return std::holds_alternative<E>(data);
    }

    [[nodiscard]] const T& unwrap() const {
        if (is_err()) {
            throw std::runtime_error("Attempted to unwrap an Err value");
        }
        return std::get<T>(data);
    }

    [[nodiscard]] const E& unwrap_err() const {
        if (is_ok()) {
            throw std::runtime_error("Attempted to unwrap_err an Ok value");
        }
        return std::get<E>(data);
    }

    template<typename F>
    [[nodiscard]] auto map(F f) const {
        using NewT = decltype(f(std::declval<T>()));
        if (is_ok()) {
            return Result<NewT, E>::ok(f(std::get<T>(data)));
        }
        return Result<NewT, E>::err(std::get<E>(data));
    }

    // and_then：链式操作
    template<typename F>
    [[nodiscard]] auto and_then(F f) const {
        if (is_ok()) {
            return f(std::get<T>(data));
        }
        using ReturnType = decltype(f(std::declval<T>()));
        return ReturnType::err(std::get<E>(data));
    }

    static Result ok(T value) {
        return Result{std::variant<T, E>(std::in_place_index<0>, std::move(value))};
    }

    static Result err(E error) {
        return Result{std::variant<T, E>(std::in_place_index<1>, std::move(error))};
    }
};

[[nodiscard]] Result<double, std::string> safe_divide(double a, double b) {
    if (b == 0.0) {
        return Result<double, std::string>::err("除以零");
    }
    return Result<double, std::string>::ok(a / b);
}

[[nodiscard]] Result<double, std::string> safe_sqrt(double x) {
    if (x < 0) {
        return Result<double, std::string>::err("负数的平方根");
    }
    return Result<double, std::string>::ok(std::sqrt(x));
}

void test_exercise_1() {
    std::cout << "=== 练习 1: Result 类型 - 答案 ===\n";

    auto r1 = safe_divide(10.0, 2.0);
    assert(r1.is_ok());
    assert(r1.unwrap() == 5.0);
    std::cout << "10 / 2 = " << r1.unwrap() << "\n";

    auto r2 = safe_divide(10.0, 0.0);
    assert(r2.is_err());
    std::cout << "10 / 0 错误: " << r2.unwrap_err() << "\n";

    auto r3 = safe_sqrt(16.0);
    assert(r3.is_ok());
    assert(r3.unwrap() == 4.0);
    std::cout << "sqrt(16) = " << r3.unwrap() << "\n";

    auto r4 = safe_sqrt(-4.0);
    assert(r4.is_err());
    std::cout << "sqrt(-4) 错误: " << r4.unwrap_err() << "\n";

    // map 测试
    auto r5 = safe_divide(10.0, 2.0).map([](double x) { return x * 2; });
    assert(r5.is_ok());
    assert(r5.unwrap() == 10.0);
    std::cout << "(10 / 2) * 2 = " << r5.unwrap() << "\n";

    // and_then 测试
    auto r6 = safe_divide(16.0, 4.0).and_then([](double x) {
        return safe_sqrt(x);
    });
    assert(r6.is_ok());
    std::cout << "sqrt(16 / 4) = " << r6.unwrap() << "\n";

    std::cout << "练习 1 通过\n\n";
}

// ============================================
// 练习 2: 连接状态机 - 答案
// ============================================

struct Disconnected {};
struct Connecting { std::string host; };
struct Connected { int session_id; };
struct ConnectionFailed { std::string error; };

using ConnectionState = std::variant<Disconnected, Connecting, Connected, ConnectionFailed>;

[[nodiscard]] ConnectionState connect(const ConnectionState& state, const std::string& host) {
    return std::visit(overload{
        [&host](const Disconnected&) -> ConnectionState { return Connecting{host}; },
        [](const auto& s) -> ConnectionState { return s; }
    }, state);
}

[[nodiscard]] ConnectionState establish(const ConnectionState& state, int session_id) {
    return std::visit(overload{
        [session_id](const Connecting&) -> ConnectionState { return Connected{session_id}; },
        [](const auto& s) -> ConnectionState { return s; }
    }, state);
}

[[nodiscard]] ConnectionState disconnect(const ConnectionState& state) {
    return std::visit(overload{
        [](const Connected&) -> ConnectionState { return Disconnected{}; },
        [](const auto& s) -> ConnectionState { return s; }
    }, state);
}

[[nodiscard]] ConnectionState fail(const ConnectionState& state, const std::string& error) {
    return std::visit(overload{
        [&error](const Disconnected&) -> ConnectionState { return ConnectionFailed{error}; },
        [&error](const Connecting&) -> ConnectionState { return ConnectionFailed{error}; },
        [&error](const Connected&) -> ConnectionState { return ConnectionFailed{error}; },
        [](const ConnectionFailed& f) -> ConnectionState { return f; }
    }, state);
}

[[nodiscard]] std::string describe_state(const ConnectionState& state) {
    return std::visit(overload{
        [](const Disconnected&) { return std::string("未连接"); },
        [](const Connecting& c) { return "连接中: " + c.host; },
        [](const Connected& c) { return "已连接 (会话: " + std::to_string(c.session_id) + ")"; },
        [](const ConnectionFailed& f) { return "失败: " + f.error; }
    }, state);
}

void test_exercise_2() {
    std::cout << "=== 练习 2: 连接状态机 - 答案 ===\n";

    ConnectionState state = Disconnected{};
    std::cout << "初始: " << describe_state(state) << "\n";
    assert(describe_state(state) == "未连接");

    state = connect(state, "example.com");
    std::cout << "连接后: " << describe_state(state) << "\n";
    assert(describe_state(state) == "连接中: example.com");

    state = establish(state, 12345);
    std::cout << "建立后: " << describe_state(state) << "\n";
    assert(describe_state(state) == "已连接 (会话: 12345)");

    state = disconnect(state);
    std::cout << "断开后: " << describe_state(state) << "\n";
    assert(describe_state(state) == "未连接");

    // 测试失败状态
    state = connect(state, "bad-host.com");
    state = fail(state, "连接超时");
    std::cout << "失败后: " << describe_state(state) << "\n";

    std::cout << "练习 2 通过\n\n";
}

// ============================================
// 练习 3: 表达式树 - 答案
// ============================================

struct Expr;
using ExprPtr = std::shared_ptr<Expr>;

struct Literal { double value; };
struct Add { ExprPtr left, right; };
struct Mul { ExprPtr left, right; };
struct Neg { ExprPtr operand; };

struct Expr {
    std::variant<Literal, Add, Mul, Neg> data;

    explicit Expr(Literal lit) : data(lit) {}
    explicit Expr(Add add) : data(add) {}
    explicit Expr(Mul mul) : data(mul) {}
    explicit Expr(Neg neg) : data(neg) {}
};

[[nodiscard]] ExprPtr lit(double value) {
    return std::make_shared<Expr>(Literal{value});
}

[[nodiscard]] ExprPtr add(ExprPtr left, ExprPtr right) {
    return std::make_shared<Expr>(Add{left, right});
}

[[nodiscard]] ExprPtr mul(ExprPtr left, ExprPtr right) {
    return std::make_shared<Expr>(Mul{left, right});
}

[[nodiscard]] ExprPtr neg(ExprPtr operand) {
    return std::make_shared<Expr>(Neg{operand});
}

double evaluate(const Expr& expr);

double evaluate(const Expr& expr) {
    return std::visit(overload{
        [](const Literal& l) { return l.value; },
        [](const Add& a) { return evaluate(*a.left) + evaluate(*a.right); },
        [](const Mul& m) { return evaluate(*m.left) * evaluate(*m.right); },
        [](const Neg& n) { return -evaluate(*n.operand); }
    }, expr.data);
}

std::string expr_to_string(const Expr& expr);

std::string expr_to_string(const Expr& expr) {
    return std::visit(overload{
        [](const Literal& l) { return std::to_string(l.value); },
        [](const Add& a) {
            return "(" + expr_to_string(*a.left) + " + " + expr_to_string(*a.right) + ")";
        },
        [](const Mul& m) {
            return "(" + expr_to_string(*m.left) + " * " + expr_to_string(*m.right) + ")";
        },
        [](const Neg& n) {
            return "(-" + expr_to_string(*n.operand) + ")";
        }
    }, expr.data);
}

void test_exercise_3() {
    std::cout << "=== 练习 3: 表达式树 - 答案 ===\n";

    // (2 + 3) * 4 = 20
    auto expr1 = mul(add(lit(2), lit(3)), lit(4));
    std::cout << "表达式: " << expr_to_string(*expr1) << "\n";
    std::cout << "结果: " << evaluate(*expr1) << "\n";
    assert(evaluate(*expr1) == 20.0);

    // -(5 + 3) = -8
    auto expr2 = neg(add(lit(5), lit(3)));
    std::cout << "表达式: " << expr_to_string(*expr2) << "\n";
    std::cout << "结果: " << evaluate(*expr2) << "\n";
    assert(evaluate(*expr2) == -8.0);

    // 更复杂的表达式：(3 * -2) + 10 = 4
    auto expr3 = add(mul(lit(3), neg(lit(2))), lit(10));
    std::cout << "表达式: " << expr_to_string(*expr3) << "\n";
    std::cout << "结果: " << evaluate(*expr3) << "\n";
    assert(evaluate(*expr3) == 4.0);

    std::cout << "练习 3 通过\n\n";
}

// ============================================
// 练习 4: Maybe 类型 - 答案
// ============================================

template<typename T>
class Maybe {
public:
    std::variant<std::monostate, T> data;

    [[nodiscard]] bool has_value() const {
        return std::holds_alternative<T>(data);
    }

    [[nodiscard]] const T& value() const {
        if (!has_value()) {
            throw std::runtime_error("Maybe has no value");
        }
        return std::get<T>(data);
    }

    [[nodiscard]] T value_or(const T& default_value) const {
        if (has_value()) {
            return std::get<T>(data);
        }
        return default_value;
    }

    template<typename F>
    [[nodiscard]] auto map(F f) const {
        using NewT = decltype(f(std::declval<T>()));
        if (has_value()) {
            return Maybe<NewT>::just(f(std::get<T>(data)));
        }
        return Maybe<NewT>::nothing();
    }

    // and_then：链式操作
    template<typename F>
    [[nodiscard]] auto and_then(F f) const {
        if (has_value()) {
            return f(std::get<T>(data));
        }
        using ReturnType = decltype(f(std::declval<T>()));
        return ReturnType::nothing();
    }

    static Maybe just(T value) {
        Maybe m;
        m.data = std::move(value);
        return m;
    }

    static Maybe nothing() {
        return Maybe{};
    }
};

void test_exercise_4() {
    std::cout << "=== 练习 4: Maybe 类型 - 答案 ===\n";

    auto m1 = Maybe<int>::just(42);
    assert(m1.has_value());
    assert(m1.value() == 42);
    std::cout << "just(42).value() = " << m1.value() << "\n";

    auto m2 = Maybe<int>::nothing();
    assert(!m2.has_value());
    assert(m2.value_or(99) == 99);
    std::cout << "nothing().value_or(99) = " << m2.value_or(99) << "\n";

    auto m3 = m1.map([](int x) { return x * 2; });
    assert(m3.has_value());
    assert(m3.value() == 84);
    std::cout << "just(42).map(*2).value() = " << m3.value() << "\n";

    auto m4 = m2.map([](int x) { return x * 2; });
    assert(!m4.has_value());
    std::cout << "nothing().map(*2).has_value() = " << m4.has_value() << "\n";

    // and_then 测试
    auto m5 = Maybe<int>::just(16).and_then([](int x) {
        if (x >= 0) return Maybe<double>::just(std::sqrt(x));
        return Maybe<double>::nothing();
    });
    assert(m5.has_value());
    std::cout << "just(16).and_then(sqrt).value() = " << m5.value() << "\n";

    std::cout << "练习 4 通过\n\n";
}

// ============================================
// 练习 5: 配置值类型 - 答案
// ============================================

class ConfigValue {
public:
    std::variant<int, double, std::string, bool> value;

    ConfigValue(int v) : value(v) {}
    ConfigValue(double v) : value(v) {}
    ConfigValue(const std::string& v) : value(v) {}
    ConfigValue(const char* v) : value(std::string(v)) {}
    ConfigValue(bool v) : value(v) {}

    [[nodiscard]] std::optional<int> as_int() const {
        if (std::holds_alternative<int>(value)) {
            return std::get<int>(value);
        }
        return std::nullopt;
    }

    [[nodiscard]] std::optional<double> as_double() const {
        if (std::holds_alternative<double>(value)) {
            return std::get<double>(value);
        }
        // 也可以从 int 转换
        if (std::holds_alternative<int>(value)) {
            return static_cast<double>(std::get<int>(value));
        }
        return std::nullopt;
    }

    [[nodiscard]] std::optional<std::string> as_string() const {
        if (std::holds_alternative<std::string>(value)) {
            return std::get<std::string>(value);
        }
        return std::nullopt;
    }

    [[nodiscard]] std::optional<bool> as_bool() const {
        if (std::holds_alternative<bool>(value)) {
            return std::get<bool>(value);
        }
        return std::nullopt;
    }

    [[nodiscard]] std::string to_string() const {
        return std::visit(overload{
            [](int i) { return std::to_string(i); },
            [](double d) { return std::to_string(d); },
            [](const std::string& s) { return s; },
            [](bool b) { return std::string(b ? "true" : "false"); }
        }, value);
    }

    // 获取类型名
    [[nodiscard]] std::string type_name() const {
        return std::visit(overload{
            [](int) { return std::string("int"); },
            [](double) { return std::string("double"); },
            [](const std::string&) { return std::string("string"); },
            [](bool) { return std::string("bool"); }
        }, value);
    }
};

void test_exercise_5() {
    std::cout << "=== 练习 5: 配置值类型 - 答案 ===\n";

    ConfigValue v1(42);
    assert(v1.as_int().value() == 42);
    assert(!v1.as_string().has_value());
    std::cout << "int value: " << v1.to_string() << " (type: " << v1.type_name() << ")\n";

    ConfigValue v2(3.14);
    assert(v2.as_double().value() == 3.14);
    std::cout << "double value: " << v2.to_string() << " (type: " << v2.type_name() << ")\n";

    ConfigValue v3("hello");
    assert(v3.as_string().value() == "hello");
    std::cout << "string value: " << v3.to_string() << " (type: " << v3.type_name() << ")\n";

    ConfigValue v4(true);
    assert(v4.as_bool().value() == true);
    std::cout << "bool value: " << v4.to_string() << " (type: " << v4.type_name() << ")\n";

    // int 也可以作为 double 读取
    assert(v1.as_double().value() == 42.0);
    std::cout << "int as double: " << v1.as_double().value() << "\n";

    std::cout << "练习 5 通过\n\n";
}

// ============================================
// 额外示例：JSON-like 值类型
// ============================================

struct JsonValue;
using JsonArray = std::vector<JsonValue>;
using JsonObject = std::vector<std::pair<std::string, JsonValue>>;

struct JsonNull {};

struct JsonValue {
    std::variant<JsonNull, bool, double, std::string, JsonArray, JsonObject> data;

    JsonValue() : data(JsonNull{}) {}
    JsonValue(std::nullptr_t) : data(JsonNull{}) {}
    JsonValue(bool b) : data(b) {}
    JsonValue(double d) : data(d) {}
    JsonValue(int i) : data(static_cast<double>(i)) {}
    JsonValue(const std::string& s) : data(s) {}
    JsonValue(const char* s) : data(std::string(s)) {}
    JsonValue(JsonArray arr) : data(std::move(arr)) {}
    JsonValue(JsonObject obj) : data(std::move(obj)) {}
};

std::string json_to_string(const JsonValue& v);

std::string json_to_string(const JsonValue& v) {
    return std::visit(overload{
        [](const JsonNull&) { return std::string("null"); },
        [](bool b) { return std::string(b ? "true" : "false"); },
        [](double d) { return std::to_string(d); },
        [](const std::string& s) { return "\"" + s + "\""; },
        [](const JsonArray& arr) {
            std::string result = "[";
            for (size_t i = 0; i < arr.size(); ++i) {
                if (i > 0) result += ", ";
                result += json_to_string(arr[i]);
            }
            return result + "]";
        },
        [](const JsonObject& obj) {
            std::string result = "{";
            for (size_t i = 0; i < obj.size(); ++i) {
                if (i > 0) result += ", ";
                result += "\"" + obj[i].first + "\": " + json_to_string(obj[i].second);
            }
            return result + "}";
        }
    }, v.data);
}

void bonus_json_demo() {
    std::cout << "=== 额外示例：JSON 值类型 ===\n";

    JsonValue config = JsonObject{
        {"name", "MyApp"},
        {"version", 1.0},
        {"debug", true},
        {"ports", JsonArray{8080, 8443}},
        {"database", JsonObject{
            {"host", "localhost"},
            {"port", 5432}
        }}
    };

    std::cout << "Config: " << json_to_string(config) << "\n\n";
}

// ============================================
// main
// ============================================

int main() {
    std::cout << "========================================\n";
    std::cout << "    第08章练习参考答案\n";
    std::cout << "========================================\n\n";

    test_exercise_1();
    test_exercise_2();
    test_exercise_3();
    test_exercise_4();
    test_exercise_5();
    bonus_json_demo();

    std::cout << "========================================\n";
    std::cout << "    所有练习完成!\n";
    std::cout << "========================================\n";

    return 0;
}
