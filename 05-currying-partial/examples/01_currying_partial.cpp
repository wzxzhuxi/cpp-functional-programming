/**
 * @file 01_currying_partial.cpp
 * @brief 柯里化与偏应用示例
 *
 * 本文件演示：
 * - 手动柯里化
 * - 通用柯里化模板
 * - 偏应用
 * - std::bind 对比
 * - 实际应用案例
 */

#include <cmath>
#include <functional>
#include <iostream>
#include <string>
#include <tuple>
#include <type_traits>

// ============================================
// 1. 手动柯里化
// ============================================

// 原始三参数函数
int add3(int a, int b, int c) {
    return a + b + c;
}

// 手动柯里化版本
[[nodiscard]] auto curry_add3(int a) {
    return [a](int b) {
        return [a, b](int c) {
            return a + b + c;
        };
    };
}

void manual_currying_demo() {
    std::cout << "=== 手动柯里化 ===\n";

    // 逐步应用
    auto step1 = curry_add3(1);
    auto step2 = step1(2);
    int result = step2(3);
    std::cout << "curry_add3(1)(2)(3) = " << result << "\n";

    // 一步到位
    std::cout << "curry_add3(10)(20)(30) = " << curry_add3(10)(20)(30) << "\n";

    // 复用中间函数
    auto add_to_100 = curry_add3(100);
    std::cout << "add_to_100(0)(0) = " << add_to_100(0)(0) << "\n";
    std::cout << "add_to_100(50)(50) = " << add_to_100(50)(50) << "\n\n";
}

// ============================================
// 2. 通用柯里化模板
// ============================================

// 两参数柯里化
template<typename F>
[[nodiscard]] auto curry2(F f) {
    return [f](auto a) {
        return [f, a](auto b) {
            return f(a, b);
        };
    };
}

// 三参数柯里化
template<typename F>
[[nodiscard]] auto curry3(F f) {
    return [f](auto a) {
        return [f, a](auto b) {
            return [f, a, b](auto c) {
                return f(a, b, c);
            };
        };
    };
}

// 四参数柯里化
template<typename F>
[[nodiscard]] auto curry4(F f) {
    return [f](auto a) {
        return [f, a](auto b) {
            return [f, a, b](auto c) {
                return [f, a, b, c](auto d) {
                    return f(a, b, c, d);
                };
            };
        };
    };
}

void generic_currying_demo() {
    std::cout << "=== 通用柯里化模板 ===\n";

    // curry2
    auto multiply = [](int a, int b) { return a * b; };
    auto curried_mult = curry2(multiply);

    auto times5 = curried_mult(5);
    std::cout << "times5(10) = " << times5(10) << "\n";
    std::cout << "times5(7) = " << times5(7) << "\n";

    // curry3
    auto volume = [](int l, int w, int h) { return l * w * h; };
    auto curried_vol = curry3(volume);

    std::cout << "volume(2)(3)(4) = " << curried_vol(2)(3)(4) << "\n";

    // 类型转换也可以
    auto format = [](std::string prefix, std::string content, std::string suffix) {
        return prefix + content + suffix;
    };
    auto curried_format = curry3(format);

    auto with_brackets = curried_format("[");
    auto bracketed = with_brackets("]");  // 注意参数顺序问题
    // 这里需要调整函数设计...

    std::cout << "\n";
}

// ============================================
// 3. 偏应用
// ============================================

// 固定第一个参数
template<typename F, typename Arg>
[[nodiscard]] auto partial(F f, Arg arg) {
    return [f, arg](auto... rest) {
        return f(arg, rest...);
    };
}

// 固定多个参数
template<typename F, typename... Args>
[[nodiscard]] auto partial_n(F f, Args... args) {
    return [f, args...](auto... rest) {
        return f(args..., rest...);
    };
}

// 从右侧固定
template<typename F, typename... Args>
[[nodiscard]] auto partial_right(F f, Args... args) {
    return [f, args...](auto... first) {
        return f(first..., args...);
    };
}

