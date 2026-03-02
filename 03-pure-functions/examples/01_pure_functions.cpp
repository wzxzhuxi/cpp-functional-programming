/**
 * @file 01_pure_functions.cpp
 * @brief 纯函数示例
 *
 * 本文件演示：
 * - 纯函数 vs 不纯函数
 * - 引用透明性
 * - 副作用隔离
 * - 可测试性
 * - 函数组合
 * - 记忆化（不使用全局状态）
 */

#include <algorithm>
#include <cassert>
#include <cmath>
#include <functional>
#include <iostream>
#include <memory>
#include <numeric>
#include <string>
#include <unordered_map>
#include <vector>

// ============================================
// 1. 纯函数 vs 不纯函数
// ============================================

// 纯函数：相同输入永远得到相同输出，无副作用
[[nodiscard]] int pure_add(int a, int b) {
    return a + b;
}

[[nodiscard]] int pure_square(int x) {
    return x * x;
}

// 不纯：依赖全局状态
int global_factor = 10;
int impure_multiply(int x) {
    return x * global_factor;  // 结果依赖外部变量
}

// 不纯：修改全局状态
int call_count = 0;
int impure_count_calls(int x) {
    call_count++;  // 副作用
    return x * 2;
}

// 不纯：I/O 操作
int impure_with_io(int x) {
    std::cout << "Calculating..." << std::endl;  // 副作用
    return x * 2;
}

void pure_vs_impure_demo() {
    std::cout << "=== 纯函数 vs 不纯函数 ===\n";

    // 纯函数：可预测，多次调用结果相同
    std::cout << "pure_add(2, 3) = " << pure_add(2, 3) << "\n";
    std::cout << "pure_add(2, 3) = " << pure_add(2, 3) << " (结果相同)\n";

    // 不纯：结果可能变化
    std::cout << "impure_multiply(5) = " << impure_multiply(5) << "\n";
    global_factor = 20;
    std::cout << "impure_multiply(5) = " << impure_multiply(5) << " (结果变了!)\n";

    // 不纯：有副作用
    std::cout << "call_count 调用前: " << call_count << "\n";
    impure_count_calls(10);
    impure_count_calls(10);
    std::cout << "call_count 调用后: " << call_count << " (被修改了!)\n\n";
}

// ============================================
// 2. 引用透明性
// ============================================

[[nodiscard]] int rt_square(int x) {
    return x * x;
}

void referential_transparency_demo() {
    std::cout << "=== 引用透明性 ===\n";

    // 纯函数调用可以被其结果替换
    int a = rt_square(5);                    // 25
    int b = rt_square(5) + rt_square(5);     // 50

    std::cout << "rt_square(5) = " << a << "\n";
    std::cout << "rt_square(5) + rt_square(5) = " << b << "\n";

    // 等价于直接用值
    int c = 25;
    int d = 25 + 25;
    std::cout << "直接用 25 = " << c << " (等价于 rt_square(5))\n";
    std::cout << "25 + 25 = " << d << " (等价于 rt_square(5) + rt_square(5))\n\n";
}

// ============================================
// 3. 副作用隔离
// ============================================

// 纯函数：核心业务逻辑
[[nodiscard]] std::vector<int> filter_evens(const std::vector<int>& nums) {
    std::vector<int> result;
    std::copy_if(nums.begin(), nums.end(),
                 std::back_inserter(result),
                 [](int x) { return x % 2 == 0; });
    return result;
}

[[nodiscard]] std::vector<int> square_all(const std::vector<int>& nums) {
    std::vector<int> result;
    result.reserve(nums.size());
    std::transform(nums.begin(), nums.end(),
                   std::back_inserter(result),
                   [](int x) { return x * x; });
    return result;
}

[[nodiscard]] int sum_all(const std::vector<int>& nums) {
    return std::accumulate(nums.begin(), nums.end(), 0);
}

void side_effect_isolation_demo() {
    std::cout << "=== 副作用隔离 ===\n";

    // 输入（实际应用中可能来自文件/网络）
    const std::vector<int> input = {1, 2, 3, 4, 5, 6};

    // 纯函数处理链：无副作用
    const auto evens = filter_evens(input);
    const auto squares = square_all(evens);
    const auto total = sum_all(squares);

    // 输出（副作用，但隔离在边界）
    std::cout << "输入: ";
    for (int x : input) std::cout << x << " ";
    std::cout << "\n";

    std::cout << "偶数: ";
    for (int x : evens) std::cout << x << " ";
    std::cout << "\n";

    std::cout << "平方: ";
    for (int x : squares) std::cout << x << " ";
    std::cout << "\n";

    std::cout << "总和: " << total << "\n";
    std::cout << "(2² + 4² + 6² = 4 + 16 + 36 = 56)\n\n";
}

