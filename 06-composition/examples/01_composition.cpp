/**
 * @file 01_composition.cpp
 * @brief 函数组合示例
 *
 * 本文件演示：
 * - compose 和 pipe
 * - 多函数组合
 * - 组合子
 * - 点自由风格
 * - 实际应用
 */

#include <algorithm>
#include <cmath>
#include <functional>
#include <iostream>
#include <optional>
#include <string>
#include <vector>

// ============================================
// 1. 基本组合：compose 和 pipe
// ============================================

template<typename F, typename G>
[[nodiscard]] auto compose(F f, G g) {
    return [f, g](auto x) { return f(g(x)); };
}

template<typename F, typename G>
[[nodiscard]] auto pipe(F f, G g) {
    return [f, g](auto x) { return g(f(x)); };
}

void basic_composition_demo() {
    std::cout << "=== 基本组合 ===\n";

    auto add10 = [](int x) { return x + 10; };
    auto double_it = [](int x) { return x * 2; };
    auto square = [](int x) { return x * x; };

    // compose: 从右到左 f(g(x))
    auto composed = compose(add10, double_it);
    std::cout << "compose(add10, double)(5) = " << composed(5)
              << " (add10(double(5)) = add10(10) = 20)\n";

    // pipe: 从左到右 g(f(x))
    auto piped = pipe(double_it, add10);
    std::cout << "pipe(double, add10)(5) = " << piped(5)
              << " (add10(double(5)) = 20)\n";

    // 两者效果相同，只是参数顺序不同
    auto same1 = compose(add10, double_it);
    auto same2 = pipe(double_it, add10);
    std::cout << "compose(a,b)(5) == pipe(b,a)(5): "
              << (same1(5) == same2(5) ? "true" : "false") << "\n\n";
}

// ============================================
// 2. 多函数组合
// ============================================

// compose_n: 可变参数版本
template<typename F>
[[nodiscard]] auto compose_n(F f) {
    return f;
}

template<typename F, typename G, typename... Rest>
[[nodiscard]] auto compose_n(F f, G g, Rest... rest) {
    return compose(f, compose_n(g, rest...));
}

// pipe_n: 可变参数版本
template<typename F>
[[nodiscard]] auto pipe_n(F f) {
    return f;
}

template<typename F, typename G, typename... Rest>
[[nodiscard]] auto pipe_n(F f, G g, Rest... rest) {
    return pipe_n(pipe(f, g), rest...);
}

void multi_composition_demo() {
    std::cout << "=== 多函数组合 ===\n";

    auto add10 = [](int x) { return x + 10; };
    auto double_it = [](int x) { return x * 2; };
    auto square = [](int x) { return x * x; };

    // compose_n: 从右到左
    auto f1 = compose_n(add10, double_it, square);
    std::cout << "compose_n(add10, double, square)(3) = " << f1(3)
              << " (add10(double(9)) = add10(18) = 28)\n";

    // pipe_n: 从左到右
    auto f2 = pipe_n(square, double_it, add10);
    std::cout << "pipe_n(square, double, add10)(3) = " << f2(3)
              << " (相同结果)\n";

    // 更长的链
    auto negate = [](int x) { return -x; };
    auto abs_val = [](int x) { return std::abs(x); };

    auto complex = pipe_n(square, negate, abs_val, double_it, add10);
    std::cout << "complex pipeline(3) = " << complex(3)
              << " (9 -> -9 -> 9 -> 18 -> 28)\n\n";
}

// ============================================
// 3. 组合子
// ============================================

// I 组合子：恒等函数
auto identity = [](auto x) { return x; };

// K 组合子：常量函数
template<typename T>
[[nodiscard]] auto constant(T value) {
    return [value](auto...) { return value; };
}

// flip：翻转参数
template<typename F>
[[nodiscard]] auto flip(F f) {
    return [f](auto a, auto b) { return f(b, a); };
}

// on：转换后应用
template<typename F, typename G>
[[nodiscard]] auto on(F f, G g) {
    return [f, g](auto a, auto b) { return f(g(a), g(b)); };
}

// both：两个谓词都满足
template<typename P, typename Q>
[[nodiscard]] auto both(P p, Q q) {
    return [p, q](auto x) { return p(x) && q(x); };
}

// either：任一谓词满足
template<typename P, typename Q>
[[nodiscard]] auto either(P p, Q q) {
    return [p, q](auto x) { return p(x) || q(x); };
}

// complement：谓词取反
template<typename P>
[[nodiscard]] auto complement(P p) {
    return [p](auto x) { return !p(x); };
}

