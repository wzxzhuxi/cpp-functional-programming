/**
 * @file solutions.cpp
 * @brief 第06章练习题参考答案
 */

#include <algorithm>
#include <cassert>
#include <functional>
#include <iostream>
#include <optional>
#include <string>
#include <vector>

// 基础工具
template<typename F, typename G>
[[nodiscard]] auto compose(F f, G g) {
    return [f, g](auto x) { return f(g(x)); };
}

template<typename F, typename G>
[[nodiscard]] auto pipe(F f, G g) {
    return [f, g](auto x) { return g(f(x)); };
}

// ============================================
// 练习 1: 实现 compose_all - 答案
// ============================================

template<typename T>
[[nodiscard]] std::function<T(T)> compose_all(
    const std::vector<std::function<T(T)>>& funcs
) {
    // 空向量返回恒等函数
    if (funcs.empty()) {
        return [](T x) { return x; };
    }

    // 从右到左组合
    return [funcs](T x) {
        T result = x;
        // 从后向前应用函数
        for (auto it = funcs.rbegin(); it != funcs.rend(); ++it) {
            result = (*it)(result);
        }
        return result;
    };
}

// 替代实现：使用 fold
template<typename T>
[[nodiscard]] std::function<T(T)> compose_all_fold(
    const std::vector<std::function<T(T)>>& funcs
) {
    std::function<T(T)> result = [](T x) { return x; };

    for (auto it = funcs.rbegin(); it != funcs.rend(); ++it) {
        auto f = *it;
        result = [f, result](T x) { return f(result(x)); };
    }

    return result;
}

void test_exercise_1() {
    std::cout << "=== 练习 1: 实现 compose_all - 答案 ===\n";

    std::vector<std::function<int(int)>> funcs = {
        [](int x) { return x + 1; },
        [](int x) { return x * 2; },
        [](int x) { return x * x; }
    };

    auto combined = compose_all(funcs);
    int result = combined(3);

    std::cout << "compose_all({add1, double, square})(3) = " << result << "\n";
    std::cout << "  = add1(double(square(3)))\n";
    std::cout << "  = add1(double(9))\n";
    std::cout << "  = add1(18)\n";
    std::cout << "  = 19\n";
    assert(result == 19);

    // 空向量测试
    std::vector<std::function<int(int)>> empty;
    auto identity = compose_all(empty);
    assert(identity(42) == 42);

    std::cout << "compose_all 测试通过\n\n";
}

// ============================================
// 练习 2: 实现 tap - 答案
// ============================================

template<typename F>
[[nodiscard]] auto tap(F f) {
    return [f](auto x) {
        f(x);     // 执行副作用
        return x; // 返回原值
    };
}

void test_exercise_2() {
    std::cout << "=== 练习 2: 实现 tap - 答案 ===\n";

    int side_effect_count = 0;

    auto add10 = [](int x) { return x + 10; };
    auto double_it = [](int x) { return x * 2; };
    auto log = [&side_effect_count](int x) {
        std::cout << "  中间值: " << x << "\n";
        side_effect_count++;
    };

    auto pipeline = compose(add10, compose(tap(log), double_it));

    std::cout << "执行 pipeline(5):\n";
    int result = pipeline(5);

    std::cout << "结果: " << result << "\n";
    assert(result == 20);
    assert(side_effect_count == 1);

    // tap 的实际应用：调试管道
    auto debug_pipeline = [](auto x) {
        auto step1 = [](int v) { return v * 2; };
        auto step2 = [](int v) { return v + 10; };
        auto step3 = [](int v) { return v * v; };

        auto traced = compose(
            step3,
            compose(tap([](int v) { std::cout << "  step2 结果: " << v << "\n"; }),
            compose(step2,
            compose(tap([](int v) { std::cout << "  step1 结果: " << v << "\n"; }),
            step1)))
        );

        return traced(x);
    };

    std::cout << "\n调试管道(3):\n";
    std::cout << "  最终结果: " << debug_pipeline(3) << "\n";

    std::cout << "tap 测试通过\n\n";
}

// ============================================
// 练习 3: Markdown 格式化器 - 答案
// ============================================

[[nodiscard]] std::string bold(const std::string& text) {
    return "**" + text + "**";
}

[[nodiscard]] std::string italic(const std::string& text) {
    return "*" + text + "*";
}

[[nodiscard]] std::string code(const std::string& text) {
    return "`" + text + "`";
}

[[nodiscard]] auto link(const std::string& url) {
    return [url](const std::string& text) -> std::string {
        return "[" + text + "](" + url + ")";
    };
}

[[nodiscard]] auto heading(int level) {
    return [level](const std::string& text) -> std::string {
        return std::string(level, '#') + " " + text;
    };
}

