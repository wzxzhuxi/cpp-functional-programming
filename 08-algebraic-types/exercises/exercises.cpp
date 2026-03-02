/**
 * @file exercises.cpp
 * @brief 第08章练习题：代数数据类型
 *
 * 完成下面的练习，然后运行程序验证结果。
 * 参考答案在 solutions.cpp
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
// 练习 1: 实现 Result<T, E> 类型
// ============================================

/**
 * Result 类型用于表示可能失败的操作结果
 *
 * 要求实现：
 * - is_ok(): 检查是否成功
 * - is_err(): 检查是否失败
 * - unwrap(): 获取成功值（失败时抛异常）
 * - unwrap_err(): 获取错误值
 * - map(): 转换成功值
 * - ok() / err(): 静态工厂函数
 */
template<typename T, typename E>
class Result {
public:
    std::variant<T, E> data;

    // TODO: 实现 is_ok
    [[nodiscard]] bool is_ok() const {
        return false;  // 修改这行
    }

    // TODO: 实现 is_err
    [[nodiscard]] bool is_err() const {
        return true;  // 修改这行
    }

    // TODO: 实现 unwrap
    [[nodiscard]] const T& unwrap() const {
        throw std::runtime_error("TODO");  // 修改这行
    }

    // TODO: 实现 unwrap_err
    [[nodiscard]] const E& unwrap_err() const {
        throw std::runtime_error("TODO");  // 修改这行
    }

    // TODO: 实现 map
    template<typename F>
    [[nodiscard]] auto map(F f) const {
        using NewT = decltype(f(std::declval<T>()));
        // 如果是 Ok，应用 f；如果是 Err，保持原样
        return Result<NewT, E>::err(E{});  // 修改这行
    }

    // 静态工厂（已实现）
    static Result ok(T value) {
        return Result{std::variant<T, E>(std::in_place_index<0>, std::move(value))};
    }

    static Result err(E error) {
        return Result{std::variant<T, E>(std::in_place_index<1>, std::move(error))};
    }
};

// TODO: 实现安全除法（返回 double，b==0 时返回错误）
[[nodiscard]] Result<double, std::string> safe_divide(double a, double b) {
    // 修改这里
    return Result<double, std::string>::err("Not implemented");
}

// TODO: 实现安全平方根（x<0 时返回错误）
[[nodiscard]] Result<double, std::string> safe_sqrt(double x) {
    // 修改这里
    return Result<double, std::string>::err("Not implemented");
}

void test_exercise_1() {
    std::cout << "=== 练习 1: Result 类型 ===\n";

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

    std::cout << "练习 1 通过\n\n";
}

// ============================================
// 练习 2: 连接状态机
// ============================================

/**
 * 实现一个网络连接状态机
 *
 * 状态：
 * - Disconnected: 未连接
 * - Connecting: 连接中（包含主机名）
 * - Connected: 已连接（包含会话ID）
 * - Failed: 失败（包含错误信息）
 *
 * 转换：
 * - connect(Disconnected, host) -> Connecting{host}
 * - establish(Connecting, session_id) -> Connected{session_id}
 * - disconnect(Connected, reason) -> Disconnected
 * - fail(任何状态, error) -> Failed{error}
 */

struct Disconnected {};
struct Connecting { std::string host; };
struct Connected { int session_id; };
struct ConnectionFailed { std::string error; };

using ConnectionState = std::variant<Disconnected, Connecting, Connected, ConnectionFailed>;

// TODO: 实现状态转换函数
[[nodiscard]] ConnectionState connect(const ConnectionState& state, const std::string& host) {
    // Disconnected -> Connecting{host}
    // 其他状态保持不变
    (void)host;
    return state;  // 修改这行
}

[[nodiscard]] ConnectionState establish(const ConnectionState& state, int session_id) {
    // Connecting -> Connected{session_id}
    // 其他状态保持不变
    (void)session_id;
    return state;  // 修改这行
}

[[nodiscard]] ConnectionState disconnect(const ConnectionState& state) {
    // Connected -> Disconnected
    // 其他状态保持不变
    return state;  // 修改这行
}

// TODO: 实现状态描述函数
[[nodiscard]] std::string describe_state(const ConnectionState& state) {
    // 使用 visit 返回状态描述
    (void)state;
    return "Unknown";  // 修改这行
}

void test_exercise_2() {
    std::cout << "=== 练习 2: 连接状态机 ===\n";

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

    std::cout << "练习 2 通过\n\n";
}

// ============================================
// 练习 3: 表达式树
// ============================================

struct Expr;
using ExprPtr = std::shared_ptr<Expr>;

struct Literal { double value; };
struct Add { ExprPtr left, right; };
struct Mul { ExprPtr left, right; };
struct Neg { ExprPtr operand; };  // 一元取负

