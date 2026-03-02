/**
 * @file 01_algebraic_types.cpp
 * @brief 代数数据类型示例
 *
 * 本文件演示：
 * - 积类型（struct, tuple, pair）
 * - 和类型（variant, optional）
 * - 模式匹配（overload 模式）
 * - 状态机建模
 * - 表达式树
 */

#include <compare>
#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <variant>
#include <vector>

// ============================================
// overload 模式辅助工具
// ============================================

template<class... Ts>
struct overload : Ts... {
    using Ts::operator()...;
};

// C++17 推导指引（C++20 可省略）
template<class... Ts>
overload(Ts...) -> overload<Ts...>;

// ============================================
// 1. 积类型 (Product Types)
// ============================================

// C++20：使用默认比较运算符
struct Point {
    int x;
    int y;

    auto operator<=>(const Point&) const = default;
};

void product_types_demo() {
    std::cout << "=== 积类型 ===\n";

    // struct
    Point p1{3, 4}, p2{3, 4}, p3{1, 2};
    std::cout << "p1 == p2: " << (p1 == p2) << "\n";
    std::cout << "p1 < p3: " << (p1 < p3) << "\n";

    // tuple
    std::tuple<int, std::string, double> person{25, "Alice", 1.65};
    auto [age, name, height] = person;
    std::cout << name << " 年龄 " << age << ", 身高 " << height << "m\n";

    // pair
    std::pair<std::string, int> score{"Bob", 95};
    std::cout << score.first << " 得分: " << score.second << "\n\n";
}

// ============================================
// 2. 和类型 - variant
// ============================================

void sum_types_demo() {
    std::cout << "=== 和类型 (variant) ===\n";

    std::variant<int, double, std::string> value;

    value = 42;
    std::cout << "当前类型索引: " << value.index() << ", 值: ";
    std::visit([](auto&& arg) { std::cout << arg << "\n"; }, value);

    value = 3.14159;
    std::cout << "当前类型索引: " << value.index() << ", 值: ";
    std::visit([](auto&& arg) { std::cout << arg << "\n"; }, value);

    value = "Hello, variant!";
    std::cout << "当前类型索引: " << value.index() << ", 值: ";
    std::visit([](auto&& arg) { std::cout << arg << "\n"; }, value);

    // 类型检查
    if (std::holds_alternative<std::string>(value)) {
        std::cout << "是字符串: " << std::get<std::string>(value) << "\n";
    }

    std::cout << "\n";
}

// ============================================
// 3. optional
// ============================================

[[nodiscard]] std::optional<int> find_value(
    const std::vector<int>& vec,
    int target
) {
    for (int v : vec) {
        if (v == target) return v;
    }
    return std::nullopt;
}

[[nodiscard]] std::optional<int> safe_divide(int a, int b) {
    if (b == 0) return std::nullopt;
    return a / b;
}

void optional_demo() {
    std::cout << "=== optional ===\n";

    std::vector<int> nums{1, 2, 3, 4, 5};

    auto result1 = find_value(nums, 3);
    if (result1) {
        std::cout << "找到: " << *result1 << "\n";
    }

    auto result2 = find_value(nums, 10);
    std::cout << "找到10: " << (result2 ? "是" : "否") << "\n";

    // value_or
    int val = result2.value_or(-1);
    std::cout << "value_or(-1): " << val << "\n";

    // 链式调用（手动实现 transform）
    auto div_result = safe_divide(10, 2);
    auto transformed = div_result ? std::optional<int>(*div_result * 2) : std::nullopt;
    std::cout << "10/2*2 = " << transformed.value_or(0) << "\n";

    std::cout << "\n";
}

// ============================================
// 4. 模式匹配 - overload 模式
// ============================================

