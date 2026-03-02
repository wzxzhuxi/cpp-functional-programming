/**
 * @file exercises.cpp
 * @brief 第07章练习题：C++20 Ranges
 *
 * 完成下面的练习，然后运行程序验证结果。
 * 参考答案在 solutions.cpp
 */

#include <algorithm>
#include <cassert>
#include <iostream>
#include <map>
#include <numeric>
#include <ranges>
#include <string>
#include <vector>

namespace views = std::views;
namespace ranges = std::ranges;

// ============================================
// 练习 1: 基本管道操作
// ============================================

/**
 * 使用 Ranges 实现以下功能：
 * 给定一个整数向量，返回所有大于 threshold 的元素的平方
 *
 * 示例：
 *   squares_above({1,2,3,4,5}, 2) = {9, 16, 25}
 */
[[nodiscard]] std::vector<int> squares_above(
    const std::vector<int>& nums,
    int threshold
) {
    // TODO: 使用 views::filter 和 views::transform
    // 提示：
    // 1. 先过滤出 > threshold 的元素
    // 2. 再对每个元素求平方
    // 3. 收集到 vector
    return {};  // 修改这行
}

void test_exercise_1() {
    std::cout << "=== 练习 1: 基本管道操作 ===\n";

    auto result = squares_above({1, 2, 3, 4, 5}, 2);

    assert(result.size() == 3);
    assert(result[0] == 9);   // 3*3
    assert(result[1] == 16);  // 4*4
    assert(result[2] == 25);  // 5*5

    std::cout << "squares_above({1,2,3,4,5}, 2) = {";
    for (size_t i = 0; i < result.size(); ++i) {
        if (i > 0) std::cout << ", ";
        std::cout << result[i];
    }
    std::cout << "}\n";

    std::cout << "练习 1 通过\n\n";
}

// ============================================
// 练习 2: 字符串处理
// ============================================

/**
 * 从逗号分隔的字符串中提取长度大于 min_len 的单词
 *
 * 示例：
 *   long_words("a,hello,hi,world,x", 2) = {"hello", "world"}
 */
[[nodiscard]] std::vector<std::string> long_words(
    const std::string& csv,
    size_t min_len
) {
    // TODO: 使用 views::split 和 views::filter
    // 提示：
    // 1. 用逗号分割字符串
    // 2. 过滤出长度 > min_len 的部分
    // 3. 将每个部分转换为 string
    return {};  // 修改这行
}

void test_exercise_2() {
    std::cout << "=== 练习 2: 字符串处理 ===\n";

    auto result = long_words("a,hello,hi,world,x", 2);

    assert(result.size() == 2);
    assert(result[0] == "hello");
    assert(result[1] == "world");

    std::cout << "long_words(\"a,hello,hi,world,x\", 2) = {";
    for (size_t i = 0; i < result.size(); ++i) {
        if (i > 0) std::cout << ", ";
        std::cout << "\"" << result[i] << "\"";
    }
    std::cout << "}\n";

    std::cout << "练习 2 通过\n\n";
}

// ============================================
// 练习 3: 嵌套数据展平
// ============================================

/**
 * 展平嵌套向量并返回前 n 个唯一元素（按出现顺序）
 *
 * 示例：
 *   flatten_unique({{1,2,2}, {2,3}, {3,4}}, 4) = {1, 2, 3, 4}
 */
[[nodiscard]] std::vector<int> flatten_unique(
    const std::vector<std::vector<int>>& nested,
    size_t n
) {
    // TODO: 使用 views::join
    // 提示：
    // 1. 先用 join 展平
    // 2. 手动去重（保持顺序）
    // 3. 取前 n 个
    return {};  // 修改这行
}

void test_exercise_3() {
    std::cout << "=== 练习 3: 嵌套数据展平 ===\n";

    std::vector<std::vector<int>> nested = {{1, 2, 2}, {2, 3}, {3, 4, 5}};
    auto result = flatten_unique(nested, 4);

    assert(result.size() == 4);
    assert(result[0] == 1);
    assert(result[1] == 2);
    assert(result[2] == 3);
    assert(result[3] == 4);

    std::cout << "flatten_unique 结果: {";
    for (size_t i = 0; i < result.size(); ++i) {
        if (i > 0) std::cout << ", ";
        std::cout << result[i];
    }
    std::cout << "}\n";

    std::cout << "练习 3 通过\n\n";
}

// ============================================
// 练习 4: 员工数据分析
// ============================================

struct Employee {
    std::string name;
    std::string department;
    double salary;
};

