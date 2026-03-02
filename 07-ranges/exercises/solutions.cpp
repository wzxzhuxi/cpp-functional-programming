/**
 * @file solutions.cpp
 * @brief 第07章练习题参考答案
 */

#include <algorithm>
#include <cassert>
#include <iostream>
#include <map>
#include <numeric>
#include <ranges>
#include <set>
#include <string>
#include <vector>

namespace views = std::views;
namespace ranges = std::ranges;

// ============================================
// 练习 1: 基本管道操作 - 答案
// ============================================

[[nodiscard]] std::vector<int> squares_above(
    const std::vector<int>& nums,
    int threshold
) {
    auto view = nums
        | views::filter([threshold](int x) { return x > threshold; })
        | views::transform([](int x) { return x * x; });

    return std::vector<int>(view.begin(), view.end());
}

void test_exercise_1() {
    std::cout << "=== 练习 1: 基本管道操作 - 答案 ===\n";

    auto result = squares_above({1, 2, 3, 4, 5}, 2);

    assert(result.size() == 3);
    assert(result[0] == 9);
    assert(result[1] == 16);
    assert(result[2] == 25);

    std::cout << "squares_above({1,2,3,4,5}, 2) = {";
    for (size_t i = 0; i < result.size(); ++i) {
        if (i > 0) std::cout << ", ";
        std::cout << result[i];
    }
    std::cout << "}\n";

    // 额外示例
    auto result2 = squares_above({10, 20, 30, 40}, 25);
    std::cout << "squares_above({10,20,30,40}, 25) = {";
    for (size_t i = 0; i < result2.size(); ++i) {
        if (i > 0) std::cout << ", ";
        std::cout << result2[i];
    }
    std::cout << "}\n";

    std::cout << "练习 1 通过\n\n";
}

// ============================================
// 练习 2: 字符串处理 - 答案
// ============================================

[[nodiscard]] std::vector<std::string> long_words(
    const std::string& csv,
    size_t min_len
) {
    std::vector<std::string> result;

    for (auto part : csv | views::split(',')) {
        std::string word(part.begin(), part.end());
        if (word.length() > min_len) {
            result.push_back(word);
        }
    }

    return result;
}

// 替代实现：更函数式的风格
[[nodiscard]] std::vector<std::string> long_words_v2(
    const std::string& csv,
    size_t min_len
) {
    std::vector<std::string> result;

    auto parts = csv | views::split(',');
    for (auto part : parts) {
        // 计算长度
        auto len = ranges::distance(part);
        if (static_cast<size_t>(len) > min_len) {
            result.emplace_back(part.begin(), part.end());
        }
    }

    return result;
}

void test_exercise_2() {
    std::cout << "=== 练习 2: 字符串处理 - 答案 ===\n";

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

    // 额外示例
    auto result2 = long_words("cat,dog,elephant,ant,hippopotamus", 4);
    std::cout << "长度>4的单词: {";
    for (size_t i = 0; i < result2.size(); ++i) {
        if (i > 0) std::cout << ", ";
        std::cout << "\"" << result2[i] << "\"";
    }
    std::cout << "}\n";

    std::cout << "练习 2 通过\n\n";
}

// ============================================
// 练习 3: 嵌套数据展平 - 答案
// ============================================

[[nodiscard]] std::vector<int> flatten_unique(
    const std::vector<std::vector<int>>& nested,
    size_t n
) {
    std::vector<int> result;
    std::set<int> seen;

    for (int val : nested | views::join) {
        if (seen.find(val) == seen.end()) {
            seen.insert(val);
            result.push_back(val);
            if (result.size() >= n) {
                break;
            }
        }
    }

    return result;
}

// 替代实现：先收集再去重
[[nodiscard]] std::vector<int> flatten_unique_v2(
    const std::vector<std::vector<int>>& nested,
    size_t n
) {
    std::vector<int> result;
    result.reserve(n);

    auto flat = nested | views::join;
    std::set<int> seen;

    for (int val : flat) {
        if (result.size() >= n) break;
        if (seen.insert(val).second) {
            result.push_back(val);
        }
    }

    return result;
}

void test_exercise_3() {
    std::cout << "=== 练习 3: 嵌套数据展平 - 答案 ===\n";

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

    // 额外示例
    std::vector<std::vector<int>> nested2 = {{1, 1, 1}, {1, 2, 2}, {2, 3}};
    auto result2 = flatten_unique(nested2, 10);
    std::cout << "全部唯一元素: {";
    for (size_t i = 0; i < result2.size(); ++i) {
        if (i > 0) std::cout << ", ";
        std::cout << result2[i];
    }
    std::cout << "}\n";

    std::cout << "练习 3 通过\n\n";
}

