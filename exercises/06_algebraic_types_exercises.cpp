#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <variant>
#include <vector>

// ============================================
// Overload pattern helper
// ============================================

template<class... Ts>
struct overload : Ts... { using Ts::operator()...; };

template<class... Ts>
overload(Ts...) -> overload<Ts...>;

// ============================================
// 练习 1: Result 类型实现
// ============================================

namespace exercise1 {
    // TODO: 实现完整的 Result<T, E> 类型
    template<typename T, typename E>
    class Result {
    public:
        std::variant<T, E> data;

        // TODO: 实现 is_ok
        bool is_ok() const {
            // 你的代码
            return false;
        }

        // TODO: 实现 is_err
        bool is_err() const {
            // 你的代码
            return true;
        }

        // TODO: 实现 unwrap (获取成功值)
        const T& unwrap() const {
            // 你的代码
            throw std::runtime_error("Not implemented");
        }

        // TODO: 实现 unwrap_err (获取错误值)
        const E& unwrap_err() const {
            // 你的代码
            throw std::runtime_error("Not implemented");
        }

        // TODO: 实现 map (转换成功值)
        template<typename F>
        auto map(F f) const {
            // 你的代码：如果是 Ok，应用 f；如果是 Err，保持原样
            using NewT = decltype(f(std::declval<T>()));
            (void)f;
            return Result<NewT, E>{};
        }

        // 静态工厂函数
        static Result ok(T value) {
            return Result{std::variant<T, E>(std::in_place_index<0>, value)};
        }

        static Result err(E error) {
            return Result{std::variant<T, E>(std::in_place_index<1>, error)};
        }
    };

    // TODO: 实现安全除法
    Result<int, std::string> safe_divide(int a, int b) {
        // 你的代码：b == 0 时返回错误，否则返回 a / b
        (void)a; (void)b;
        return Result<int, std::string>::err("Not implemented");
    }

    // TODO: 实现安全平方根
    Result<double, std::string> safe_sqrt(double x) {
        // 你的代码：x < 0 时返回错误，否则返回 sqrt(x)
        (void)x;
        return Result<double, std::string>::err("Not implemented");
    }
}

// ============================================
// 练习 2: 状态机实现
// ============================================

namespace exercise2 {
    // 定义状态
    struct Idle {};
    struct Connecting { std::string host; };
    struct Connected { int session_id; };
    struct Disconnected { std::string reason; };

    using ConnectionState = std::variant<Idle, Connecting, Connected, Disconnected>;

    // TODO: 实现状态转换函数
    ConnectionState connect(const ConnectionState& state, const std::string& host) {
        // 你的代码：
        // - 如果是 Idle，转换到 Connecting{host}
        // - 其他状态，保持不变
        (void)state; (void)host;
        return Idle{};
    }

    ConnectionState establish(const ConnectionState& state, int session_id) {
        // 你的代码：
        // - 如果是 Connecting，转换到 Connected{session_id}
        // - 其他状态，保持不变
        (void)state; (void)session_id;
        return Idle{};
    }

    ConnectionState disconnect(const ConnectionState& state, const std::string& reason) {
        // 你的代码：
        // - 如果是 Connected，转换到 Disconnected{reason}
        // - 其他状态，保持不变
        (void)state; (void)reason;
        return Idle{};
    }

    // TODO: 实现状态描述函数
    std::string describe_state(const ConnectionState& state) {
        // 你的代码：使用 visit 返回状态描述
        (void)state;
        return "Unknown";
    }
}

// ============================================
// 练习 3: 表达式树求值
// ============================================

namespace exercise3 {
    struct Expr;
    using ExprPtr = std::shared_ptr<Expr>;

    struct Literal { int value; };
    struct Add { ExprPtr left, right; };
    struct Mul { ExprPtr left, right; };
    struct Div { ExprPtr left, right; };

