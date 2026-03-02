/**
 * @file exercises.cpp
 * @brief 第06章练习题：函数组合
 *
 * 完成下面的练习，然后运行程序验证结果。
 * 参考答案在 solutions.cpp
 */

#include <algorithm>
#include <cassert>
#include <functional>
#include <iostream>
#include <string>
#include <type_traits>
#include <vector>

// 提供的基础工具
template<typename F, typename G>
[[nodiscard]] auto compose(F f, G g) {
    return [f, g](auto x) { return f(g(x)); };
}

template<typename F, typename G>
[[nodiscard]] auto pipe(F f, G g) {
    return [f, g](auto x) { return g(f(x)); };
}

// ============================================
// 练习 1: 实现 compose_all
// ============================================

/**
 * compose_all: 接受函数向量，返回组合后的函数
 *
 * 示例：
 *   std::vector<std::function<int(int)>> funcs = {add1, double_it, square};
 *   compose_all(funcs)(3) = add1(double_it(square(3)))
 *
 * 注意：空向量应返回恒等函数
 */
template<typename T>
[[nodiscard]] std::function<T(T)> compose_all(
    const std::vector<std::function<T(T)>>& funcs
) {
    // TODO: 实现
    return [](T x) { return x; };
}

void test_exercise_1() {
    std::cout << "=== 练习 1: 实现 compose_all ===\n";

    std::vector<std::function<int(int)>> funcs = {
        [](int x) { return x + 1; },      // add1
        [](int x) { return x * 2; },      // double
        [](int x) { return x * x; }       // square
    };

    auto combined = compose_all(funcs);
    // compose_all({add1, double, square})(3)
    // = add1(double(square(3)))
    // = add1(double(9))
    // = add1(18)
    // = 19

    int result = combined(3);
    std::cout << "compose_all({add1, double, square})(3) = " << result << "\n";
    assert(result == 19);

    // 空向量应返回恒等函数
    std::vector<std::function<int(int)>> empty;
    auto identity = compose_all(empty);
    assert(identity(42) == 42);

    std::cout << "compose_all 测试通过\n\n";
}

// ============================================
// 练习 2: 实现 tap 组合子
// ============================================

/**
 * tap: 在管道中执行副作用，但不改变值
 *
 * 用于调试：
 *   pipe_n(
 *       transform,
 *       tap([](auto x) { std::cout << "中间值: " << x; }),
 *       filter
 *   )
 */
template<typename F>
[[nodiscard]] auto tap(F f) {
    // TODO: 实现
    // 调用 f(x) 但返回 x 本身
    return [f](auto x) {
        return x;  // 修改这行
    };
}

void test_exercise_2() {
    std::cout << "=== 练习 2: 实现 tap ===\n";

    int side_effect_count = 0;

    auto add10 = [](int x) { return x + 10; };
    auto double_it = [](int x) { return x * 2; };
    auto log = [&side_effect_count](int x) {
        std::cout << "  中间值: " << x << "\n";
        side_effect_count++;
    };

    auto pipeline = compose(add10, compose(tap(log), double_it));
    // double -> tap(log) -> add10

    std::cout << "执行 pipeline(5):\n";
    int result = pipeline(5);  // double(5)=10 -> log(10) -> add10(10)=20

    std::cout << "结果: " << result << "\n";
    assert(result == 20);
    assert(side_effect_count == 1);

    std::cout << "tap 测试通过\n\n";
}

// ============================================
// 练习 3: Markdown 格式化器
// ============================================

/**
 * 使用函数组合构建 Markdown 格式化器
 *
 * 实现以下格式化函数，然后组合它们
 */

// TODO: 实现 bold (加粗: **text**)
[[nodiscard]] std::string bold(const std::string& text) {
    return text;  // 修改这行
}

// TODO: 实现 italic (斜体: *text*)
[[nodiscard]] std::string italic(const std::string& text) {
    return text;  // 修改这行
}

// TODO: 实现 code (代码: `text`)
[[nodiscard]] std::string code(const std::string& text) {
    return text;  // 修改这行
}

// TODO: 实现 link (链接: [text](url))
[[nodiscard]] auto link(const std::string& url) {
    return [url](const std::string& text) -> std::string {
        return text;  // 修改这行
    };
}

