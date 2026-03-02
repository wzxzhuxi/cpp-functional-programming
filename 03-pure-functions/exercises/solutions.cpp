/**
 * @file solutions.cpp
 * @brief 第03章练习题参考答案
 */

#include <algorithm>
#include <cassert>
#include <functional>
#include <iostream>
#include <memory>
#include <numeric>
#include <string>
#include <unordered_map>
#include <vector>

// ============================================
// 练习 1: 判断纯度 - 答案
// ============================================

// 函数 A: 纯函数
// 原因：相同输入永远得到相同输出，无副作用
int func_a(int x) {
    return x * 2;
}

// 函数 B: 不纯
// 原因：依赖外部可变状态 counter_b
int counter_b = 0;
int func_b(int x) {
    return x + counter_b;  // 如果 counter_b 变了，结果就变了
}

// 函数 C: 不纯
// 原因：使用静态变量存储状态，结果依赖历史调用
int func_c(int x) {
    static int last = 0;
    int result = x + last;  // 依赖上次调用的值
    last = x;               // 副作用：修改静态变量
    return result;
}

// 函数 D: 不纯
// 原因：修改参数（副作用）
void func_d(std::vector<int>& vec) {
    std::sort(vec.begin(), vec.end());  // 修改了调用者的数据
}

// 函数 E: 纯函数
// 原因：不修改输入，相同输入得到相同输出
int func_e(const std::vector<int>& vec) {
    return std::accumulate(vec.begin(), vec.end(), 0);
}

// 函数 F: 不纯
// 原因：I/O 副作用（输出到控制台）
int func_f(int x) {
    std::cout << "Input: " << x << "\n";  // 副作用
    return x * 2;
}

void test_exercise_1() {
    std::cout << "=== 练习 1: 判断纯度 - 答案 ===\n";
    std::cout << "func_a: 纯函数 - 无外部依赖，无副作用\n";
    std::cout << "func_b: 不纯 - 依赖外部变量 counter_b\n";
    std::cout << "func_c: 不纯 - 使用静态变量，有状态\n";
    std::cout << "func_d: 不纯 - 修改参数\n";
    std::cout << "func_e: 纯函数 - const参数，只读取不修改\n";
    std::cout << "func_f: 不纯 - I/O操作（cout）\n\n";
}

// ============================================
// 练习 2: 重构为纯函数 - 答案
// ============================================

// 纯函数版本：返回新容器
[[nodiscard]] std::vector<int> pure_scale(const std::vector<int>& vec, int factor) {
    std::vector<int> result;
    result.reserve(vec.size());
    std::transform(vec.begin(), vec.end(),
                   std::back_inserter(result),
                   [factor](int x) { return x * factor; });
    return result;
}

// 纯函数版本：税率作为参数
[[nodiscard]] int calculate_tax_pure(int price, int rate_percent) {
    return price * rate_percent / 100;
}

// 纯函数版本：显式状态
struct Stats {
    int sum;
    int count;
};

[[nodiscard]] Stats add_value_pure(Stats current, int x) {
    return Stats{current.sum + x, current.count + 1};
}

[[nodiscard]] double get_average_pure(Stats s) {
    if (s.count == 0) return 0.0;
    return static_cast<double>(s.sum) / s.count;
}

void test_exercise_2() {
    std::cout << "=== 练习 2: 重构为纯函数 ===\n";

    // 测试 pure_scale
    std::vector<int> nums = {1, 2, 3, 4, 5};
    auto scaled = pure_scale(nums, 3);
    assert(nums[0] == 1);  // 原数组不变
    assert(scaled.size() == 5);
    assert(scaled[0] == 3);
    assert(scaled[4] == 15);
    std::cout << "pure_scale 通过: ";
    for (int x : scaled) std::cout << x << " ";
    std::cout << "\n";

    // 测试 calculate_tax_pure
    assert(calculate_tax_pure(100, 10) == 10);
    assert(calculate_tax_pure(200, 15) == 30);
    assert(calculate_tax_pure(1000, 20) == 200);
    std::cout << "calculate_tax_pure 通过\n";

    // 测试 Stats 纯函数
    Stats s = {0, 0};
    s = add_value_pure(s, 10);
    s = add_value_pure(s, 20);
    s = add_value_pure(s, 30);
    assert(s.sum == 60);
    assert(s.count == 3);
    assert(get_average_pure(s) == 20.0);
    std::cout << "Stats: sum=" << s.sum << ", count=" << s.count
              << ", avg=" << get_average_pure(s) << "\n\n";
}

// ============================================
// 练习 3: 实现记忆化 - 答案
// ============================================

class MemoizedFibonacci {
    std::unordered_map<int, int> cache_;

public:
    int operator()(int n) {
        if (n <= 1) return n;

        // 检查缓存
        auto it = cache_.find(n);
        if (it != cache_.end()) {
            return it->second;
        }

        // 递归计算并缓存
        int result = (*this)(n - 1) + (*this)(n - 2);
        cache_[n] = result;
        return result;
    }

    // 可选：清空缓存
    void clear() { cache_.clear(); }

    // 可选：查看缓存大小
    [[nodiscard]] size_t cache_size() const { return cache_.size(); }
};

