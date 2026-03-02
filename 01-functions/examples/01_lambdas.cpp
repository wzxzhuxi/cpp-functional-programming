/**
 * @file 01_lambdas.cpp
 * @brief Lambda 表达式和闭包示例
 *
 * 本文件演示：
 * - Lambda 基本语法
 * - 变量捕获（值捕获、引用捕获）
 * - 闭包的创建和使用
 * - 与 STL 算法配合
 * - C++14/17/20 新特性
 */

#include <algorithm>
#include <functional>
#include <iostream>
#include <memory>
#include <numeric>
#include <string>
#include <vector>

using namespace std::string_literals;

// ============================================
// 1. 基本 Lambda 语法
// ============================================

void basic_lambda_demo() {
    std::cout << "=== 基本 Lambda ===\n";

    // 最简单的 lambda
    auto add = [](int a, int b) { return a + b; };
    std::cout << "add(2, 3) = " << add(2, 3) << "\n";

    // 显式返回类型
    auto divide = [](int a, int b) -> double {
        return static_cast<double>(a) / b;
    };
    std::cout << "divide(7, 2) = " << divide(7, 2) << "\n";

    // 无参数 lambda
    auto get_pi = []() { return 3.14159; };
    std::cout << "pi = " << get_pi() << "\n\n";
}

// ============================================
// 2. 变量捕获
// ============================================

void capture_demo() {
    std::cout << "=== 变量捕获 ===\n";

    int x = 10;
    int y = 20;

    // 值捕获：复制 x
    auto f1 = [x]() { return x * 2; };
    std::cout << "值捕获 f1() = " << f1() << "\n";

    // 引用捕获：可以修改 x
    auto f2 = [&x]() {
        x += 5;
        return x;
    };
    std::cout << "引用捕获 f2() = " << f2() << " (x 被修改)\n";
    std::cout << "现在 x = " << x << "\n";

    // 混合捕获
    auto f3 = [x, &y]() {
        y += x;  // 可以修改 y，但 x 是副本
        return y;
    };
    std::cout << "混合捕获 f3() = " << f3() << "\n";
    std::cout << "现在 y = " << y << "\n\n";
}

// ============================================
// 3. 闭包：函数工厂
// ============================================

auto make_adder(int n) {
    // 返回一个记住 n 的 lambda
    return [n](int x) { return x + n; };
}

auto make_multiplier(int n) {
    return [n](int x) { return x * n; };
}

void closure_demo() {
    std::cout << "=== 闭包 ===\n";

    auto add5 = make_adder(5);
    auto add10 = make_adder(10);
    auto times3 = make_multiplier(3);

    std::cout << "add5(7) = " << add5(7) << "\n";
    std::cout << "add10(7) = " << add10(7) << "\n";
    std::cout << "times3(7) = " << times3(7) << "\n\n";
}

// ============================================
// 4. 与 STL 算法配合
// ============================================

void stl_algorithm_demo() {
    std::cout << "=== STL 算法 ===\n";

    std::vector<int> nums = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    // filter：过滤偶数
    std::vector<int> evens;
    std::copy_if(nums.begin(), nums.end(), std::back_inserter(evens),
                 [](int x) { return x % 2 == 0; });

    std::cout << "偶数: ";
    for (int n : evens) std::cout << n << " ";
    std::cout << "\n";

    // map：平方变换
    std::vector<int> squares;
    std::transform(nums.begin(), nums.end(), std::back_inserter(squares),
                   [](int x) { return x * x; });

    std::cout << "平方: ";
    for (int n : squares) std::cout << n << " ";
    std::cout << "\n";

    // reduce：求和
    auto sum = std::accumulate(nums.begin(), nums.end(), 0,
                               [](int acc, int x) { return acc + x; });
    std::cout << "求和: " << sum << "\n";

    // 复合操作：偶数的平方和
    auto even_square_sum = std::accumulate(nums.begin(), nums.end(), 0,
        [](int acc, int x) {
            return x % 2 == 0 ? acc + x * x : acc;
        });
    std::cout << "偶数平方和: " << even_square_sum << "\n\n";
}

// ============================================
// 5. std::function 示例
// ============================================

