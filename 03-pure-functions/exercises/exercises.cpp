/**
 * @file exercises.cpp
 * @brief 第03章练习题：纯函数
 *
 * 完成下面的练习，然后运行程序验证结果。
 * 参考答案在 solutions.cpp
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
// 练习 1: 判断纯度
// ============================================

/**
 * 判断以下函数是否为纯函数，在注释中解释原因
 * 纯函数需要满足：
 * 1. 相同输入永远得到相同输出
 * 2. 无副作用
 */

// 函数 A
int func_a(int x) {
    return x * 2;
}
// TODO: 是否纯函数？为什么？

// 函数 B
int counter_b = 0;
int func_b(int x) {
    return x + counter_b;
}
// TODO: 是否纯函数？为什么？

// 函数 C
int func_c(int x) {
    static int last = 0;
    int result = x + last;
    last = x;
    return result;
}
// TODO: 是否纯函数？为什么？

// 函数 D
void func_d(std::vector<int>& vec) {
    std::sort(vec.begin(), vec.end());
}
// TODO: 是否纯函数？为什么？

// 函数 E
int func_e(const std::vector<int>& vec) {
    return std::accumulate(vec.begin(), vec.end(), 0);
}
// TODO: 是否纯函数？为什么？

// 函数 F
int func_f(int x) {
    std::cout << "Input: " << x << "\n";
    return x * 2;
}
// TODO: 是否纯函数？为什么？

void test_exercise_1() {
    std::cout << "=== 练习 1: 判断纯度 ===\n";
    std::cout << "请在代码注释中填写每个函数是否为纯函数\n";
    std::cout << "然后对照 solutions.cpp 检查答案\n\n";
}

// ============================================
// 练习 2: 重构为纯函数
// ============================================

/**
 * 将以下不纯函数重构为纯函数
 */

// 不纯版本 1：修改参数
void bad_scale(std::vector<int>& vec, int factor) {
    for (auto& x : vec) {
        x *= factor;
    }
}

// TODO: 实现纯函数版本
[[nodiscard]] std::vector<int> pure_scale(const std::vector<int>& vec, int factor) {
    // 你的代码写在这里
    return {};
}

// 不纯版本 2：依赖全局状态
int tax_rate = 10;  // 百分比
int calculate_tax_bad(int price) {
    return price * tax_rate / 100;
}

// TODO: 实现纯函数版本（税率作为参数传入）
[[nodiscard]] int calculate_tax_pure(int price, int rate_percent) {
    // 你的代码写在这里
    return 0;
}

// 不纯版本 3：修改全局状态
namespace stats {
    int sum = 0;
    int count = 0;

    void add_value_bad(int x) {
        sum += x;
        count++;
    }

    double get_average_bad() {
        return static_cast<double>(sum) / count;
    }
}

// TODO: 实现纯函数版本
struct Stats {
    int sum;
    int count;
};

[[nodiscard]] Stats add_value_pure(Stats current, int x) {
    // 你的代码写在这里
    return current;
}

[[nodiscard]] double get_average_pure(Stats s) {
    // 你的代码写在这里
    return 0.0;
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
    std::cout << "pure_scale 通过\n";

    // 测试 calculate_tax_pure
    assert(calculate_tax_pure(100, 10) == 10);
    assert(calculate_tax_pure(200, 15) == 30);
    std::cout << "calculate_tax_pure 通过\n";

    // 测试 Stats 纯函数
    Stats s = {0, 0};
    s = add_value_pure(s, 10);
    s = add_value_pure(s, 20);
    s = add_value_pure(s, 30);
    assert(s.sum == 60);
    assert(s.count == 3);
    assert(get_average_pure(s) == 20.0);
    std::cout << "Stats 纯函数 通过\n\n";
}

// ============================================
// 练习 3: 实现记忆化
// ============================================

/**
 * 实现一个带记忆化的斐波那契函数
 *
 * 要求：
 * 1. 不使用全局/静态变量
 * 2. 使用类或闭包封装缓存
 * 3. 对外表现为纯函数
 */

class MemoizedFibonacci {
    // TODO: 添加缓存成员
public:
    int operator()(int n) {
        // TODO: 实现带缓存的斐波那契
        if (n <= 1) return n;
        return (*this)(n-1) + (*this)(n-2);  // 替换这行
    }
};

void test_exercise_3() {
    std::cout << "=== 练习 3: 实现记忆化 ===\n";

    MemoizedFibonacci fib;

    // 基本测试
    assert(fib(0) == 0);
    assert(fib(1) == 1);
    assert(fib(10) == 55);
    std::cout << "fib(10) = " << fib(10) << "\n";

    // 性能测试：如果没有记忆化，这会很慢
    int result = fib(40);
    std::cout << "fib(40) = " << result << "\n";
    assert(result == 102334155);

    std::cout << "记忆化斐波那契 通过\n\n";
}

// ============================================
// 练习 4: 分离副作用
// ============================================

/**
 * 将以下混合逻辑拆分为纯函数 + I/O
 *
 * 原函数做了三件事：
 * 1. 读取输入
 * 2. 处理数据
 * 3. 输出结果
 *
 * 重构为：
 * - process_data: 纯函数，处理数据
 * - run_pipeline: 不纯，负责 I/O
 */

// 原始混合版本（不要修改）
void bad_pipeline() {
    // 模拟读取
    std::vector<int> data = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    // 处理（混在一起）
    std::vector<int> result;
    for (int x : data) {
        if (x % 2 == 0) {
            result.push_back(x * x);
            std::cout << "Processed: " << x * x << "\n";  // 副作用
        }
    }

    // 输出
    int sum = std::accumulate(result.begin(), result.end(), 0);
    std::cout << "Sum: " << sum << "\n";
}

// TODO: 实现纯函数部分
struct ProcessResult {
    std::vector<int> values;
    int sum;
};

[[nodiscard]] ProcessResult process_data(const std::vector<int>& input) {
    // 你的代码写在这里
    // 过滤偶数，平方，计算总和
    return {{}, 0};
}

// TODO: 实现 I/O 部分
void run_pipeline() {
    // 你的代码写在这里
    // 1. 创建输入数据
    // 2. 调用 process_data
    // 3. 输出结果
}

void test_exercise_4() {
    std::cout << "=== 练习 4: 分离副作用 ===\n";

    // 测试纯函数
    std::vector<int> input = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    auto result = process_data(input);

    // 偶数: 2, 4, 6, 8, 10
    // 平方: 4, 16, 36, 64, 100
    // 总和: 220
    assert(result.values.size() == 5);
    assert(result.values[0] == 4);
    assert(result.values[4] == 100);
    assert(result.sum == 220);

    std::cout << "process_data 通过\n";
    std::cout << "调用 run_pipeline:\n";
    run_pipeline();
    std::cout << "\n";
}

// ============================================
// main
// ============================================

int main() {
    std::cout << "========================================\n";
    std::cout << "    第03章练习：纯函数\n";
    std::cout << "========================================\n\n";

    test_exercise_1();
    test_exercise_2();
    test_exercise_3();
    test_exercise_4();

    std::cout << "========================================\n";
    std::cout << "    所有练习完成!\n";
    std::cout << "========================================\n";

    return 0;
}
