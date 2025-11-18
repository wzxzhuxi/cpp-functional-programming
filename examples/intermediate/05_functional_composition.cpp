#include <algorithm>
#include <cctype>
#include <cmath>
#include <functional>
#include <iostream>
#include <optional>
#include <string>
#include <variant>
#include <vector>

// ============================================
// 1. 基础 Compose 和 Pipe
// ============================================

template<typename F, typename G>
auto compose(F f, G g) {
    return [f, g](auto x) { return f(g(x)); };
}

template<typename F, typename G>
auto pipe(F f, G g) {
    return [f, g](auto x) { return g(f(x)); };
}

void compose_pipe_demo() {
    std::cout << "=== Compose and Pipe ===\n";

    auto add10 = [](int x) { return x + 10; };
    auto double_it = [](int x) { return x * 2; };
    auto square = [](int x) { return x * x; };

    // Compose: 从右到左
    auto f1 = compose(add10, double_it);
    std::cout << "compose(+10, *2)(5) = " << f1(5)
              << " (先 *2=10, 再 +10=20)\n";

    // Pipe: 从左到右
    auto f2 = pipe(double_it, add10);
    std::cout << "pipe(*2, +10)(5) = " << f2(5)
              << " (先 *2=10, 再 +10=20)\n";

    // 多层组合
    auto f3 = compose(add10, compose(double_it, square));
    std::cout << "compose(+10, compose(*2, ^2))(3) = " << f3(3)
              << " (3 -> 9 -> 18 -> 28)\n\n";
}

// ============================================
// 2. 可变参数 Compose
// ============================================

template<typename F>
auto compose_many(F f) {
    return f;
}

template<typename F, typename... Rest>
auto compose_many(F f, Rest... rest) {
    return [f, rest...](auto x) {
        return f(compose_many(rest...)(x));
    };
}

template<typename F>
auto pipe_many(F f) {
    return f;
}

template<typename F, typename... Rest>
auto pipe_many(F f, Rest... rest) {
    return [f, rest...](auto x) {
        return pipe_many(rest...)(f(x));
    };
}

void variadic_compose_demo() {
    std::cout << "=== Variadic Compose ===\n";

    auto add5 = [](int x) { return x + 5; };
    auto mul3 = [](int x) { return x * 3; };
    auto sub2 = [](int x) { return x - 2; };

    auto f = compose_many(add5, mul3, sub2);
    std::cout << "compose_many(+5, *3, -2)(10) = " << f(10)
              << " (10 -> 8 -> 24 -> 29)\n";

    auto g = pipe_many(sub2, mul3, add5);
    std::cout << "pipe_many(-2, *3, +5)(10) = " << g(10)
              << " (10 -> 8 -> 24 -> 29)\n\n";
}

// ============================================
// 3. 文本处理管道
// ============================================

auto trim = [](std::string s) {
    if (s.empty()) return s;
    size_t start = s.find_first_not_of(" \t\n\r");
    if (start == std::string::npos) return std::string();
    size_t end = s.find_last_not_of(" \t\n\r");
    return s.substr(start, end - start + 1);
};

auto to_upper = [](std::string s) {
    std::transform(s.begin(), s.end(), s.begin(),
                   [](unsigned char c) { return std::toupper(c); });
    return s;
};

