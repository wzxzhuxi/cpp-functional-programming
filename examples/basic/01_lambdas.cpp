#include <algorithm>
#include <functional>
#include <iostream>
#include <numeric>
#include <vector>
#include <string>

// 1. 基本 lambda 用法
void basic_lambdas() {
    std::cout << "=== Basic Lambdas ===\n";

    // 简单 lambda
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

// 2. 捕获外部变量
void capture_examples() {
    std::cout << "=== Capture Examples ===\n";

    int x = 10;
    int y = 20;

    // 值捕获
    auto f1 = [x]() { return x * 2; };
    std::cout << "f1() = " << f1() << " (x captured by value)\n";

    // 引用捕获
    auto f2 = [&x]() { x += 5; return x; };
    std::cout << "f2() = " << f2() << " (x modified via reference)\n";
    std::cout << "x is now: " << x << "\n";

    // 捕获所有（值）
    auto f3 = [=]() { return x + y; };
    std::cout << "f3() = " << f3() << " (captured all by value)\n";

    // 混合捕获
    auto f4 = [x, &y]() { y += x; return y; };
    std::cout << "f4() = " << f4() << " (x by value, y by ref)\n\n";
}

// 3. 闭包示例
auto make_adder(int n) {
    return [n](int x) { return x + n; };
}

auto make_multiplier(int n) {
    return [n](int x) { return x * n; };
}

void closure_examples() {
    std::cout << "=== Closure Examples ===\n";

    auto add5 = make_adder(5);
    auto add10 = make_adder(10);
    auto times3 = make_multiplier(3);

    std::cout << "add5(7) = " << add5(7) << "\n";
    std::cout << "add10(7) = " << add10(7) << "\n";
    std::cout << "times3(7) = " << times3(7) << "\n\n";
}

// 4. Lambda 用于 STL 算法
void stl_algorithm_examples() {
    std::cout << "=== STL Algorithm Examples ===\n";

    std::vector<int> nums = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    // 过滤偶数
    std::vector<int> evens;
    std::copy_if(nums.begin(), nums.end(),
                 std::back_inserter(evens),
                 [](int x) { return x % 2 == 0; });

    std::cout << "Evens: ";
    for (int n : evens) std::cout << n << " ";
    std::cout << "\n";

    // 平方变换
    std::vector<int> squares;
    std::transform(nums.begin(), nums.end(),
                   std::back_inserter(squares),
                   [](int x) { return x * x; });

    std::cout << "Squares: ";
    for (int n : squares) std::cout << n << " ";
    std::cout << "\n";

    // 求和
    auto sum = std::accumulate(nums.begin(), nums.end(), 0,
                               [](int acc, int x) { return acc + x; });
    std::cout << "Sum: " << sum << "\n";

    // 计数满足条件的元素
    auto count = std::count_if(nums.begin(), nums.end(),
                                [](int x) { return x > 5; });
    std::cout << "Count (>5): " << count << "\n\n";
}

// 5. std::function 示例
void function_wrapper_examples() {
    std::cout << "=== std::function Examples ===\n";

    std::function<int(int, int)> op;

    // 存储 lambda
    op = [](int a, int b) { return a + b; };
    std::cout << "op(2, 3) with lambda: " << op(2, 3) << "\n";

    // 存储另一个 lambda
    op = [](int a, int b) { return a * b; };
    std::cout << "op(2, 3) with multiply: " << op(2, 3) << "\n";

    // 存储闭包
    int factor = 10;
    op = [factor](int a, int b) { return (a + b) * factor; };
    std::cout << "op(2, 3) with closure: " << op(2, 3) << "\n\n";
}

// 6. 高阶函数示例
template<typename T, typename Pred>
std::vector<T> filter(const std::vector<T>& vec, Pred pred) {
    std::vector<T> result;
    std::copy_if(vec.begin(), vec.end(),
                 std::back_inserter(result),
                 pred);
    return result;
}

template<typename T, typename Func>
auto map(const std::vector<T>& vec, Func f) {
    using ReturnType = decltype(f(std::declval<T>()));
    std::vector<ReturnType> result;
    std::transform(vec.begin(), vec.end(),
                   std::back_inserter(result),
                   f);
    return result;
}

void higher_order_examples() {
    std::cout << "=== Higher-Order Function Examples ===\n";

    std::vector<int> nums = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    // 使用自定义 filter
    auto evens = filter(nums, [](int x) { return x % 2 == 0; });
    std::cout << "Filtered evens: ";
    for (int n : evens) std::cout << n << " ";
    std::cout << "\n";

    // 使用自定义 map
    auto squares = map(nums, [](int x) { return x * x; });
    std::cout << "Mapped squares: ";
    for (int n : squares) std::cout << n << " ";
    std::cout << "\n";

    // 链式调用
    auto result = map(
        filter(nums, [](int x) { return x > 5; }),
        [](int x) { return x * 2; }
    );
    std::cout << "Filtered (>5) then doubled: ";
    for (int n : result) std::cout << n << " ";
    std::cout << "\n\n";
}

int main() {
    basic_lambdas();
    capture_examples();
    closure_examples();
    stl_algorithm_examples();
    function_wrapper_examples();
    higher_order_examples();

    return 0;
}
