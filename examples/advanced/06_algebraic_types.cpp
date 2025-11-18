#include <cmath>
#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <variant>
#include <vector>

// ============================================
// Overload pattern for visit
// ============================================

template<class... Ts>
struct overload : Ts... { using Ts::operator()...; };

template<class... Ts>
overload(Ts...) -> overload<Ts...>;

// ============================================
// 1. Product Types
// ============================================

void product_types_demo() {
    std::cout << "=== Product Types ===\n";

    // Tuple
    std::tuple<int, std::string, double> person{25, "Alice", 1.65};
    auto [age, name, height] = person;
    std::cout << name << " is " << age << " years old, " << height << "m tall\n";

    // Pair
    std::pair<std::string, int> score{"Bob", 95};
    std::cout << score.first << " scored " << score.second << "\n\n";
}

// ============================================
// 2. Sum Types - variant
// ============================================

void sum_types_demo() {
    std::cout << "=== Sum Types (variant) ===\n";

    std::variant<int, double, std::string> value;

    value = 42;
    std::visit([](auto&& arg) { std::cout << "Value: " << arg << "\n"; }, value);

    value = 3.14;
    std::visit([](auto&& arg) { std::cout << "Value: " << arg << "\n"; }, value);

    value = "hello";
    std::visit([](auto&& arg) { std::cout << "Value: " << arg << "\n"; }, value);
    std::cout << "\n";
}

// ============================================
// 3. Optional
// ============================================

std::optional<int> find_value(const std::vector<int>& vec, int target) {
    for (int v : vec) {
        if (v == target) return v;
    }
    return std::nullopt;
}

void optional_demo() {
    std::cout << "=== Optional ===\n";

    std::vector<int> nums{1, 2, 3, 4, 5};

    auto result1 = find_value(nums, 3);
    if (result1) {
        std::cout << "Found: " << *result1 << "\n";
    }

    auto result2 = find_value(nums, 10);
    if (!result2) {
        std::cout << "Not found: 10\n";
    }
    std::cout << "\n";
}

// ============================================
// 4. Pattern Matching with overload
// ============================================

void pattern_matching_demo() {
    std::cout << "=== Pattern Matching ===\n";

    std::variant<int, double, std::string> v = 42;

    std::visit(overload{
        [](int i) { std::cout << "Integer: " << i << "\n"; },
        [](double d) { std::cout << "Double: " << d << "\n"; },
        [](const std::string& s) { std::cout << "String: " << s << "\n"; }
    }, v);

    v = 3.14159;
    std::visit(overload{
        [](int i) { std::cout << "Integer: " << i << "\n"; },
        [](double d) { std::cout << "Double: " << d << "\n"; },
        [](const std::string& s) { std::cout << "String: " << s << "\n"; }
    }, v);

    v = "test";
    std::visit(overload{
        [](int i) { std::cout << "Integer: " << i << "\n"; },
        [](double d) { std::cout << "Double: " << d << "\n"; },
        [](const std::string& s) { std::cout << "String: " << s << "\n"; }
    }, v);
    std::cout << "\n";
}

// ============================================
// 5. Result Type
// ============================================

template<typename T, typename E>
struct Result {
    std::variant<T, E> data;

    bool is_ok() const { return std::holds_alternative<T>(data); }
    const T& unwrap() const { return std::get<T>(data); }
    const E& unwrap_err() const { return std::get<E>(data); }

    static Result ok(T value) { return Result{std::variant<T, E>(std::in_place_index<0>, value)}; }
    static Result err(E error) { return Result{std::variant<T, E>(std::in_place_index<1>, error)}; }
};

Result<int, std::string> safe_divide(int a, int b) {
    if (b == 0) return Result<int, std::string>::err("Division by zero");
    return Result<int, std::string>::ok(a / b);
}

Result<double, std::string> safe_sqrt(double x) {
    if (x < 0) return Result<double, std::string>::err("Negative sqrt");
    return Result<double, std::string>::ok(std::sqrt(x));
}

void result_type_demo() {
    std::cout << "=== Result Type ===\n";

    auto r1 = safe_divide(10, 2);
    if (r1.is_ok()) {
        std::cout << "10 / 2 = " << r1.unwrap() << "\n";
    } else {
        std::cout << "Error: " << r1.unwrap_err() << "\n";
    }

    auto r2 = safe_divide(10, 0);
    if (!r2.is_ok()) {
        std::cout << "10 / 0 error: " << r2.unwrap_err() << "\n";
    }

    auto r3 = safe_sqrt(16.0);
    if (r3.is_ok()) {
        std::cout << "sqrt(16) = " << r3.unwrap() << "\n";
    }

    auto r4 = safe_sqrt(-4.0);
    if (!r4.is_ok()) {
        std::cout << "sqrt(-4) error: " << r4.unwrap_err() << "\n";
    }
    std::cout << "\n";
}