void combinators_demo() {
    std::cout << "=== 组合子 ===\n";

    // identity
    std::cout << "identity(42) = " << identity(42) << "\n";

    // constant
    auto always_42 = constant(42);
    std::cout << "constant(42)(anything) = " << always_42("ignored") << "\n";

    // flip
    auto subtract = [](int a, int b) { return a - b; };
    auto flipped = flip(subtract);
    std::cout << "subtract(10, 3) = " << subtract(10, 3) << "\n";
    std::cout << "flip(subtract)(10, 3) = " << flipped(10, 3) << " (3 - 10)\n";

    // on
    auto length = [](const std::string& s) { return s.length(); };
    auto less = [](size_t a, size_t b) { return a < b; };
    auto compare_by_length = on(less, length);
    std::cout << "compare_by_length(\"hi\", \"hello\") = "
              << compare_by_length("hi", "hello") << " (2 < 5)\n";

    // both / either
    auto is_positive = [](int x) { return x > 0; };
    auto is_even = [](int x) { return x % 2 == 0; };

    auto is_positive_even = both(is_positive, is_even);
    auto is_positive_or_even = either(is_positive, is_even);

    std::cout << "both(positive, even)(4) = " << is_positive_even(4) << "\n";
    std::cout << "both(positive, even)(-2) = " << is_positive_even(-2) << "\n";
    std::cout << "either(positive, even)(-2) = " << is_positive_or_even(-2) << "\n";

    // complement
    auto is_odd = complement(is_even);
    std::cout << "complement(is_even)(5) = " << is_odd(5) << "\n\n";
}

// ============================================
// 4. 点自由风格
// ============================================

void point_free_demo() {
    std::cout << "=== 点自由风格 ===\n";

    auto add10 = [](int x) { return x + 10; };
    auto double_it = [](int x) { return x * 2; };

    // Point-ful (显式参数)
    auto process_pointful = [&](int x) {
        return add10(double_it(x));
    };

    // Point-free (无显式参数)
    auto process_pointfree = compose(add10, double_it);

    std::cout << "point-ful(5) = " << process_pointful(5) << "\n";
    std::cout << "point-free(5) = " << process_pointfree(5) << "\n";

    // 更复杂的例子
    auto is_positive = [](int x) { return x > 0; };
    auto is_less_than_100 = [](int x) { return x < 100; };

    // Point-ful
    auto valid_score_pf = [&](int x) {
        return is_positive(x) && is_less_than_100(x);
    };

    // Point-free
    auto valid_score_pfree = both(is_positive, is_less_than_100);

    std::cout << "valid_score(50) = " << valid_score_pf(50) << "\n";
    std::cout << "valid_score(150) = " << valid_score_pfree(150) << "\n\n";
}

// ============================================
// 5. 实际应用：文本处理
// ============================================

[[nodiscard]] auto trim(std::string s) {
    auto start = s.find_first_not_of(" \t\n\r");
    if (start == std::string::npos) return std::string{};
    auto end = s.find_last_not_of(" \t\n\r");
    return s.substr(start, end - start + 1);
}

[[nodiscard]] auto to_upper(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(),
                   [](unsigned char c) { return std::toupper(c); });
    return s;
}

[[nodiscard]] auto to_lower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return s;
}

[[nodiscard]] auto remove_spaces(std::string s) {
    s.erase(std::remove(s.begin(), s.end(), ' '), s.end());
    return s;
}

[[nodiscard]] auto replace_char(char from, char to) {
    return [from, to](std::string s) {
        std::replace(s.begin(), s.end(), from, to);
        return s;
    };
}

void text_processing_demo() {
    std::cout << "=== 文本处理管道 ===\n";

    // 组合文本处理函数
    auto normalize = pipe_n(trim, to_upper, remove_spaces);
    auto slug = pipe_n(trim, to_lower, replace_char(' ', '-'));

    std::string input = "  Hello World  ";

    std::cout << "输入: \"" << input << "\"\n";
    std::cout << "normalize: \"" << normalize(input) << "\"\n";
    std::cout << "slug: \"" << slug(input) << "\"\n\n";
}

// ============================================
// 6. 实际应用：价格计算
// ============================================

[[nodiscard]] auto apply_discount(double rate) {
    return [rate](double price) {
        return price * (1.0 - rate);
    };
}

[[nodiscard]] auto apply_tax(double rate) {
    return [rate](double price) {
        return price * (1.0 + rate);
    };
}

[[nodiscard]] auto round_cents(double price) {
    return std::round(price * 100) / 100.0;
}