// ============================================
// 4. 可测试性
// ============================================

[[nodiscard]] int factorial(int n) {
    if (n <= 1) return 1;
    return n * factorial(n - 1);
}

[[nodiscard]] int gcd(int a, int b) {
    while (b != 0) {
        int temp = b;
        b = a % b;
        a = temp;
    }
    return a;
}

[[nodiscard]] bool is_palindrome(const std::string& s) {
    auto clean = std::string{};
    std::copy_if(s.begin(), s.end(), std::back_inserter(clean),
                 [](char c) { return std::isalnum(c); });
    std::transform(clean.begin(), clean.end(), clean.begin(),
                   [](char c) { return std::tolower(c); });

    auto reversed = clean;
    std::reverse(reversed.begin(), reversed.end());
    return clean == reversed;
}

void testability_demo() {
    std::cout << "=== 纯函数的可测试性 ===\n";

    // factorial 测试：不需要 setup/teardown
    assert(factorial(0) == 1);
    assert(factorial(1) == 1);
    assert(factorial(5) == 120);
    assert(factorial(10) == 3628800);
    std::cout << "factorial 测试通过\n";

    // gcd 测试
    assert(gcd(12, 8) == 4);
    assert(gcd(17, 5) == 1);
    assert(gcd(100, 50) == 50);
    assert(gcd(0, 5) == 5);
    std::cout << "gcd 测试通过\n";

    // is_palindrome 测试
    assert(is_palindrome("A man a plan a canal Panama"));
    assert(is_palindrome("racecar"));
    assert(!is_palindrome("hello"));
    std::cout << "is_palindrome 测试通过\n\n";
}

// ============================================
// 5. 函数组合
// ============================================

template<typename F, typename G>
[[nodiscard]] auto compose(F f, G g) {
    return [f, g](auto x) { return f(g(x)); };
}

template<typename F, typename G>
[[nodiscard]] auto pipe(F f, G g) {
    return [f, g](auto x) { return g(f(x)); };
}

void composition_demo() {
    std::cout << "=== 函数组合 ===\n";

    auto double_it = [](int x) { return x * 2; };
    auto add_ten = [](int x) { return x + 10; };
    auto square = [](int x) { return x * x; };

    // compose: 从右到左 (数学风格)
    auto square_then_add = compose(add_ten, square);
    std::cout << "compose(add_ten, square)(5) = " << square_then_add(5) << " (25 + 10 = 35)\n";

    // pipe: 从左到右 (数据流风格)
    auto double_then_add = pipe(double_it, add_ten);
    std::cout << "pipe(double, add_ten)(5) = " << double_then_add(5) << " (10 + 10 = 20)\n";

    // 链式组合
    const std::vector<int> nums = {1, 2, 3, 4, 5};
    const auto evens = filter_evens(nums);
    const auto squares = square_all(evens);
    const auto total = sum_all(squares);

    std::cout << "filter_evens -> square_all -> sum_all: " << total << "\n";
    std::cout << "(2² + 4² = 4 + 16 = 20)\n\n";
}

// ============================================
// 6. 记忆化（不使用全局状态）
// ============================================

/**
 * 通用记忆化包装器
 *
 * 使用 shared_ptr 存储缓存，而不是静态变量
 * 这样每次调用 memoize() 都创建独立的缓存
 */
template<typename F>
auto memoize(F f) {
    // 推断参数和返回类型
    // 简化版：假设单参数函数
    using traits = decltype(std::function{f});

    return [f, cache = std::make_shared<std::unordered_map<int, int>>()](int arg) {
        auto it = cache->find(arg);
        if (it != cache->end()) {
            return it->second;
        }
        auto result = f(arg);
        (*cache)[arg] = result;
        return result;
    };
}

// 慢版本：O(2^n)
int fib_slow(int n) {
    if (n <= 1) return n;
    return fib_slow(n - 1) + fib_slow(n - 2);
}