// ============================================
// 6. State Machine
// ============================================

struct Idle {};
struct Running { int progress; };
struct Completed { std::string result; };
struct Failed { std::string error; };

using State = std::variant<Idle, Running, Completed, Failed>;

void handle_state(const State& state) {
    std::visit(overload{
        [](const Idle&) { std::cout << "State: Idle\n"; },
        [](const Running& r) { std::cout << "State: Running (" << r.progress << "%)\n"; },
        [](const Completed& c) { std::cout << "State: Completed - " << c.result << "\n"; },
        [](const Failed& f) { std::cout << "State: Failed - " << f.error << "\n"; }
    }, state);
}

void state_machine_demo() {
    std::cout << "=== State Machine ===\n";

    std::vector<State> states{
        Idle{},
        Running{25},
        Running{50},
        Running{75},
        Completed{"Success!"}
    };

    for (const auto& s : states) {
        handle_state(s);
    }

    // 错误状态
    State failed = Failed{"Network timeout"};
    handle_state(failed);
    std::cout << "\n";
}

// ============================================
// 7. Expression Tree (Recursive ADT)
// ============================================

struct Expr;
using ExprPtr = std::shared_ptr<Expr>;

struct Literal { int value; };
struct Add { ExprPtr left, right; };
struct Mul { ExprPtr left, right; };
struct Sub { ExprPtr left, right; };

struct Expr {
    std::variant<Literal, Add, Mul, Sub> data;

    Expr(Literal lit) : data(lit) {}
    Expr(Add add) : data(add) {}
    Expr(Mul mul) : data(mul) {}
    Expr(Sub sub) : data(sub) {}
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

ExprPtr sub(ExprPtr left, ExprPtr right) {
    return std::make_shared<Expr>(Sub{left, right});
}

int eval(const Expr& expr);

int eval(const Expr& expr) {
    return std::visit(overload{
        [](const Literal& l) { return l.value; },
        [](const Add& a) { return eval(*a.left) + eval(*a.right); },
        [](const Mul& m) { return eval(*m.left) * eval(*m.right); },
        [](const Sub& s) { return eval(*s.left) - eval(*s.right); }
    }, expr.data);
}

std::string to_string(const Expr& expr);

std::string to_string(const Expr& expr) {
    return std::visit(overload{
        [](const Literal& l) { return std::to_string(l.value); },
        [](const Add& a) { return "(" + to_string(*a.left) + " + " + to_string(*a.right) + ")"; },
        [](const Mul& m) { return "(" + to_string(*m.left) + " * " + to_string(*m.right) + ")"; },
        [](const Sub& s) { return "(" + to_string(*s.left) + " - " + to_string(*s.right) + ")"; }
    }, expr.data);
}

void expression_tree_demo() {
    std::cout << "=== Expression Tree ===\n";

    // (2 + 3) * (5 - 1)
    auto expr = mul(
        add(lit(2), lit(3)),
        sub(lit(5), lit(1))
    );

    std::cout << "Expression: " << to_string(*expr) << "\n";
    std::cout << "Result: " << eval(*expr) << "\n\n";
}

// ============================================
// 8. API Response Example
// ============================================

struct Success { std::string data; };
struct NotFound { std::string resource; };
struct ServerError { int code; std::string message; };

using ApiResponse = std::variant<Success, NotFound, ServerError>;

void handle_response(const ApiResponse& resp) {
    std::visit(overload{
        [](const Success& s) {
            std::cout << "Success: " << s.data << "\n";
        },
        [](const NotFound& nf) {
            std::cout << "404: " << nf.resource << " not found\n";
        },
        [](const ServerError& se) {
            std::cout << "Error " << se.code << ": " << se.message << "\n";
        }
    }, resp);
}

void api_response_demo() {
    std::cout << "=== API Response Handling ===\n";

    std::vector<ApiResponse> responses{
        Success{"User data retrieved"},
        NotFound{"/api/user/999"},
        ServerError{500, "Internal server error"}
    };

    for (const auto& resp : responses) {
        handle_response(resp);
    }
    std::cout << "\n";
}

// ============================================
// Main
// ============================================

int main() {
    product_types_demo();
    sum_types_demo();
    optional_demo();
    pattern_matching_demo();
    result_type_demo();
    state_machine_demo();
    expression_tree_demo();
    api_response_demo();

    return 0;
}