auto to_lower = [](std::string s) {
    std::transform(s.begin(), s.end(), s.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return s;
};

auto remove_spaces = [](std::string s) {
    s.erase(std::remove(s.begin(), s.end(), ' '), s.end());
    return s;
};

void text_pipeline_demo() {
    std::cout << "=== Text Processing Pipeline ===\n";

    std::string input = "  Hello World  ";

    // 单独步骤
    std::cout << "Original: \"" << input << "\"\n";
    std::cout << "Trimmed: \"" << trim(input) << "\"\n";
    std::cout << "Upper: \"" << to_upper(trim(input)) << "\"\n";

    // 使用管道
    auto normalize = pipe_many(trim, to_upper, remove_spaces);
    std::cout << "Normalized: \"" << normalize(input) << "\"\n";

    // 不同的管道
    auto to_slug = pipe_many(trim, to_lower, remove_spaces);
    std::cout << "Slug: \"" << to_slug("  My Blog Post  ") << "\"\n\n";
}

// ============================================
// 4. 点自由风格 (Point-Free)
// ============================================

void point_free_demo() {
    std::cout << "=== Point-Free Style ===\n";

    auto increment = [](int x) { return x + 1; };
    auto double_it = [](int x) { return x * 2; };

    // 显式参数 (point-ful)
    auto process_pointful = [=](int x) {
        return double_it(increment(x));
    };

    // 点自由 (point-free)
    auto process_pointfree = compose(double_it, increment);

    std::cout << "Point-ful: " << process_pointful(5) << "\n";
    std::cout << "Point-free: " << process_pointfree(5) << "\n";

    // 更复杂的例子
    std::vector<int> nums = {1, 2, 3, 4, 5};

    // 点自由风格的 map
    auto transform_pointfree = [](auto f) {
        return [f](const auto& vec) {
            std::vector<std::decay_t<decltype(vec[0])>> result;
            result.reserve(vec.size());
            std::transform(vec.begin(), vec.end(),
                         std::back_inserter(result), f);
            return result;
        };
    };

    auto doubled = transform_pointfree(double_it)(nums);
    std::cout << "Doubled: ";
    for (int x : doubled) std::cout << x << " ";
    std::cout << "\n\n";
}

// ============================================
// 5. 组合子 (Combinators)
// ============================================

// K 组合子：常量函数
template<typename T>
auto K(T value) {
    return [value](auto...) { return value; };
}

// I 组合子：恒等函数
auto I = [](auto x) { return x; };

// flip：翻转参数
template<typename F>
auto flip(F f) {
    return [f](auto a, auto b) { return f(b, a); };
}

// on：在应用前转换
template<typename F, typename G>
auto on(F f, G g) {
    return [f, g](auto a, auto b) {
        return f(g(a), g(b));
    };
}

void combinators_demo() {
    std::cout << "=== Combinators ===\n";

    // K 组合子
    auto always_42 = K(42);
    std::cout << "K(42)(1, 2, 3) = " << always_42(1, 2, 3) << "\n";

    // I 组合子
    std::cout << "I(100) = " << I(100) << "\n";

    // flip
    auto subtract = [](int a, int b) { return a - b; };
    auto reversed_sub = flip(subtract);
    std::cout << "subtract(10, 3) = " << subtract(10, 3) << "\n";
    std::cout << "flip(subtract)(10, 3) = " << reversed_sub(10, 3) << "\n";

    // on
    auto compare_length = on(
        [](size_t a, size_t b) { return a < b; },
        [](const std::string& s) { return s.length(); }
    );
    std::cout << "compare_length(\"hi\", \"hello\") = "
              << std::boolalpha << compare_length("hi", "hello") << "\n\n";
}

// ============================================
// 6. 实际应用：价格计算管道
// ============================================

auto apply_discount = [](double rate) {
    return [rate](double price) {
        return price * (1.0 - rate);
    };
};

auto apply_tax = [](double rate) {
    return [rate](double price) {
        return price * (1.0 + rate);
    };
};

auto round_to_cents = [](double price) {
    return std::round(price * 100.0) / 100.0;
};

void price_pipeline_demo() {
    std::cout << "=== Price Calculation Pipeline ===\n";

    double original = 100.0;

    // 逐步计算
    double after_discount = apply_discount(0.1)(original);
    double after_tax = apply_tax(0.2)(after_discount);
    double final = round_to_cents(after_tax);

    std::cout << "Original: $" << original << "\n";
    std::cout << "After 10% discount: $" << after_discount << "\n";
    std::cout << "After 20% tax: $" << after_tax << "\n";
    std::cout << "Rounded: $" << final << "\n";

    // 使用管道
    auto calculate_price = pipe_many(
        apply_discount(0.1),
        apply_tax(0.2),
        round_to_cents
    );

    std::cout << "\nUsing pipeline: $" << calculate_price(100.0) << "\n\n";
}

// ============================================
// 7. Optional 链式组合
// ============================================

template<typename T, typename F>
auto fmap(F f) {
    return [f](const std::optional<T>& opt) -> std::optional<std::decay_t<decltype(f(std::declval<T>()))>> {
        if (!opt) return std::nullopt;
        return f(*opt);
    };
}

void optional_composition_demo() {
    std::cout << "=== Optional Composition ===\n";

    auto safe_div = [](int a, int b) -> std::optional<int> {
        if (b == 0) return std::nullopt;
        return a / b;
    };

    auto safe_sqrt = [](int x) -> std::optional<double> {
        if (x < 0) return std::nullopt;
        return std::sqrt(x);
    };

    // 手动链式
    auto result1 = safe_div(100, 5);
    if (result1) {
        auto result2 = safe_sqrt(*result1);
        if (result2) {
            std::cout << "Manual chain: " << *result2 << "\n";
        }
    }

    // 使用 and_then (C++23) 或手动实现
    auto safe_operation = [&](int a, int b) -> std::optional<double> {
        auto div_result = safe_div(a, b);
        if (!div_result) return std::nullopt;
        return safe_sqrt(*div_result);
    };

    auto r1 = safe_operation(100, 5);
    if (r1) std::cout << "Composed: " << *r1 << "\n";

    auto r2 = safe_operation(100, 0);
    std::cout << "Failed (div by 0): "
              << (r2 ? std::to_string(*r2) : "nullopt") << "\n\n";
}

// ============================================
// 8. 函数容器
// ============================================

void function_container_demo() {
    std::cout << "=== Function Container ===\n";

    std::vector<std::function<int(int)>> transformations;

    transformations.push_back([](int x) { return x + 10; });
    transformations.push_back([](int x) { return x * 2; });
    transformations.push_back([](int x) { return x - 5; });

    // 应用所有转换
    int value = 5;
    std::cout << "Initial: " << value << "\n";

    for (const auto& f : transformations) {
        value = f(value);
        std::cout << "After transformation: " << value << "\n";
    }

    // 组合所有函数
    auto combined = [&](int x) {
        for (const auto& f : transformations) {
            x = f(x);
        }
        return x;
    };

    std::cout << "Combined(5) = " << combined(5) << "\n\n";
}

// ============================================
// Main
// ============================================

int main() {
    compose_pipe_demo();
    variadic_compose_demo();
    text_pipeline_demo();
    point_free_demo();
    combinators_demo();
    price_pipeline_demo();
    optional_composition_demo();
    function_container_demo();

    return 0;
}