// 额外：更多格式化函数
[[nodiscard]] std::string strikethrough(const std::string& text) {
    return "~~" + text + "~~";
}

[[nodiscard]] std::string blockquote(const std::string& text) {
    return "> " + text;
}

[[nodiscard]] auto list_item(int indent = 0) {
    return [indent](const std::string& text) -> std::string {
        return std::string(indent * 2, ' ') + "- " + text;
    };
}

void test_exercise_3() {
    std::cout << "=== 练习 3: Markdown 格式化器 - 答案 ===\n";

    assert(bold("Hello") == "**Hello**");
    assert(italic("World") == "*World*");
    assert(code("printf()") == "`printf()`");
    assert(link("https://example.com")("Click") == "[Click](https://example.com)");
    assert(heading(1)("Title") == "# Title");
    assert(heading(2)("Subtitle") == "## Subtitle");

    std::cout << "基础测试通过\n";

    // 组合使用
    auto make_link = link("https://github.com");

    // 加粗的链接
    auto bold_link = compose(bold, make_link);
    std::cout << "bold(link(GitHub)) = " << bold_link("GitHub") << "\n";

    // 斜体的代码
    auto italic_code = compose(italic, code);
    std::cout << "italic(code(main)) = " << italic_code("main") << "\n";

    // 复杂组合：加粗的斜体链接
    auto fancy_link = compose(bold, compose(italic, make_link));
    std::cout << "bold(italic(link)) = " << fancy_link("Fancy") << "\n";

    std::cout << "Markdown 格式化器 测试通过\n\n";
}

// ============================================
// 练习 4: 验证器组合 - 答案
// ============================================

template<typename T>
using Validator = std::function<std::optional<std::string>(const T&)>;

template<typename T>
[[nodiscard]] Validator<T> validate_and(Validator<T> v1, Validator<T> v2) {
    return [v1, v2](const T& value) -> std::optional<std::string> {
        // v1 失败则返回 v1 错误
        if (auto err = v1(value)) {
            return err;
        }
        // v1 通过则返回 v2 结果
        return v2(value);
    };
}

template<typename T>
[[nodiscard]] Validator<T> validate_or(Validator<T> v1, Validator<T> v2) {
    return [v1, v2](const T& value) -> std::optional<std::string> {
        // v1 通过则直接通过
        if (!v1(value)) {
            return std::nullopt;
        }
        // v1 失败，尝试 v2
        return v2(value);
    };
}

// 额外：更多验证器组合
template<typename T>
[[nodiscard]] Validator<T> validate_not(Validator<T> v, std::string error) {
    return [v, error](const T& value) -> std::optional<std::string> {
        if (!v(value)) {  // 原验证器通过
            return error;  // 取反失败
        }
        return std::nullopt;  // 原验证器失败，取反通过
    };
}

template<typename T, typename... Vs>
[[nodiscard]] Validator<T> validate_all(Vs... validators) {
    return [validators...](const T& value) -> std::optional<std::string> {
        std::optional<std::string> result;
        (
            [&](auto v) {
                if (!result && v(value)) {
                    result = v(value);
                }
            }(validators),
            ...
        );
        return result;
    };
}

void test_exercise_4() {
    std::cout << "=== 练习 4: 验证器组合 - 答案 ===\n";

    Validator<int> is_positive = [](int x) -> std::optional<std::string> {
        if (x <= 0) return "必须为正数";
        return std::nullopt;
    };

    Validator<int> is_even = [](int x) -> std::optional<std::string> {
        if (x % 2 != 0) return "必须为偶数";
        return std::nullopt;
    };

    Validator<int> less_than_100 = [](int x) -> std::optional<std::string> {
        if (x >= 100) return "必须小于100";
        return std::nullopt;
    };

    // validate_and 测试
    auto positive_and_even = validate_and(is_positive, is_even);

    assert(!positive_and_even(4).has_value());
    assert(positive_and_even(-4).has_value());
    assert(positive_and_even(3).has_value());

    std::cout << "validate_and(positive, even)(4) = 通过\n";
    std::cout << "validate_and(positive, even)(-4) = "
              << positive_and_even(-4).value() << "\n";
    std::cout << "validate_and(positive, even)(3) = "
              << positive_and_even(3).value() << "\n";

    // validate_or 测试
    auto positive_or_even = validate_or(is_positive, is_even);

    assert(!positive_or_even(4).has_value());
    assert(!positive_or_even(3).has_value());
    assert(!positive_or_even(-4).has_value());
    assert(positive_or_even(-3).has_value());

    std::cout << "validate_or(positive, even)(-3) = "
              << positive_or_even(-3).value() << "\n";

    // 复杂组合
    auto valid_score = validate_and(
        validate_and(is_positive, is_even),
        less_than_100
    );

    std::cout << "valid_score(50) = "
              << (valid_score(50) ? "失败" : "通过") << "\n";
    std::cout << "valid_score(150) = "
              << valid_score(150).value_or("通过") << "\n";

    std::cout << "验证器组合 测试通过\n\n";
}