void price_calculation_demo() {
    std::cout << "=== 价格计算管道 ===\n";

    // 普通价格计算
    auto standard_price = pipe_n(
        apply_tax(0.2),   // 20% 税
        round_cents
    );

    // 会员价格计算
    auto member_price = pipe_n(
        apply_discount(0.1),  // 10% 折扣
        apply_tax(0.2),       // 20% 税
        round_cents
    );

    // VIP 价格计算
    auto vip_price = pipe_n(
        apply_discount(0.2),  // 20% 折扣
        apply_tax(0.2),       // 20% 税
        round_cents
    );

    double original = 99.99;

    std::cout << "原价: " << original << "\n";
    std::cout << "标准价(+20%税): " << standard_price(original) << "\n";
    std::cout << "会员价(-10%+20%税): " << member_price(original) << "\n";
    std::cout << "VIP价(-20%+20%税): " << vip_price(original) << "\n\n";
}

// ============================================
// 7. 实际应用：数据验证
// ============================================

template<typename T>
using ValidationResult = std::optional<std::string>;

template<typename T, typename Pred>
[[nodiscard]] auto make_validator(Pred pred, std::string error) {
    return [pred, error](const T& value) -> ValidationResult<T> {
        if (!pred(value)) {
            return error;
        }
        return std::nullopt;
    };
}

// 组合验证器
template<typename T, typename... Validators>
[[nodiscard]] auto validate_all(Validators... validators) {
    return [validators...](const T& value) -> std::vector<std::string> {
        std::vector<std::string> errors;
        (
            [&](auto validator) {
                if (auto err = validator(value)) {
                    errors.push_back(*err);
                }
            }(validators),
            ...
        );
        return errors;
    };
}

void validation_demo() {
    std::cout << "=== 数据验证管道 ===\n";

    // 创建验证器
    auto not_empty = make_validator<std::string>(
        [](const std::string& s) { return !s.empty(); },
        "不能为空"
    );

    auto max_length = [](size_t n) {
        return make_validator<std::string>(
            [n](const std::string& s) { return s.length() <= n; },
            "长度不能超过 " + std::to_string(n)
        );
    };

    auto no_spaces = make_validator<std::string>(
        [](const std::string& s) {
            return s.find(' ') == std::string::npos;
        },
        "不能包含空格"
    );

    // 组合验证器
    auto validate_username = validate_all<std::string>(
        not_empty,
        max_length(20),
        no_spaces
    );

    // 测试
    auto test = [&](const std::string& username) {
        auto errors = validate_username(username);
        std::cout << "\"" << username << "\": ";
        if (errors.empty()) {
            std::cout << "有效\n";
        } else {
            std::cout << "无效 - ";
            for (const auto& e : errors) std::cout << e << "; ";
            std::cout << "\n";
        }
    };

    test("alice");
    test("");
    test("this_is_a_very_long_username");
    test("john doe");

    std::cout << "\n";
}

// ============================================
// 8. 条件组合
// ============================================

// when：条件执行
template<typename Pred, typename F>
[[nodiscard]] auto when(Pred pred, F f) {
    return [pred, f](auto x) {
        if (pred(x)) {
            return f(x);
        }
        return x;
    };
}

// unless：条件不满足时执行
template<typename Pred, typename F>
[[nodiscard]] auto unless(Pred pred, F f) {
    return when(complement(pred), f);
}

// if_else：条件分支
template<typename Pred, typename Then, typename Else>
[[nodiscard]] auto if_else(Pred pred, Then then_fn, Else else_fn) {
    return [pred, then_fn, else_fn](auto x) {
        if (pred(x)) {
            return then_fn(x);
        }
        return else_fn(x);
    };
}

void conditional_demo() {
    std::cout << "=== 条件组合 ===\n";

    auto is_negative = [](int x) { return x < 0; };
    auto negate = [](int x) { return -x; };
    auto double_it = [](int x) { return x * 2; };

    // when: 仅在条件满足时应用
    auto abs_value = when(is_negative, negate);
    std::cout << "abs(-5) = " << abs_value(-5) << "\n";
    std::cout << "abs(5) = " << abs_value(5) << "\n";

    // if_else: 条件分支
    auto process = if_else(
        is_negative,
        negate,     // 负数取反
        double_it   // 正数翻倍
    );
    std::cout << "process(-5) = " << process(-5) << " (negate)\n";
    std::cout << "process(5) = " << process(5) << " (double)\n\n";
}

// ============================================
// main
// ============================================

int main() {
    basic_composition_demo();
    multi_composition_demo();
    combinators_demo();
    point_free_demo();
    text_processing_demo();
    price_calculation_demo();
    validation_demo();
    conditional_demo();

    return 0;
}
