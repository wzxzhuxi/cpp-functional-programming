#include <cmath>
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
    template<typename T, typename E>
    class Result {
    public:
        std::variant<T, E> data;

        bool is_ok() const {
            return std::holds_alternative<T>(data);
        }

        bool is_err() const {
            return std::holds_alternative<E>(data);
        }

        const T& unwrap() const {
            return std::get<T>(data);
        }

        const E& unwrap_err() const {
            return std::get<E>(data);
        }

        template<typename F>
        auto map(F f) const {
            using NewT = decltype(f(std::declval<T>()));
            if (is_ok()) {
                return Result<NewT, E>::ok(f(unwrap()));
            } else {
                return Result<NewT, E>::err(unwrap_err());
            }
        }

        static Result ok(T value) {
            return Result{std::variant<T, E>(std::in_place_index<0>, value)};
        }

        static Result err(E error) {
            return Result{std::variant<T, E>(std::in_place_index<1>, error)};
        }
    };

    Result<int, std::string> safe_divide(int a, int b) {
        if (b == 0) {
            return Result<int, std::string>::err("Division by zero");
        }
        return Result<int, std::string>::ok(a / b);
    }

    Result<double, std::string> safe_sqrt(double x) {
        if (x < 0) {
            return Result<double, std::string>::err("Negative sqrt");
        }
        return Result<double, std::string>::ok(std::sqrt(x));
    }
}

// ============================================
// 练习 2: 状态机实现
// ============================================

namespace exercise2 {
    struct Idle {};
    struct Connecting { std::string host; };
    struct Connected { int session_id; };
    struct Disconnected { std::string reason; };

    using ConnectionState = std::variant<Idle, Connecting, Connected, Disconnected>;

    ConnectionState connect(const ConnectionState& state, const std::string& host) {
        return std::visit(overload{
            [&](const Idle&) -> ConnectionState { return Connecting{host}; },
            [&](const auto& other) -> ConnectionState { return other; }
        }, state);
    }

    ConnectionState establish(const ConnectionState& state, int session_id) {
        return std::visit(overload{
            [&](const Connecting&) -> ConnectionState { return Connected{session_id}; },
            [&](const auto& other) -> ConnectionState { return other; }
        }, state);
    }

    ConnectionState disconnect(const ConnectionState& state, const std::string& reason) {
        return std::visit(overload{
            [&](const Connected&) -> ConnectionState { return Disconnected{reason}; },
            [&](const auto& other) -> ConnectionState { return other; }
        }, state);
    }

    std::string describe_state(const ConnectionState& state) {
        return std::visit(overload{
            [](const Idle&) { return std::string("Idle"); },
            [](const Connecting& c) { return "Connecting to " + c.host; },
            [](const Connected& c) { return "Connected (session " + std::to_string(c.session_id) + ")"; },
            [](const Disconnected& d) { return "Disconnected: " + d.reason; }
        }, state);
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

    int eval(const Expr& expr);

    int eval(const Expr& expr) {
        return std::visit(overload{
            [](const Literal& l) { return l.value; },
            [](const Add& a) { return eval(*a.left) + eval(*a.right); },
            [](const Mul& m) { return eval(*m.left) * eval(*m.right); },
            [](const Div& d) {
                int right_val = eval(*d.right);
                if (right_val == 0) return 0;  // 简化处理
                return eval(*d.left) / right_val;
            }
        }, expr.data);
    }

    std::string to_string(const Expr& expr);

    std::string to_string(const Expr& expr) {
        return std::visit(overload{
            [](const Literal& l) { return std::to_string(l.value); },
            [](const Add& a) { return "(" + to_string(*a.left) + " + " + to_string(*a.right) + ")"; },
            [](const Mul& m) { return "(" + to_string(*m.left) + " * " + to_string(*m.right) + ")"; },
            [](const Div& d) { return "(" + to_string(*d.left) + " / " + to_string(*d.right) + ")"; }
        }, expr.data);
    }
}

// ============================================
// 练习 4: Maybe 类型（类似 Optional）
// ============================================

namespace exercise4 {
    template<typename T>
    class Maybe {
    public:
        std::variant<std::monostate, T> data;

        bool has_value() const {
            return std::holds_alternative<T>(data);
        }

        const T& value() const {
            return std::get<T>(data);
        }

        T value_or(const T& default_value) const {
            if (has_value()) {
                return value();
            }
            return default_value;
        }

        template<typename F>
        auto map(F f) const {
            using NewT = decltype(f(std::declval<T>()));
            if (has_value()) {
                return Maybe<NewT>::just(f(value()));
            }
            return Maybe<NewT>::nothing();
        }

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
    class ConfigValue {
    public:
        std::variant<int, double, std::string, bool> value;

        ConfigValue(int v) : value(v) {}
        ConfigValue(double v) : value(v) {}
        ConfigValue(const std::string& v) : value(v) {}
        ConfigValue(bool v) : value(v) {}

        std::optional<int> as_int() const {
            if (std::holds_alternative<int>(value)) {
                return std::get<int>(value);
            }
            return std::nullopt;
        }

        std::optional<double> as_double() const {
            if (std::holds_alternative<double>(value)) {
                return std::get<double>(value);
            }
            return std::nullopt;
        }

        std::optional<std::string> as_string() const {
            if (std::holds_alternative<std::string>(value)) {
                return std::get<std::string>(value);
            }
            return std::nullopt;
        }

        std::optional<bool> as_bool() const {
            if (std::holds_alternative<bool>(value)) {
                return std::get<bool>(value);
            }
            return std::nullopt;
        }

        std::string to_string() const {
            return std::visit(overload{
                [](int i) { return std::to_string(i); },
                [](double d) { return std::to_string(d); },
                [](const std::string& s) { return s; },
                [](bool b) { return std::to_string(b); }
            }, value);
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
    std::cout << "C++ Functional Programming - Chapter 06 Solutions\n";
    std::cout << "=================================================\n\n";

    test_exercise_1();
    test_exercise_2();
    test_exercise_3();
    test_exercise_4();
    test_exercise_5();

    return 0;
}
