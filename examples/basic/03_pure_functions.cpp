#include <algorithm>
#include <cassert>
#include <cmath>
#include <iostream>
#include <numeric>
#include <vector>
#include <string>
#include <unordered_map>

// ============================================
// 1. 纯函数 vs 不纯函数
// ============================================

// 纯函数
int pure_add(int a, int b) {
    return a + b;
}

int pure_square(int x) {
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
    std::cout << "=== Pure vs Impure Functions ===\n";

    // 纯函数：可预测
    std::cout << "pure_add(2, 3) = " << pure_add(2, 3) << "\n";
    std::cout << "pure_add(2, 3) = " << pure_add(2, 3) << " (same result)\n";

    // 不纯：结果可能变化
    std::cout << "impure_multiply(5) = " << impure_multiply(5) << "\n";
    global_factor = 20;
    std::cout << "impure_multiply(5) = " << impure_multiply(5) << " (different!)\n";

    // 不纯：有副作用
    std::cout << "call_count before: " << call_count << "\n";
    impure_count_calls(10);
    std::cout << "call_count after: " << call_count << " (modified!)\n\n";
}

// ============================================
// 2. 引用透明性
// ============================================

// 引用透明的函数
int rt_square(int x) {
    return x * x;
}

void referential_transparency_demo() {
    std::cout << "=== Referential Transparency ===\n";

    // 可以被结果替换
    int a = rt_square(5);  // 25
    int b = rt_square(5) + rt_square(5);  // 50

    std::cout << "a = " << a << "\n";
    std::cout << "b = " << b << "\n";

    // 等价于直接用值
    int c = 25;
    int d = 25 + 25;
    std::cout << "c = " << c << " (same as a)\n";
    std::cout << "d = " << d << " (same as b)\n\n";
}

// ============================================
// 3. 副作用隔离
// ============================================

// 纯函数：核心逻辑
std::vector<int> filter_evens(const std::vector<int>& nums) {
    std::vector<int> result;
    std::copy_if(nums.begin(), nums.end(),
                 std::back_inserter(result),
                 [](int x) { return x % 2 == 0; });
    return result;
}

std::vector<int> square_all(const std::vector<int>& nums) {
    std::vector<int> result;
    std::transform(nums.begin(), nums.end(),
                   std::back_inserter(result),
                   [](int x) { return x * x; });
    return result;
}

// 不纯：I/O 边界
void side_effect_isolation_demo() {
    std::cout << "=== Side Effect Isolation ===\n";

    // 不纯：输入
    std::vector<int> input = {1, 2, 3, 4, 5, 6};

    // 纯函数：业务逻辑
    auto evens = filter_evens(input);
    auto squares = square_all(evens);

    // 不纯：输出
    std::cout << "Input: ";
    for (int x : input) std::cout << x << " ";
    std::cout << "\n";

    std::cout << "Evens: ";
    for (int x : evens) std::cout << x << " ";
    std::cout << "\n";

    std::cout << "Squared: ";
    for (int x : squares) std::cout << x << " ";
    std::cout << "\n\n";
}

// ============================================
// 4. 纯函数的可测试性
// ============================================

// 纯函数：易于测试
int factorial(int n) {
    if (n <= 1) return 1;
    return n * factorial(n - 1);
}

int gcd(int a, int b) {
    while (b != 0) {
        int temp = b;
        b = a % b;
        a = temp;
    }
    return a;
}

void testability_demo() {
    std::cout << "=== Testability of Pure Functions ===\n";

    // 简单直接的测试
    assert(factorial(0) == 1);
    assert(factorial(1) == 1);
    assert(factorial(5) == 120);
    std::cout << "factorial tests passed\n";

    assert(gcd(12, 8) == 4);
    assert(gcd(17, 5) == 1);
    assert(gcd(100, 50) == 50);
    std::cout << "gcd tests passed\n\n";
}

// ============================================
// 5. 纯函数组合
// ============================================

// 纯函数可以随意组合
auto is_even = [](int x) { return x % 2 == 0; };
auto square = [](int x) { return x * x; };
auto add_ten = [](int x) { return x + 10; };

template<typename F, typename G>
auto compose(F f, G g) {
    return [f, g](auto x) { return f(g(x)); };
}

void composition_demo() {
    std::cout << "=== Function Composition ===\n";

    // 组合函数
    auto square_then_add_ten = compose(add_ten, square);

    std::cout << "square_then_add_ten(5) = " << square_then_add_ten(5) << " (25 + 10 = 35)\n";

    // 链式组合
    std::vector<int> nums = {1, 2, 3, 4, 5};

    // 过滤 -> 平方 -> 求和
    auto evens = filter_evens(nums);
    auto squares = square_all(evens);
    int sum = std::accumulate(squares.begin(), squares.end(), 0);

    std::cout << "Sum of squares of evens: " << sum << "\n";
    std::cout << "(2^2 + 4^2 = 4 + 16 = 20)\n\n";
}

// ============================================
// 6. Memoization（缓存优化）
// ============================================

// 昂贵计算（斐波那契）
int fib_slow(int n) {
    if (n <= 1) return n;
    return fib_slow(n - 1) + fib_slow(n - 2);
}

// 带缓存的版本（对外仍是纯函数）
int fib_cached(int n) {
    static std::unordered_map<int, int> cache;

    if (n <= 1) return n;

    auto it = cache.find(n);
    if (it != cache.end()) {
        return it->second;
    }

    int result = fib_cached(n - 1) + fib_cached(n - 2);
    cache[n] = result;
    return result;
}

void memoization_demo() {
    std::cout << "=== Memoization ===\n";

    std::cout << "fib_slow(10) = " << fib_slow(10) << "\n";
    std::cout << "fib_cached(10) = " << fib_cached(10) << "\n";
    std::cout << "fib_cached(30) = " << fib_cached(30) << " (would be slow without cache)\n\n";
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
std::vector<int> double_all(const std::vector<int>& vec) {
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
int add_to_total(int current_total, int x) {
    return current_total + x;
}

void refactoring_demo() {
    std::cout << "=== Refactoring to Pure Functions ===\n";

    std::vector<int> nums = {1, 2, 3, 4, 5};

    // 不纯版本
    auto nums_copy = nums;
    bad_double_all(nums_copy);
    std::cout << "After bad_double_all (modified in place): ";
    for (int x : nums_copy) std::cout << x << " ";
    std::cout << "\n";

    // 纯版本
    auto doubled = double_all(nums);
    std::cout << "Original: ";
    for (int x : nums) std::cout << x << " ";
    std::cout << " (unchanged)\n";
    std::cout << "Doubled: ";
    for (int x : doubled) std::cout << x << " ";
    std::cout << "\n\n";
}

// ============================================
// 8. const 正确性
// ============================================

class Calculator {
public:
    // 纯：const 成员函数
    int add(int a, int b) const {
        return a + b;
    }

    int multiply(int a, int b) const {
        return a * b;
    }

    // 纯：不修改参数
    int sum_vector(const std::vector<int>& nums) const {
        return std::accumulate(nums.begin(), nums.end(), 0);
    }
};

void const_correctness_demo() {
    std::cout << "=== Const Correctness ===\n";

    const Calculator calc;  // const 对象
    std::cout << "calc.add(2, 3) = " << calc.add(2, 3) << "\n";
    std::cout << "calc.multiply(4, 5) = " << calc.multiply(4, 5) << "\n";

    const std::vector<int> nums = {1, 2, 3, 4, 5};
    std::cout << "calc.sum_vector = " << calc.sum_vector(nums) << "\n\n";
}

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