/**
 * 获取指定部门中薪资最高的 n 名员工的名字
 *
 * 示例：
 *   top_earners(employees, "Engineering", 2) = {"Diana", "Alice"}
 */
[[nodiscard]] std::vector<std::string> top_earners(
    std::vector<Employee> employees,  // 值传递以便排序
    const std::string& department,
    size_t n
) {
    // TODO: 使用 ranges 算法和视图
    // 提示：
    // 1. 过滤指定部门
    // 2. 按薪资降序排序
    // 3. 取前 n 个
    // 4. 提取名字
    return {};  // 修改这行
}

void test_exercise_4() {
    std::cout << "=== 练习 4: 员工数据分析 ===\n";

    std::vector<Employee> employees = {
        {"Alice", "Engineering", 80000},
        {"Bob", "Engineering", 75000},
        {"Charlie", "Sales", 60000},
        {"Diana", "Engineering", 90000},
        {"Eve", "Sales", 55000}
    };

    auto result = top_earners(employees, "Engineering", 2);

    assert(result.size() == 2);
    assert(result[0] == "Diana");  // 90000
    assert(result[1] == "Alice");  // 80000

    std::cout << "工程部薪资前2名: {";
    for (size_t i = 0; i < result.size(); ++i) {
        if (i > 0) std::cout << ", ";
        std::cout << "\"" << result[i] << "\"";
    }
    std::cout << "}\n";

    std::cout << "练习 4 通过\n\n";
}

// ============================================
// 练习 5: 自定义视图适配器
// ============================================

/**
 * 创建一个生成斐波那契数列前 n 项的视图
 *
 * 使用 views::iota 配合 views::transform 实现
 */
[[nodiscard]] std::vector<long long> fibonacci(size_t n) {
    // TODO: 使用惰性视图生成斐波那契数列
    // 提示：
    // 1. 使用 views::iota(0, n)
    // 2. 使用 mutable lambda 缓存前两项
    // 3. 或者用简单循环，但要体现 ranges 思想
    return {};  // 修改这行
}

void test_exercise_5() {
    std::cout << "=== 练习 5: 斐波那契数列 ===\n";

    auto result = fibonacci(10);

    std::vector<long long> expected = {0, 1, 1, 2, 3, 5, 8, 13, 21, 34};
    assert(result == expected);

    std::cout << "fibonacci(10) = {";
    for (size_t i = 0; i < result.size(); ++i) {
        if (i > 0) std::cout << ", ";
        std::cout << result[i];
    }
    std::cout << "}\n";

    std::cout << "练习 5 通过\n\n";
}

// ============================================
// 练习 6: 数据统计管道
// ============================================

struct Stats {
    double min;
    double max;
    double sum;
    double avg;
    size_t count;
};

/**
 * 计算满足条件的元素的统计信息
 *
 * 只统计 predicate 返回 true 的元素
 */
template<typename Pred>
[[nodiscard]] Stats compute_stats(
    const std::vector<double>& data,
    Pred predicate
) {
    // TODO: 使用 ranges 过滤并计算统计
    // 提示：
    // 1. 过滤满足条件的数据
    // 2. 使用 ranges::minmax 获取极值
    // 3. 计算总和和平均值
    return Stats{0, 0, 0, 0, 0};  // 修改这行
}

void test_exercise_6() {
    std::cout << "=== 练习 6: 数据统计管道 ===\n";

    std::vector<double> data = {1.0, -2.0, 3.0, -4.0, 5.0, -6.0, 7.0};

    // 只统计正数
    auto stats = compute_stats(data, [](double x) { return x > 0; });

    assert(stats.count == 4);
    assert(stats.min == 1.0);
    assert(stats.max == 7.0);
    assert(stats.sum == 16.0);
    assert(stats.avg == 4.0);

    std::cout << "正数统计:\n";
    std::cout << "  count: " << stats.count << "\n";
    std::cout << "  min: " << stats.min << "\n";
    std::cout << "  max: " << stats.max << "\n";
    std::cout << "  sum: " << stats.sum << "\n";
    std::cout << "  avg: " << stats.avg << "\n";

    std::cout << "练习 6 通过\n\n";
}

// ============================================
// main
// ============================================

int main() {
    std::cout << "========================================\n";
    std::cout << "    第07章练习：C++20 Ranges\n";
    std::cout << "========================================\n\n";

    test_exercise_1();
    test_exercise_2();
    test_exercise_3();
    test_exercise_4();
    test_exercise_5();
    test_exercise_6();

    std::cout << "========================================\n";
    std::cout << "    所有练习完成!\n";
    std::cout << "========================================\n";

    return 0;
}