    struct Expr {
        std::variant<Literal, Add, Mul, Div> data;
        Expr(Literal lit) : data(lit) {}
        Expr(Add add) : data(add) {}
        Expr(Mul mul) : data(mul) {}
        Expr(Div div) : data(div) {}
    };

    // 辅助构造函数
    ExprPtr lit(int value) {
        return std::make_shared<Expr>(Literal{value});
    }

    ExprPtr add(ExprPtr left, ExprPtr right) {
        return std::make_shared<Expr>(Add{left, right});
    }

    ExprPtr mul(ExprPtr left, ExprPtr right) {
        return std::make_shared<Expr>(Mul{left, right});
    }

    ExprPtr div(ExprPtr left, ExprPtr right) {
        return std::make_shared<Expr>(Div{left, right});
    }

    // TODO: 实现求值函数
    int eval(const Expr& expr) {
        // 你的代码：使用 visit 递归求值
        // 注意：除法需要检查除数是否为 0
        (void)expr;
        return 0;
    }

    // TODO: 实现表达式转字符串
    std::string to_string(const Expr& expr) {
        // 你的代码：使用 visit 递归转换
        // 格式：(left op right)
        (void)expr;
        return "";
    }
}

// ============================================
// 练习 4: Maybe 类型（类似 Optional）
// ============================================

namespace exercise4 {
    // TODO: 使用 variant 实现 Maybe<T>
    template<typename T>
    class Maybe {
    public:
        // 你的代码：使用 variant<std::monostate, T>
        std::variant<std::monostate, T> data;

        // TODO: 实现 has_value
        bool has_value() const {
            // 你的代码
            return false;
        }

        // TODO: 实现 value
        const T& value() const {
            // 你的代码
            throw std::runtime_error("Not implemented");
        }

        // TODO: 实现 value_or
        T value_or(const T& default_value) const {
            // 你的代码
            (void)default_value;
            return T();
        }

        // TODO: 实现 map
        template<typename F>
        auto map(F f) const {
            // 你的代码
            using NewT = decltype(f(std::declval<T>()));
            (void)f;
            return Maybe<NewT>();
        }

        // 静态工厂
        static Maybe just(T value) {
            return Maybe{std::variant<std::monostate, T>(std::in_place_index<1>, value)};
        }

        static Maybe nothing() {
            return Maybe{std::variant<std::monostate, T>(std::monostate{})};
        }
    };
}

// ============================================
// 练习 5: 配置值类型
// ============================================

namespace exercise5 {
    // TODO: 实现支持多种类型的配置值
    class ConfigValue {
    public:
        // 你的代码：使用 variant<int, double, std::string, bool>
        std::variant<int, double, std::string, bool> value;

        ConfigValue(int v) : value(v) {}
        ConfigValue(double v) : value(v) {}
        ConfigValue(const std::string& v) : value(v) {}
        ConfigValue(bool v) : value(v) {}

        // TODO: 实现 as_int
        std::optional<int> as_int() const {
            // 你的代码：如果是 int，返回值；否则返回 nullopt
            return std::nullopt;
        }

        // TODO: 实现 as_double
        std::optional<double> as_double() const {
            // 你的代码
            return std::nullopt;
        }

        // TODO: 实现 as_string
        std::optional<std::string> as_string() const {
            // 你的代码
            return std::nullopt;
        }

        // TODO: 实现 as_bool
        std::optional<bool> as_bool() const {
            // 你的代码
            return std::nullopt;
        }

        // TODO: 实现 to_string (将任意类型转为字符串)
        std::string to_string() const {
            // 你的代码：使用 visit
            return "";
        }
    };
}

// ============================================
// 测试代码
// ============================================