void std_function_demo() {
    std::cout << "=== std::function ===\n";

    // 可以存储任何签名匹配的可调用对象
    std::function<int(int, int)> op;

    // 存储 lambda
    op = [](int a, int b) { return a + b; };
    std::cout << "加法: op(2, 3) = " << op(2, 3) << "\n";

    // 存储另一个 lambda
    op = [](int a, int b) { return a * b; };
    std::cout << "乘法: op(2, 3) = " << op(2, 3) << "\n";

    // 存储闭包
    int factor = 10;
    op = [factor](int a, int b) { return (a + b) * factor; };
    std::cout << "闭包: op(2, 3) = " << op(2, 3) << "\n\n";
}

// ============================================
// 6. C++14 泛型 Lambda
// ============================================

void generic_lambda_demo() {
    std::cout << "=== 泛型 Lambda (C++14) ===\n";

    // auto 参数：像模板一样
    auto print_value = [](const auto& value) {
        std::cout << value << " ";
    };

    print_value(42);
    print_value(3.14);
    print_value("hello"s);
    std::cout << "\n";

    // 泛型加法
    auto add = [](auto a, auto b) { return a + b; };
    std::cout << "int: " << add(1, 2) << "\n";
    std::cout << "double: " << add(1.5, 2.5) << "\n";
    std::cout << "string: " << add("hello "s, "world"s) << "\n\n";
}

// ============================================
// 7. C++14 初始化捕获
// ============================================

void init_capture_demo() {
    std::cout << "=== 初始化捕获 (C++14) ===\n";

    // 移动捕获 unique_ptr
    auto ptr = std::make_unique<int>(42);

    auto f = [p = std::move(ptr)]() {
        return *p;
    };

    std::cout << "移动捕获 unique_ptr: " << f() << "\n";

    // 重命名捕获
    int value = 100;
    auto g = [v = value * 2]() {
        return v;
    };
    std::cout << "重命名捕获: " << g() << "\n\n";
}

// ============================================
// 8. C++17 constexpr Lambda
// ============================================

void constexpr_lambda_demo() {
    std::cout << "=== constexpr Lambda (C++17) ===\n";

    constexpr auto square = [](int x) { return x * x; };

    // 编译期计算
    constexpr int result = square(5);
    static_assert(result == 25, "编译期计算错误");

    std::cout << "constexpr square(5) = " << result << "\n\n";
}

// ============================================
// 9. C++20 模板 Lambda
// ============================================

void template_lambda_demo() {
    std::cout << "=== 模板 Lambda (C++20) ===\n";

    // 显式模板参数
    auto print_vector = []<typename T>(const std::vector<T>& vec) {
        std::cout << "[ ";
        for (const auto& item : vec) {
            std::cout << item << " ";
        }
        std::cout << "]\n";
    };

    print_vector(std::vector{1, 2, 3, 4, 5});
    print_vector(std::vector{"a"s, "b"s, "c"s});

    // 带约束的模板 lambda
    auto add_numeric = []<typename T>(T a, T b)
        requires std::is_arithmetic_v<T>
    {
        return a + b;
    };

    std::cout << "numeric add: " << add_numeric(10, 20) << "\n\n";
}

// ============================================
// 10. 自定义高阶函数
// ============================================

template <typename Container, typename Pred>
auto filter(const Container& c, Pred pred) {
    Container result;
    std::copy_if(c.begin(), c.end(), std::back_inserter(result), pred);
    return result;
}

template <typename Container, typename Func>
auto map(const Container& c, Func f) {
    using T = typename Container::value_type;
    using R = decltype(f(std::declval<T>()));
    std::vector<R> result;
    result.reserve(c.size());
    std::transform(c.begin(), c.end(), std::back_inserter(result), f);
    return result;
}

void custom_hof_demo() {
    std::cout << "=== 自定义高阶函数 ===\n";

    std::vector<int> nums = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    // 链式调用
    auto result = map(
        filter(nums, [](int x) { return x > 5; }),
        [](int x) { return x * 2; });

    std::cout << "filter(>5) 然后 map(*2): ";
    for (int n : result) std::cout << n << " ";
    std::cout << "\n\n";
}

// ============================================
// main
// ============================================

int main() {
    basic_lambda_demo();
    capture_demo();
    closure_demo();
    stl_algorithm_demo();
    std_function_demo();
    generic_lambda_demo();
    init_capture_demo();
    constexpr_lambda_demo();
    template_lambda_demo();
    custom_hof_demo();

    return 0;
}