// ============================================
// 练习 5: 实现 juxt - 答案
// ============================================

template<typename... Fs>
[[nodiscard]] auto juxt(Fs... fs) {
    return [fs...](auto x) {
        using R = std::common_type_t<decltype(fs(x))...>;
        return std::vector<R>{fs(x)...};
    };
}

// 返回 tuple 的版本（保留类型信息）
template<typename... Fs>
[[nodiscard]] auto juxt_tuple(Fs... fs) {
    return [fs...](auto x) {
        return std::make_tuple(fs(x)...);
    };
}

void test_exercise_5() {
    std::cout << "=== 练习 5: 实现 juxt - 答案 ===\n";

    auto add1 = [](int x) { return x + 1; };
    auto double_it = [](int x) { return x * 2; };
    auto square = [](int x) { return x * x; };

    auto analyze = juxt(add1, double_it, square);
    auto results = analyze(5);

    assert(results.size() == 3);
    assert(results[0] == 6);
    assert(results[1] == 10);
    assert(results[2] == 25);

    std::cout << "juxt(add1, double, square)(5) = {";
    for (size_t i = 0; i < results.size(); ++i) {
        if (i > 0) std::cout << ", ";
        std::cout << results[i];
    }
    std::cout << "}\n";

    // tuple 版本
    auto analyze_tuple = juxt_tuple(add1, double_it, square);
    auto [a, b, c] = analyze_tuple(5);
    std::cout << "juxt_tuple(5) = (" << a << ", " << b << ", " << c << ")\n";

    // 实际应用：统计分析
    auto min_fn = [](const std::vector<int>& v) {
        return *std::min_element(v.begin(), v.end());
    };
    auto max_fn = [](const std::vector<int>& v) {
        return *std::max_element(v.begin(), v.end());
    };
    auto sum_fn = [](const std::vector<int>& v) {
        int sum = 0;
        for (int x : v) sum += x;
        return sum;
    };

    auto stats = juxt(min_fn, max_fn, sum_fn);
    std::vector<int> data = {3, 1, 4, 1, 5, 9, 2, 6};
    auto [min_v, max_v, sum_v] = [&]() {
        auto r = stats(data);
        return std::make_tuple(r[0], r[1], r[2]);
    }();

    std::cout << "stats({3,1,4,1,5,9,2,6}) = min=" << min_v
              << ", max=" << max_v << ", sum=" << sum_v << "\n";

    std::cout << "juxt 测试通过\n\n";
}

// ============================================
// 额外示例：更多组合子
// ============================================

// converge：应用多个函数到同一输入，然后汇聚结果
template<typename Combine, typename... Fs>
[[nodiscard]] auto converge(Combine combine, Fs... fs) {
    return [combine, fs...](auto x) {
        return combine(fs(x)...);
    };
}

// ap：应用多个函数到多个参数（笛卡尔积）
template<typename F>
[[nodiscard]] auto ap(F f, auto... args) {
    return std::vector{f(args)...};
}

void bonus_demo() {
    std::cout << "=== 额外示例：更多组合子 ===\n";

    // converge：计算平均值
    auto sum = [](int a, int b) { return a + b; };
    auto length = [](const std::vector<int>& v) { return static_cast<int>(v.size()); };
    auto total = [](const std::vector<int>& v) {
        int s = 0;
        for (int x : v) s += x;
        return s;
    };

    auto divide = [](int a, int b) { return static_cast<double>(a) / b; };
    auto average = converge(divide, total, length);

    std::vector<int> data = {1, 2, 3, 4, 5};
    std::cout << "average({1,2,3,4,5}) = " << average(data) << "\n";

    // 另一个 converge 示例：范围
    auto range = converge(
        [](int min, int max) { return max - min; },
        [](const std::vector<int>& v) { return *std::min_element(v.begin(), v.end()); },
        [](const std::vector<int>& v) { return *std::max_element(v.begin(), v.end()); }
    );

    std::cout << "range({1,2,3,4,5}) = " << range(data) << "\n\n";
}

// ============================================
// main
// ============================================

int main() {
    std::cout << "========================================\n";
    std::cout << "    第06章练习参考答案\n";
    std::cout << "========================================\n\n";

    test_exercise_1();
    test_exercise_2();
    test_exercise_3();
    test_exercise_4();
    test_exercise_5();
    bonus_demo();

    std::cout << "========================================\n";
    std::cout << "    所有练习完成!\n";
    std::cout << "========================================\n";

    return 0;
}