void test_exercise_1() {
    std::cout << "=== Exercise 1: Result Type ===\n";

    using namespace exercise1;

    auto r1 = safe_divide(10, 2);
    std::cout << "10 / 2: "
              << (r1.is_ok() ? std::to_string(r1.unwrap()) : r1.unwrap_err())
              << " (expected: 5)\n";

    auto r2 = safe_divide(10, 0);
    std::cout << "10 / 0: "
              << (r2.is_ok() ? std::to_string(r2.unwrap()) : r2.unwrap_err())
              << " (expected: error)\n";

    auto r3 = safe_sqrt(16.0);
    std::cout << "sqrt(16): "
              << (r3.is_ok() ? std::to_string(r3.unwrap()) : r3.unwrap_err())
              << " (expected: 4)\n";

    // map 测试
    auto r4 = safe_divide(10, 2).map([](int x) { return x * 2; });
    std::cout << "10 / 2 * 2: "
              << (r4.is_ok() ? std::to_string(r4.unwrap()) : r4.unwrap_err())
              << " (expected: 10)\n\n";
}

void test_exercise_2() {
    std::cout << "=== Exercise 2: State Machine ===\n";

    using namespace exercise2;

    ConnectionState state = Idle{};
    std::cout << "Initial: " << describe_state(state) << "\n";

    state = connect(state, "example.com");
    std::cout << "After connect: " << describe_state(state) << "\n";

    state = establish(state, 12345);
    std::cout << "After establish: " << describe_state(state) << "\n";

    state = disconnect(state, "User logout");
    std::cout << "After disconnect: " << describe_state(state) << "\n\n";
}

void test_exercise_3() {
    std::cout << "=== Exercise 3: Expression Tree ===\n";

    using namespace exercise3;

    // (2 + 3) * 4
    auto expr1 = mul(add(lit(2), lit(3)), lit(4));
    std::cout << "Expression: " << to_string(*expr1) << "\n";
    std::cout << "Result: " << eval(*expr1) << " (expected: 20)\n";

    // (10 / 2) + 3
    auto expr2 = add(div(lit(10), lit(2)), lit(3));
    std::cout << "Expression: " << to_string(*expr2) << "\n";
    std::cout << "Result: " << eval(*expr2) << " (expected: 8)\n\n";
}

void test_exercise_4() {
    std::cout << "=== Exercise 4: Maybe Type ===\n";

    using namespace exercise4;

    auto m1 = Maybe<int>::just(42);
    std::cout << "just(42).has_value: " << m1.has_value() << " (expected: 1)\n";
    std::cout << "just(42).value: " << m1.value() << " (expected: 42)\n";

    auto m2 = Maybe<int>::nothing();
    std::cout << "nothing().has_value: " << m2.has_value() << " (expected: 0)\n";
    std::cout << "nothing().value_or(99): " << m2.value_or(99) << " (expected: 99)\n";

    auto m3 = m1.map([](int x) { return x * 2; });
    std::cout << "just(42).map(*2).value: " << m3.value() << " (expected: 84)\n\n";
}

void test_exercise_5() {
    std::cout << "=== Exercise 5: Config Value ===\n";

    using namespace exercise5;

    ConfigValue v1(42);
    std::cout << "int value: " << v1.to_string() << " (expected: 42)\n";
    std::cout << "as_int: " << v1.as_int().value_or(-1) << " (expected: 42)\n";

    ConfigValue v2(3.14);
    std::cout << "double value: " << v2.to_string() << " (expected: 3.14)\n";

    ConfigValue v3("hello");
    std::cout << "string value: " << v3.to_string() << " (expected: hello)\n";

    ConfigValue v4(true);
    std::cout << "bool value: " << v4.to_string() << " (expected: 1)\n\n";
}

int main() {
    std::cout << "C++ Functional Programming - Chapter 06 Exercises\n";
    std::cout << "=================================================\n\n";

    test_exercise_1();
    test_exercise_2();
    test_exercise_3();
    test_exercise_4();
    test_exercise_5();

    std::cout << "Hint: Check solutions in exercises/solutions/06_algebraic_types_solutions.cpp\n";

    return 0;
}