// TODO: 实现 heading (标题: # text, ## text, etc.)
[[nodiscard]] auto heading(int level) {
    return [level](const std::string& text) -> std::string {
        return text;  // 修改这行
    };
}

void test_exercise_3() {
    std::cout << "=== 练习 3: Markdown 格式化器 ===\n";

    assert(bold("Hello") == "**Hello**");
    assert(italic("World") == "*World*");
    assert(code("printf()") == "`printf()`");
    assert(link("https://example.com")("Click") == "[Click](https://example.com)");
    assert(heading(1)("Title") == "# Title");
    assert(heading(2)("Subtitle") == "## Subtitle");

    // 组合使用
    auto make_title = heading(1);
    auto make_link = link("https://github.com");

    // 组合：加粗的链接
    auto bold_link = compose(bold, make_link);
    std::cout << bold_link("GitHub") << "\n";

    std::cout << "Markdown 格式化器 测试通过\n\n";
}

// ============================================
// 练习 4: 验证器组合
// ============================================

/**
 * 实现验证器组合
 *
 * Validator<T> 是一个返回 optional<string> 的函数
 * - nullopt 表示验证通过
 * - string 表示错误消息
 */

template<typename T>
using Validator = std::function<std::optional<std::string>(const T&)>;

// TODO: 实现 validate_and: 组合两个验证器，都通过才通过
template<typename T>
[[nodiscard]] Validator<T> validate_and(Validator<T> v1, Validator<T> v2) {
    return [v1, v2](const T& value) -> std::optional<std::string> {
        // 如果 v1 失败，返回 v1 的错误
        // 否则返回 v2 的结果
        return std::nullopt;  // 修改这行
    };
}

// TODO: 实现 validate_or: 组合两个验证器，任一通过就通过
template<typename T>
[[nodiscard]] Validator<T> validate_or(Validator<T> v1, Validator<T> v2) {
    return [v1, v2](const T& value) -> std::optional<std::string> {
        // 如果 v1 通过，返回 nullopt
        // 否则如果 v2 通过，返回 nullopt
        // 否则返回 v2 的错误
        return std::nullopt;  // 修改这行
    };
}

void test_exercise_4() {
    std::cout << "=== 练习 4: 验证器组合 ===\n";

    // 基础验证器
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

    assert(!positive_and_even(4).has_value());  // 通过
    assert(positive_and_even(-4).has_value());  // 失败：不是正数
    assert(positive_and_even(3).has_value());   // 失败：不是偶数

    // validate_or 测试
    auto positive_or_even = validate_or(is_positive, is_even);

    assert(!positive_or_even(4).has_value());   // 通过：正数且偶数
    assert(!positive_or_even(3).has_value());   // 通过：正数
    assert(!positive_or_even(-4).has_value());  // 通过：偶数
    assert(positive_or_even(-3).has_value());   // 失败：非正非偶

    std::cout << "验证器组合 测试通过\n\n";
}

// ============================================
// 练习 5: 实现 juxt 组合子
// ============================================

/**
 * juxt: 并行应用多个函数，返回结果向量
 *
 * juxt(f, g, h)(x) = {f(x), g(x), h(x)}
 *
 * 用例：一次计算多个属性
 */

// TODO: 实现 juxt
template<typename... Fs>
[[nodiscard]] auto juxt(Fs... fs) {
    return [fs...](auto x) {
        // 返回 vector，包含每个函数应用到 x 的结果
        // 假设所有函数返回相同类型
        using R = std::common_type_t<decltype(fs(x))...>;
        return std::vector<R>{};  // 修改这行
    };
}

void test_exercise_5() {
    std::cout << "=== 练习 5: 实现 juxt ===\n";

    auto add1 = [](int x) { return x + 1; };
    auto double_it = [](int x) { return x * 2; };
    auto square = [](int x) { return x * x; };

    auto analyze = juxt(add1, double_it, square);
    auto results = analyze(5);

    // results = {6, 10, 25}
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

    std::cout << "juxt 测试通过\n\n";
}

// ============================================
// main
// ============================================

int main() {
    std::cout << "========================================\n";
    std::cout << "    第06章练习：函数组合\n";
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