void pattern_matching_demo() {
    std::cout << "=== 模式匹配 ===\n";

    std::variant<int, double, std::string> v = 42;

    // 使用 overload 模式
    auto printer = overload{
        [](int i) { std::cout << "整数: " << i << "\n"; },
        [](double d) { std::cout << "浮点: " << d << "\n"; },
        [](const std::string& s) { std::cout << "字符串: " << s << "\n"; }
    };

    std::visit(printer, v);

    v = 3.14159;
    std::visit(printer, v);

    v = "模式匹配示例";
    std::visit(printer, v);

    std::cout << "\n";
}

// ============================================
// 5. 状态机
// ============================================

// 状态定义（带 C++20 默认比较）
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
        [](const Idle&) {
            std::cout << "  状态: 空闲\n";
        },
        [](const Running& r) {
            std::cout << "  状态: 运行中 (" << r.progress << "%)\n";
        },
        [](const Completed& c) {
            std::cout << "  状态: 完成 - " << c.result << "\n";
        },
        [](const Failed& f) {
            std::cout << "  状态: 失败 - " << f.error << "\n";
        }
    }, state);
}

// 状态转换函数
[[nodiscard]] TaskState start_task(const TaskState& state) {
    return std::visit(overload{
        [](const Idle&) -> TaskState { return Running{0}; },
        [](const auto& s) -> TaskState { return s; }  // 其他状态不变
    }, state);
}

[[nodiscard]] TaskState update_progress(const TaskState& state, int progress) {
    return std::visit(overload{
        [progress](const Running&) -> TaskState {
            if (progress >= 100) {
                return Completed{"任务完成"};
            }
            return Running{progress};
        },
        [](const auto& s) -> TaskState { return s; }
    }, state);
}

void state_machine_demo() {
    std::cout << "=== 状态机 ===\n";

    TaskState state = Idle{};
    handle_state(state);

    state = start_task(state);
    handle_state(state);

    state = update_progress(state, 50);
    handle_state(state);

    state = update_progress(state, 100);
    handle_state(state);

    // 失败状态
    TaskState failed = Failed{"网络超时"};
    handle_state(failed);

    std::cout << "\n";
}

// ============================================
// 6. 表达式树
// ============================================

struct Expr;
using ExprPtr = std::shared_ptr<Expr>;

struct Literal { double value; };
struct Add { ExprPtr left, right; };
struct Mul { ExprPtr left, right; };
struct Sub { ExprPtr left, right; };
struct Div { ExprPtr left, right; };

struct Expr {
    std::variant<Literal, Add, Mul, Sub, Div> data;

    explicit Expr(Literal lit) : data(lit) {}
    explicit Expr(Add add) : data(add) {}
    explicit Expr(Mul mul) : data(mul) {}
    explicit Expr(Sub sub) : data(sub) {}
    explicit Expr(Div div) : data(div) {}
};

// 构建函数
[[nodiscard]] ExprPtr lit(double value) {
    return std::make_shared<Expr>(Literal{value});
}

[[nodiscard]] ExprPtr add(ExprPtr left, ExprPtr right) {
    return std::make_shared<Expr>(Add{left, right});
}

[[nodiscard]] ExprPtr mul(ExprPtr left, ExprPtr right) {
    return std::make_shared<Expr>(Mul{left, right});
}

[[nodiscard]] ExprPtr sub(ExprPtr left, ExprPtr right) {
    return std::make_shared<Expr>(Sub{left, right});
}

[[nodiscard]] ExprPtr div_expr(ExprPtr left, ExprPtr right) {
    return std::make_shared<Expr>(Div{left, right});
}

// 求值函数
double evaluate(const Expr& expr);

double evaluate(const Expr& expr) {
    return std::visit(overload{
        [](const Literal& l) { return l.value; },
        [](const Add& a) { return evaluate(*a.left) + evaluate(*a.right); },
        [](const Mul& m) { return evaluate(*m.left) * evaluate(*m.right); },
        [](const Sub& s) { return evaluate(*s.left) - evaluate(*s.right); },
        [](const Div& d) { return evaluate(*d.left) / evaluate(*d.right); }
    }, expr.data);
}