// 手动记忆化版本（展示原理）
class MemoizedFib {
    std::unordered_map<int, int> cache_;

public:
    int operator()(int n) {
        if (n <= 1) return n;

        auto it = cache_.find(n);
        if (it != cache_.end()) {
            return it->second;
        }

        int result = (*this)(n - 1) + (*this)(n - 2);
        cache_[n] = result;
        return result;
    }

    void clear_cache() { cache_.clear(); }
};

void memoization_demo() {
    std::cout << "=== 记忆化 ===\n";

    // 慢版本
    std::cout << "fib_slow(10) = " << fib_slow(10) << "\n";
    std::cout << "fib_slow(20) = " << fib_slow(20) << " (开始变慢)\n";

    // 快版本（记忆化）
    MemoizedFib fast_fib;
    std::cout << "MemoizedFib(30) = " << fast_fib(30) << " (瞬间完成)\n";
    std::cout << "MemoizedFib(40) = " << fast_fib(40) << " (仍然很快)\n";

    // 使用 memoize 包装器
    auto square_cached = memoize([](int x) { return x * x; });
    std::cout << "memoize(square)(5) = " << square_cached(5) << "\n";
    std::cout << "memoize(square)(5) = " << square_cached(5) << " (缓存命中)\n\n";
}

// ============================================
// 7. 重构不纯函数为纯函数
// ============================================

// 不纯：修改参数
void bad_double_all(std::vector<int>& vec) {
    for (auto& x : vec) {
        x *= 2;
    }
}

// 纯：返回新值
[[nodiscard]] std::vector<int> double_all(const std::vector<int>& vec) {
    std::vector<int> result;
    result.reserve(vec.size());
    std::transform(vec.begin(), vec.end(),
                   std::back_inserter(result),
                   [](int x) { return x * 2; });
    return result;
}

// 不纯：依赖和修改全局状态
namespace bad {
    int total = 0;
    void add_to_total(int x) {
        total += x;
    }
}

// 纯：显式传递状态
[[nodiscard]] int add_to_total(int current_total, int x) {
    return current_total + x;
}

void refactoring_demo() {
    std::cout << "=== 重构为纯函数 ===\n";

    std::vector<int> nums = {1, 2, 3, 4, 5};

    // 不纯版本：修改原数据
    auto nums_copy = nums;
    bad_double_all(nums_copy);
    std::cout << "bad_double_all 后 (原地修改): ";
    for (int x : nums_copy) std::cout << x << " ";
    std::cout << "\n";

    // 纯版本：原数据不变
    const auto doubled = double_all(nums);
    std::cout << "原数据: ";
    for (int x : nums) std::cout << x << " ";
    std::cout << " (未改变)\n";
    std::cout << "double_all 结果: ";
    for (int x : doubled) std::cout << x << " ";
    std::cout << "\n";

    // 状态传递示例
    int state = 0;
    state = add_to_total(state, 10);
    state = add_to_total(state, 20);
    state = add_to_total(state, 30);
    std::cout << "纯函数累加: " << state << " (10 + 20 + 30 = 60)\n\n";
}

// ============================================
// 8. const 正确性
// ============================================

class Calculator {
public:
    // 纯：const 成员函数，不修改对象状态
    [[nodiscard]] int add(int a, int b) const {
        return a + b;
    }

    [[nodiscard]] int multiply(int a, int b) const {
        return a * b;
    }

    [[nodiscard]] int sum_vector(const std::vector<int>& nums) const {
        return std::accumulate(nums.begin(), nums.end(), 0);
    }

    [[nodiscard]] double average(const std::vector<int>& nums) const {
        if (nums.empty()) return 0.0;
        return static_cast<double>(sum_vector(nums)) / nums.size();
    }
};

void const_correctness_demo() {
    std::cout << "=== const 正确性 ===\n";

    const Calculator calc;  // const 对象只能调用 const 方法
    std::cout << "calc.add(2, 3) = " << calc.add(2, 3) << "\n";
    std::cout << "calc.multiply(4, 5) = " << calc.multiply(4, 5) << "\n";

    const std::vector<int> nums = {1, 2, 3, 4, 5};
    std::cout << "calc.sum_vector = " << calc.sum_vector(nums) << "\n";
    std::cout << "calc.average = " << calc.average(nums) << "\n\n";
}

// ============================================
// main
// ============================================

int main() {
    pure_vs_impure_demo();
    referential_transparency_demo();
    side_effect_isolation_demo();
    testability_demo();
    composition_demo();
    memoization_demo();
    refactoring_demo();
    const_correctness_demo();

    return 0;
}