// ============================================
// 练习 4: 员工数据分析 - 答案
// ============================================

struct Employee {
    std::string name;
    std::string department;
    double salary;
};

[[nodiscard]] std::vector<std::string> top_earners(
    std::vector<Employee> employees,
    const std::string& department,
    size_t n
) {
    // 1. 过滤指定部门
    std::vector<Employee> dept_employees;
    for (const auto& e : employees) {
        if (e.department == department) {
            dept_employees.push_back(e);
        }
    }

    // 2. 按薪资降序排序
    ranges::sort(dept_employees, std::greater{}, &Employee::salary);

    // 3. 取前 n 个并提取名字
    std::vector<std::string> result;
    for (const auto& e : dept_employees | views::take(n)) {
        result.push_back(e.name);
    }

    return result;
}

// 替代实现：更紧凑的写法
[[nodiscard]] std::vector<std::string> top_earners_v2(
    std::vector<Employee> employees,
    const std::string& department,
    size_t n
) {
    // 先排序整个数组
    ranges::sort(employees, std::greater{}, &Employee::salary);

    std::vector<std::string> result;
    size_t count = 0;

    for (const auto& e : employees) {
        if (e.department == department) {
            result.push_back(e.name);
            if (++count >= n) break;
        }
    }

    return result;
}

void test_exercise_4() {
    std::cout << "=== 练习 4: 员工数据分析 - 答案 ===\n";

    std::vector<Employee> employees = {
        {"Alice", "Engineering", 80000},
        {"Bob", "Engineering", 75000},
        {"Charlie", "Sales", 60000},
        {"Diana", "Engineering", 90000},
        {"Eve", "Sales", 55000}
    };

    auto result = top_earners(employees, "Engineering", 2);

    assert(result.size() == 2);
    assert(result[0] == "Diana");
    assert(result[1] == "Alice");

    std::cout << "工程部薪资前2名: {";
    for (size_t i = 0; i < result.size(); ++i) {
        if (i > 0) std::cout << ", ";
        std::cout << "\"" << result[i] << "\"";
    }
    std::cout << "}\n";

    // 销售部测试
    auto sales_top = top_earners(employees, "Sales", 2);
    std::cout << "销售部薪资前2名: {";
    for (size_t i = 0; i < sales_top.size(); ++i) {
        if (i > 0) std::cout << ", ";
        std::cout << "\"" << sales_top[i] << "\"";
    }
    std::cout << "}\n";

    std::cout << "练习 4 通过\n\n";
}

// ============================================
// 练习 5: 斐波那契数列 - 答案
// ============================================

[[nodiscard]] std::vector<long long> fibonacci(size_t n) {
    if (n == 0) return {};

    // 方法：预计算后使用 views::take
    std::vector<long long> cache = {0, 1};
    while (cache.size() < n) {
        cache.push_back(cache.back() + cache[cache.size() - 2]);
    }

    // 使用 views::take 截取
    std::vector<long long> result;
    result.reserve(n);
    for (auto val : cache | views::take(n)) {
        result.push_back(val);
    }
    return result;
}

// 替代实现：更纯净的方式
[[nodiscard]] std::vector<long long> fibonacci_v2(size_t n) {
    if (n == 0) return {};

    std::vector<long long> result;
    result.reserve(n);

    long long a = 0, b = 1;
    for (size_t i = 0; i < n; ++i) {
        result.push_back(a);
        long long next = a + b;
        a = b;
        b = next;
    }

    return result;
}

// 方法3：使用 views::iota 配合索引计算
[[nodiscard]] std::vector<long long> fibonacci_v3(size_t n) {
    // 预计算所有值
    std::vector<long long> all_fibs = {0, 1};
    while (all_fibs.size() < n) {
        all_fibs.push_back(all_fibs.back() + all_fibs[all_fibs.size() - 2]);
    }

    // 使用 views::take
    auto view = all_fibs | views::take(n);
    return std::vector<long long>(view.begin(), view.end());
}

void test_exercise_5() {
    std::cout << "=== 练习 5: 斐波那契数列 - 答案 ===\n";

    auto result = fibonacci(10);

    std::vector<long long> expected = {0, 1, 1, 2, 3, 5, 8, 13, 21, 34};
    assert(result == expected);

    std::cout << "fibonacci(10) = {";
    for (size_t i = 0; i < result.size(); ++i) {
        if (i > 0) std::cout << ", ";
        std::cout << result[i];
    }
    std::cout << "}\n";

    // 更多测试
    auto fib20 = fibonacci(20);
    std::cout << "fibonacci(20) 最后5项: {";
    for (size_t i = 15; i < 20; ++i) {
        if (i > 15) std::cout << ", ";
        std::cout << fib20[i];
    }
    std::cout << "}\n";

    std::cout << "练习 5 通过\n\n";
}