// 转字符串
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
        [](const Sub& s) {
            return "(" + expr_to_string(*s.left) + " - " + expr_to_string(*s.right) + ")";
        },
        [](const Div& d) {
            return "(" + expr_to_string(*d.left) + " / " + expr_to_string(*d.right) + ")";
        }
    }, expr.data);
}

void expression_tree_demo() {
    std::cout << "=== 表达式树 ===\n";

    // (2 + 3) * (10 - 4)
    auto expr1 = mul(
        add(lit(2), lit(3)),
        sub(lit(10), lit(4))
    );
    std::cout << "表达式: " << expr_to_string(*expr1) << "\n";
    std::cout << "结果: " << evaluate(*expr1) << "\n";

    // 100 / (5 * 4)
    auto expr2 = div_expr(
        lit(100),
        mul(lit(5), lit(4))
    );
    std::cout << "表达式: " << expr_to_string(*expr2) << "\n";
    std::cout << "结果: " << evaluate(*expr2) << "\n";

    std::cout << "\n";
}

// ============================================
// 7. API 响应处理
// ============================================

struct Success {
    std::string data;
    auto operator<=>(const Success&) const = default;
};

struct NotFound {
    std::string resource;
    auto operator<=>(const NotFound&) const = default;
};

struct ServerError {
    int code;
    std::string message;
    auto operator<=>(const ServerError&) const = default;
};

using ApiResponse = std::variant<Success, NotFound, ServerError>;

void handle_response(const ApiResponse& resp) {
    std::visit(overload{
        [](const Success& s) {
            std::cout << "  成功: " << s.data << "\n";
        },
        [](const NotFound& nf) {
            std::cout << "  404: " << nf.resource << " 未找到\n";
        },
        [](const ServerError& se) {
            std::cout << "  错误 " << se.code << ": " << se.message << "\n";
        }
    }, resp);
}

void api_response_demo() {
    std::cout << "=== API 响应处理 ===\n";

    std::vector<ApiResponse> responses{
        Success{"用户数据获取成功"},
        NotFound{"/api/user/999"},
        ServerError{500, "内部服务器错误"}
    };

    for (const auto& resp : responses) {
        handle_response(resp);
    }

    std::cout << "\n";
}

// ============================================
// 8. 配置值系统
// ============================================

using ConfigValue = std::variant<int, double, std::string, bool>;

template<typename T>
[[nodiscard]] std::optional<T> get_as(const ConfigValue& v) {
    if (std::holds_alternative<T>(v)) {
        return std::get<T>(v);
    }
    return std::nullopt;
}

[[nodiscard]] std::string config_to_string(const ConfigValue& v) {
    return std::visit(overload{
        [](int i) { return std::to_string(i); },
        [](double d) { return std::to_string(d); },
        [](const std::string& s) { return "\"" + s + "\""; },
        [](bool b) { return std::string(b ? "true" : "false"); }
    }, v);
}

void config_value_demo() {
    std::cout << "=== 配置值系统 ===\n";

    std::vector<std::pair<std::string, ConfigValue>> config = {
        {"port", 8080},
        {"timeout", 30.5},
        {"host", std::string("localhost")},
        {"debug", true}
    };

    for (const auto& [key, value] : config) {
        std::cout << "  " << key << " = " << config_to_string(value) << "\n";
    }

    // 类型安全获取
    auto port = get_as<int>(config[0].second);
    if (port) {
        std::cout << "端口号: " << *port << "\n";
    }

    std::cout << "\n";
}

// ============================================
// main
// ============================================

int main() {
    product_types_demo();
    sum_types_demo();
    optional_demo();
    pattern_matching_demo();
    state_machine_demo();
    expression_tree_demo();
    api_response_demo();
    config_value_demo();

    return 0;
}