void partial_application_demo() {
    std::cout << "=== 偏应用 ===\n";

    // 基本偏应用
    auto power = [](int base, int exp) {
        return static_cast<int>(std::pow(base, exp));
    };

    auto powers_of_2 = partial(power, 2);
    std::cout << "powers_of_2(3) = " << powers_of_2(3) << " (2³)\n";
    std::cout << "powers_of_2(10) = " << powers_of_2(10) << " (2¹⁰)\n";

    // 固定指数
    auto square = partial([](int exp, int base) {
        return static_cast<int>(std::pow(base, exp));
    }, 2);
    auto cube = partial([](int exp, int base) {
        return static_cast<int>(std::pow(base, exp));
    }, 3);

    std::cout << "square(5) = " << square(5) << "\n";
    std::cout << "cube(3) = " << cube(3) << "\n";

    // 多参数偏应用
    auto greet = [](std::string greeting, std::string name, std::string punct) {
        return greeting + ", " + name + punct;
    };

    auto say_hello = partial_n(greet, std::string("Hello"));
    std::cout << say_hello(std::string("World"), std::string("!")) << "\n";

    auto excited_hello = partial_n(greet, std::string("Hello"), std::string("World"));
    std::cout << excited_hello(std::string("!!!")) << "\n\n";
}

// ============================================
// 4. 柯里化 vs 偏应用
// ============================================

void currying_vs_partial_demo() {
    std::cout << "=== 柯里化 vs 偏应用 ===\n";

    auto add = [](int a, int b, int c) { return a + b + c; };

    // 柯里化：必须逐个传参
    auto curried = curry3(add);
    auto step1 = curried(1);
    auto step2 = step1(2);
    auto result = step2(3);
    std::cout << "柯里化: curried(1)(2)(3) = " << result << "\n";

    // 偏应用：可以一次固定多个
    auto add_to_3 = partial_n(add, 1, 2);
    std::cout << "偏应用: partial_n(add, 1, 2)(3) = " << add_to_3(3) << "\n";

    // 柯里化总是返回函数（直到最后一个参数）
    // 偏应用可能返回函数或值

    std::cout << "\n";
}

// ============================================
// 5. std::bind 对比
// ============================================

void std_bind_demo() {
    std::cout << "=== std::bind 对比 ===\n";

    using namespace std::placeholders;

    auto multiply = [](int a, int b) { return a * b; };
    auto divide = [](double a, double b) { return a / b; };

    // std::bind 固定参数
    auto times5_bind = std::bind(multiply, 5, _1);
    std::cout << "std::bind times5(10) = " << times5_bind(10) << "\n";

    // lambda 版本（推荐）
    auto times5_lambda = [&multiply](int x) { return multiply(5, x); };
    std::cout << "lambda times5(10) = " << times5_lambda(10) << "\n";

    // std::bind 交换参数
    auto flipped_divide = std::bind(divide, _2, _1);
    std::cout << "flipped_divide(2, 10) = " << flipped_divide(2, 10) << " (10/2)\n";

    // 用 lambda 实现 flip
    auto flip = [](auto f) {
        return [f](auto a, auto b) { return f(b, a); };
    };
    auto flipped_div_lambda = flip(divide);
    std::cout << "flip(divide)(2, 10) = " << flipped_div_lambda(2, 10) << "\n\n";
}

// ============================================
// 6. 实际应用：配置工厂
// ============================================

// 日志工厂
[[nodiscard]] auto make_logger(std::string level) {
    return [level](const std::string& message) {
        std::cout << "[" << level << "] " << message << "\n";
    };
}

// 验证器工厂
[[nodiscard]] auto make_range_validator(int min, int max) {
    return [min, max](int value) {
        return value >= min && value <= max;
    };
}

// 格式化器工厂
[[nodiscard]] auto make_formatter(std::string prefix, std::string suffix) {
    return [prefix, suffix](const std::string& content) {
        return prefix + content + suffix;
    };
}

void factory_demo() {
    std::cout << "=== 配置工厂 ===\n";

    // 日志器
    auto log_info = make_logger("INFO");
    auto log_error = make_logger("ERROR");
    auto log_debug = make_logger("DEBUG");

    log_info("Server started");
    log_error("Connection failed");
    log_debug("Processing request");

    // 验证器
    auto is_valid_age = make_range_validator(0, 150);
    auto is_valid_score = make_range_validator(0, 100);
    auto is_valid_month = make_range_validator(1, 12);

    std::cout << "is_valid_age(25) = " << is_valid_age(25) << "\n";
    std::cout << "is_valid_age(200) = " << is_valid_age(200) << "\n";
    std::cout << "is_valid_score(85) = " << is_valid_score(85) << "\n";

    // 格式化器
    auto bold = make_formatter("<b>", "</b>");
    auto italic = make_formatter("<i>", "</i>");
    auto code = make_formatter("`", "`");

    std::cout << bold("Hello") << "\n";
    std::cout << italic("World") << "\n";
    std::cout << code("printf()") << "\n\n";
}