// ============================================
// 练习 6: 数据统计管道 - 答案
// ============================================

struct Stats {
    double min;
    double max;
    double sum;
    double avg;
    size_t count;
};

template<typename Pred>
[[nodiscard]] Stats compute_stats(
    const std::vector<double>& data,
    Pred predicate
) {
    // 收集满足条件的数据
    std::vector<double> filtered;
    for (double x : data | views::filter(predicate)) {
        filtered.push_back(x);
    }

    if (filtered.empty()) {
        return Stats{0, 0, 0, 0, 0};
    }

    auto [min_it, max_it] = ranges::minmax_element(filtered);
    double sum = std::accumulate(filtered.begin(), filtered.end(), 0.0);

    return Stats{
        .min = *min_it,
        .max = *max_it,
        .sum = sum,
        .avg = sum / static_cast<double>(filtered.size()),
        .count = filtered.size()
    };
}

// 替代实现：不收集中间结果（更高效但需要多次遍历）
template<typename Pred>
[[nodiscard]] Stats compute_stats_v2(
    const std::vector<double>& data,
    Pred predicate
) {
    auto filtered = data | views::filter(predicate);

    // 检查是否为空
    auto it = filtered.begin();
    if (it == filtered.end()) {
        return Stats{0, 0, 0, 0, 0};
    }

    double min_val = *it;
    double max_val = *it;
    double sum = 0;
    size_t count = 0;

    for (double x : filtered) {
        min_val = std::min(min_val, x);
        max_val = std::max(max_val, x);
        sum += x;
        count++;
    }

    return Stats{
        .min = min_val,
        .max = max_val,
        .sum = sum,
        .avg = sum / static_cast<double>(count),
        .count = count
    };
}

void test_exercise_6() {
    std::cout << "=== 练习 6: 数据统计管道 - 答案 ===\n";

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

    // 负数统计
    auto neg_stats = compute_stats(data, [](double x) { return x < 0; });
    std::cout << "\n负数统计:\n";
    std::cout << "  count: " << neg_stats.count << "\n";
    std::cout << "  min: " << neg_stats.min << "\n";
    std::cout << "  max: " << neg_stats.max << "\n";
    std::cout << "  sum: " << neg_stats.sum << "\n";
    std::cout << "  avg: " << neg_stats.avg << "\n";

    std::cout << "练习 6 通过\n\n";
}

// ============================================
// 额外示例：更多 Ranges 技巧
// ============================================

void bonus_demo() {
    std::cout << "=== 额外示例 ===\n";

    // 1. 生成等差数列
    std::cout << "等差数列 (首项=1, 公差=3, 10项): ";
    for (int n : views::iota(0, 10) | views::transform([](int i) { return 1 + i * 3; })) {
        std::cout << n << " ";
    }
    std::cout << "\n";

    // 2. 笛卡尔积模拟（嵌套循环）
    std::cout << "坐标对 (0-2, 0-2): ";
    for (int x : views::iota(0, 3)) {
        for (int y : views::iota(0, 3)) {
            std::cout << "(" << x << "," << y << ") ";
        }
    }
    std::cout << "\n";

    // 3. 条件转换
    std::vector<int> nums = {1, 2, 3, 4, 5, 6};
    std::cout << "偶数翻倍，奇数不变: ";
    for (int n : nums | views::transform([](int x) {
        return x % 2 == 0 ? x * 2 : x;
    })) {
        std::cout << n << " ";
    }
    std::cout << "\n";

    // 4. 复合键排序
    struct Item {
        std::string category;
        int value;
    };
    std::vector<Item> items = {
        {"B", 1}, {"A", 2}, {"B", 2}, {"A", 1}
    };
    ranges::sort(items, [](const Item& a, const Item& b) {
        if (a.category != b.category) return a.category < b.category;
        return a.value < b.value;
    });
    std::cout << "复合排序后: ";
    for (const auto& item : items) {
        std::cout << "{" << item.category << "," << item.value << "} ";
    }
    std::cout << "\n\n";
}

// ============================================
// main
// ============================================

int main() {
    std::cout << "========================================\n";
    std::cout << "    第07章练习参考答案\n";
    std::cout << "========================================\n\n";

    test_exercise_1();
    test_exercise_2();
    test_exercise_3();
    test_exercise_4();
    test_exercise_5();
    test_exercise_6();
    bonus_demo();

    std::cout << "========================================\n";
    std::cout << "    所有练习完成!\n";
    std::cout << "========================================\n";

    return 0;
}