struct Expr {
    std::variant<Literal, Add, Mul, Neg> data;

    explicit Expr(Literal lit) : data(lit) {}
    explicit Expr(Add add) : data(add) {}
    explicit Expr(Mul mul) : data(mul) {}
    explicit Expr(Neg neg) : data(neg) {}
};

// 构建辅助函数（已实现）
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

// TODO: 实现求值函数
[[nodiscard]] double evaluate(const Expr& expr) {
    // 使用 visit 递归求值
    (void)expr;
    return 0.0;  // 修改这行
}

// TODO: 实现表达式转字符串
[[nodiscard]] std::string expr_to_string(const Expr& expr) {
    // 格式：(left op right) 或 (-operand)
    (void)expr;
    return "";  // 修改这行
}

void test_exercise_3() {
    std::cout << "=== 练习 3: 表达式树 ===\n";

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

    std::cout << "练习 3 通过\n\n";
}

// ============================================
// 练习 4: Maybe 类型
// ============================================

/**
 * 使用 variant 实现 Maybe<T>（类似 optional）
 *
 * 要求：
 * - has_value(): 检查是否有值
 * - value(): 获取值
 * - value_or(): 获取值或默认值
 * - map(): 转换值
 */
template<typename T>
class Maybe {
public:
    std::variant<std::monostate, T> data;

    // TODO: 实现 has_value
    [[nodiscard]] bool has_value() const {
        return false;  // 修改这行
    }

    // TODO: 实现 value
    [[nodiscard]] const T& value() const {
        throw std::runtime_error("TODO");  // 修改这行
    }

    // TODO: 实现 value_or
    [[nodiscard]] T value_or(const T& default_value) const {
        (void)default_value;
        return T{};  // 修改这行
    }

    // TODO: 实现 map
    template<typename F>
    [[nodiscard]] auto map(F f) const {
        using NewT = decltype(f(std::declval<T>()));
        (void)f;
        return Maybe<NewT>::nothing();  // 修改这行
    }

    // 静态工厂（已实现）
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
    std::cout << "=== 练习 4: Maybe 类型 ===\n";

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

    std::cout << "练习 4 通过\n\n";
}

// ============================================
// 练习 5: 配置值类型
// ============================================

/**
 * 实现支持多种类型的配置值
 *
 * 要求：
 * - as_int/as_double/as_string/as_bool: 类型安全获取
 * - to_string: 转为字符串表示
 */
class ConfigValue {
public:
    std::variant<int, double, std::string, bool> value;

    ConfigValue(int v) : value(v) {}
    ConfigValue(double v) : value(v) {}
    ConfigValue(const std::string& v) : value(v) {}
    ConfigValue(const char* v) : value(std::string(v)) {}
    ConfigValue(bool v) : value(v) {}

    // TODO: 实现 as_int
    [[nodiscard]] std::optional<int> as_int() const {
        return std::nullopt;  // 修改这行
    }

    // TODO: 实现 as_double
    [[nodiscard]] std::optional<double> as_double() const {
        return std::nullopt;  // 修改这行
    }

    // TODO: 实现 as_string
    [[nodiscard]] std::optional<std::string> as_string() const {
        return std::nullopt;  // 修改这行
    }

    // TODO: 实现 as_bool
    [[nodiscard]] std::optional<bool> as_bool() const {
        return std::nullopt;  // 修改这行
    }

    // TODO: 实现 to_string
    [[nodiscard]] std::string to_string() const {
        return "";  // 修改这行
    }
};

void test_exercise_5() {
    std::cout << "=== 练习 5: 配置值类型 ===\n";

    ConfigValue v1(42);
    assert(v1.as_int().value() == 42);
    assert(!v1.as_double().has_value());
    std::cout << "int value: " << v1.to_string() << "\n";

    ConfigValue v2(3.14);
    assert(v2.as_double().value() == 3.14);
    std::cout << "double value: " << v2.to_string() << "\n";

    ConfigValue v3("hello");
    assert(v3.as_string().value() == "hello");
    std::cout << "string value: " << v3.to_string() << "\n";

    ConfigValue v4(true);
    assert(v4.as_bool().value() == true);
    std::cout << "bool value: " << v4.to_string() << "\n";

    std::cout << "练习 5 通过\n\n";
}

// ============================================
// main
// ============================================

int main() {
    std::cout << "========================================\n";
    std::cout << "    第08章练习：代数数据类型\n";
    std::cout << "========================================\n\n";

    test_exercise_1();
    test_exercise_2();
    test_exercise_3();
    test_exercise_4();
    test_exercise_5();

    std::cout << "========================================\n";
    std::cout << "    所有练习完成!\n";
    std::cout << "========================================\n";

    return 0;
}