// ============================================
// 7. 实际应用：价格计算
// ============================================

void price_calculation_demo() {
    std::cout << "=== 价格计算 ===\n";

    // 柯里化价格计算
    auto calc_price = curry2([](double tax_rate, double price) {
        return price * (1 + tax_rate);
    });

    auto with_vat = calc_price(0.2);         // 20% 增值税
    auto with_sales_tax = calc_price(0.08);  // 8% 销售税
    auto tax_free = calc_price(0.0);         // 免税

    std::cout << "原价 100:\n";
    std::cout << "  含增值税(20%): " << with_vat(100) << "\n";
    std::cout << "  含销售税(8%): " << with_sales_tax(100) << "\n";
    std::cout << "  免税: " << tax_free(100) << "\n";

    // 折扣计算
    auto apply_discount = curry2([](double discount, double price) {
        return price * (1 - discount);
    });

    auto member_discount = apply_discount(0.1);   // 10% 会员折扣
    auto vip_discount = apply_discount(0.2);      // 20% VIP 折扣
    auto flash_sale = apply_discount(0.5);        // 50% 闪购

    std::cout << "\n原价 200:\n";
    std::cout << "  会员价: " << member_discount(200) << "\n";
    std::cout << "  VIP价: " << vip_discount(200) << "\n";
    std::cout << "  闪购价: " << flash_sale(200) << "\n\n";
}

// ============================================
// 8. 组合柯里化与偏应用
// ============================================

template<typename F, typename G>
[[nodiscard]] auto compose(F f, G g) {
    return [f, g](auto x) { return f(g(x)); };
}

void composition_demo() {
    std::cout << "=== 组合柯里化与偏应用 ===\n";

    // 价格处理管道
    auto apply_discount = [](double rate, double price) {
        return price * (1 - rate);
    };
    auto apply_tax = [](double rate, double price) {
        return price * (1 + rate);
    };
    auto round_cents = [](double price) {
        return std::round(price * 100) / 100.0;
    };

    // 偏应用创建专用函数
    auto discount_10 = partial(apply_discount, 0.1);
    auto add_vat_20 = partial(apply_tax, 0.2);

    // 组合成管道
    auto final_price = compose(round_cents, compose(add_vat_20, discount_10));

    double original = 99.99;
    std::cout << "原价: " << original << "\n";
    std::cout << "打9折后含税: " << final_price(original) << "\n";

    // 另一种写法：pipe (从左到右)
    auto pipe = [](auto f, auto g) {
        return [f, g](auto x) { return g(f(x)); };
    };

    auto final_price2 = pipe(pipe(discount_10, add_vat_20), round_cents);
    std::cout << "验证（pipe）: " << final_price2(original) << "\n\n";
}

// ============================================
// 9. 高级：自动柯里化（简化版）
// ============================================

// 递归柯里化：使用函数模板而非类
template<typename F, typename... CapturedArgs>
[[nodiscard]] auto make_curried(F f, CapturedArgs... captured) {
    return [f, captured...](auto arg) {
        if constexpr (std::is_invocable_v<F, CapturedArgs..., decltype(arg)>) {
            return f(captured..., arg);
        } else {
            return make_curried(f, captured..., arg);
        }
    };
}

template<typename F>
[[nodiscard]] auto auto_curry(F f) {
    return make_curried(f);
}

void auto_curry_demo() {
    std::cout << "=== 自动柯里化 ===\n";

    auto add = [](int a, int b, int c) { return a + b + c; };
    auto curried = auto_curry(add);

    // 逐个传参
    std::cout << "auto_curry(add)(1)(2)(3) = " << curried(1)(2)(3) << "\n";

    // 部分应用
    auto add_1 = curried(1);
    auto add_1_2 = add_1(2);
    std::cout << "curried(1)(2)(10) = " << add_1_2(10) << "\n\n";
}

// ============================================
// main
// ============================================

int main() {
    manual_currying_demo();
    generic_currying_demo();
    partial_application_demo();
    currying_vs_partial_demo();
    std_bind_demo();
    factory_demo();
    price_calculation_demo();
    composition_demo();
    auto_curry_demo();

    return 0;
}