// 替代实现：使用闭包
auto make_memoized_fib() {
    auto cache = std::make_shared<std::unordered_map<int, int>>();

    // 需要 std::function 来实现递归 lambda
    std::function<int(int)> fib = [cache, &fib](int n) -> int {
        if (n <= 1) return n;

        auto it = cache->find(n);
        if (it != cache->end()) {
            return it->second;
        }

        int result = fib(n - 1) + fib(n - 2);
        (*cache)[n] = result;
        return result;
    };

    return fib;
}

void test_exercise_3() {
    std::cout << "=== 练习 3: 实现记忆化 ===\n";

    MemoizedFibonacci fib;

    // 基本测试
    assert(fib(0) == 0);
    assert(fib(1) == 1);
    assert(fib(10) == 55);
    std::cout << "fib(10) = " << fib(10) << "\n";

    // 性能测试
    int result = fib(40);
    std::cout << "fib(40) = " << result << "\n";
    assert(result == 102334155);

    std::cout << "缓存大小: " << fib.cache_size() << " 个条目\n";

    // 测试闭包版本
    auto fib2 = make_memoized_fib();
    assert(fib2(30) == 832040);
    std::cout << "闭包版本 fib(30) = " << fib2(30) << "\n\n";
}

// ============================================
// 练习 4: 分离副作用 - 答案
// ============================================

struct ProcessResult {
    std::vector<int> values;
    int sum;
};

// 纯函数：只做数据处理
[[nodiscard]] ProcessResult process_data(const std::vector<int>& input) {
    std::vector<int> values;

    // 过滤偶数并平方
    for (int x : input) {
        if (x % 2 == 0) {
            values.push_back(x * x);
        }
    }

    // 计算总和
    int sum = std::accumulate(values.begin(), values.end(), 0);

    return {values, sum};
}

// 替代实现：使用 STL 算法
[[nodiscard]] ProcessResult process_data_stl(const std::vector<int>& input) {
    std::vector<int> evens;
    std::copy_if(input.begin(), input.end(), std::back_inserter(evens),
                 [](int x) { return x % 2 == 0; });

    std::vector<int> values;
    values.reserve(evens.size());
    std::transform(evens.begin(), evens.end(), std::back_inserter(values),
                   [](int x) { return x * x; });

    int sum = std::accumulate(values.begin(), values.end(), 0);
    return {values, sum};
}

// 不纯函数：只负责 I/O
void run_pipeline() {
    // 1. 输入（副作用：可能来自文件/网络）
    std::vector<int> input = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    // 2. 纯函数处理
    auto result = process_data(input);

    // 3. 输出（副作用）
    std::cout << "处理结果: ";
    for (int x : result.values) {
        std::cout << x << " ";
    }
    std::cout << "\n";
    std::cout << "总和: " << result.sum << "\n";
}

void test_exercise_4() {
    std::cout << "=== 练习 4: 分离副作用 ===\n";

    // 测试纯函数（不涉及 I/O）
    std::vector<int> input = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    auto result = process_data(input);

    assert(result.values.size() == 5);
    assert(result.values[0] == 4);   // 2²
    assert(result.values[1] == 16);  // 4²
    assert(result.values[2] == 36);  // 6²
    assert(result.values[3] == 64);  // 8²
    assert(result.values[4] == 100); // 10²
    assert(result.sum == 220);

    std::cout << "process_data 测试通过\n";

    // 测试 STL 版本
    auto result2 = process_data_stl(input);
    assert(result2.sum == result.sum);
    std::cout << "process_data_stl 测试通过\n";

    // 运行完整管道
    std::cout << "\n运行 run_pipeline:\n";
    run_pipeline();
    std::cout << "\n";
}

// ============================================
// 额外示例：更复杂的副作用分离
// ============================================

// 纯函数：验证和转换
struct ValidationResult {
    bool valid;
    std::string error_message;
    int processed_value;
};

[[nodiscard]] ValidationResult validate_and_process(int input) {
    if (input < 0) {
        return {false, "输入不能为负数", 0};
    }
    if (input > 100) {
        return {false, "输入不能大于100", 0};
    }
    return {true, "", input * 2};
}

// 不纯：处理结果并输出
void handle_user_input(int input) {
    auto result = validate_and_process(input);

    if (!result.valid) {
        std::cerr << "错误: " << result.error_message << "\n";
        return;
    }

    std::cout << "处理结果: " << result.processed_value << "\n";
}

void bonus_demo() {
    std::cout << "=== 额外示例：验证与处理分离 ===\n";

    // 纯函数可以独立测试
    auto r1 = validate_and_process(50);
    assert(r1.valid && r1.processed_value == 100);

    auto r2 = validate_and_process(-5);
    assert(!r2.valid);

    auto r3 = validate_and_process(150);
    assert(!r3.valid);

    std::cout << "验证逻辑测试通过\n";

    // I/O 部分
    handle_user_input(50);
    handle_user_input(-5);
    std::cout << "\n";
}

// ============================================
// main
// ============================================

int main() {
    std::cout << "========================================\n";
    std::cout << "    第03章练习参考答案\n";
    std::cout << "========================================\n\n";

    test_exercise_1();
    test_exercise_2();
    test_exercise_3();
    test_exercise_4();
    bonus_demo();

    std::cout << "========================================\n";
    std::cout << "    所有练习完成!\n";
    std::cout << "========================================\n";

    return 0;
}
