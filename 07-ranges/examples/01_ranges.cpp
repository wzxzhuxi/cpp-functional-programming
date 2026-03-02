/**
 * @file 01_ranges.cpp
 * @brief C++20 Ranges 示例
 *
 * 本文件演示：
 * - 基本视图适配器
 * - 管道语法
 * - 惰性求值
 * - 范围算法
 * - 实际应用
 */

#include <algorithm>
#include <iostream>
#include <map>
#include <numeric>
#include <ranges>
#include <string>
#include <vector>

namespace views = std::views;
namespace ranges = std::ranges;

// ============================================
// 1. 基本视图适配器
// ============================================

void basic_views_demo() {
    std::cout << "=== 基本视图适配器 ===\n";

    std::vector<int> nums = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    // filter：过滤
    std::cout << "偶数: ";
    for (int n : nums | views::filter([](int x) { return x % 2 == 0; })) {
        std::cout << n << " ";
    }
    std::cout << "\n";

    // transform：转换
    std::cout << "平方: ";
    for (int n : nums | views::transform([](int x) { return x * x; })) {
        std::cout << n << " ";
    }
    std::cout << "\n";

    // take：取前 N 个
    std::cout << "前5个: ";
    for (int n : nums | views::take(5)) {
        std::cout << n << " ";
    }
    std::cout << "\n";

    // drop：跳过前 N 个
    std::cout << "跳过前3个: ";
    for (int n : nums | views::drop(3)) {
        std::cout << n << " ";
    }
    std::cout << "\n";

    // reverse：反转
    std::cout << "反转: ";
    for (int n : nums | views::reverse) {
        std::cout << n << " ";
    }
    std::cout << "\n\n";
}

// ============================================
// 2. 管道组合
// ============================================

void pipeline_demo() {
    std::cout << "=== 管道组合 ===\n";

    std::vector<int> nums = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    // 组合多个视图：过滤偶数 -> 平方 -> 取前3个
    auto result = nums
        | views::filter([](int x) { return x % 2 == 0; })
        | views::transform([](int x) { return x * x; })
        | views::take(3);

    std::cout << "偶数平方的前3个: ";
    for (int n : result) {
        std::cout << n << " ";
    }
    std::cout << "\n";

    // 更复杂的管道
    auto complex = nums
        | views::filter([](int x) { return x > 3; })      // {4,5,6,7,8,9,10}
        | views::transform([](int x) { return x * 2; })   // {8,10,12,14,16,18,20}
        | views::drop(2)                                   // {12,14,16,18,20}
        | views::take(3);                                  // {12,14,16}

    std::cout << "复杂管道: ";
    for (int n : complex) {
        std::cout << n << " ";
    }
    std::cout << "\n\n";
}

// ============================================
// 3. 惰性求值演示
// ============================================

void lazy_evaluation_demo() {
    std::cout << "=== 惰性求值 ===\n";

    std::vector<int> nums = {1, 2, 3, 4, 5};

    // 创建视图时不会执行任何计算
    auto expensive_view = nums
        | views::transform([](int n) {
            std::cout << "  [计算 " << n << " 的平方]" << "\n";
            return n * n;
        })
        | views::take(2);

    std::cout << "视图已创建，但还没有计算...\n";
    std::cout << "开始迭代:\n";

    for (int x : expensive_view) {
        std::cout << "  结果: " << x << "\n";
    }

    std::cout << "（注意：只计算了2个元素，不是5个）\n\n";
}

// ============================================
// 4. views::iota - 生成序列
// ============================================

void iota_demo() {
    std::cout << "=== views::iota ===\n";

    // 有界序列 [0, 10)
    std::cout << "0到9: ";
    for (int n : views::iota(0, 10)) {
        std::cout << n << " ";
    }
    std::cout << "\n";

    // 无限序列 + take
    std::cout << "从100开始的5个数: ";
    for (int n : views::iota(100) | views::take(5)) {
        std::cout << n << " ";
    }
    std::cout << "\n";

    // 配合其他视图
    std::cout << "前10个偶数: ";
    for (int n : views::iota(0)
                 | views::filter([](int x) { return x % 2 == 0; })
                 | views::take(10)) {
        std::cout << n << " ";
    }
    std::cout << "\n\n";
}

// ============================================
// 5. 字符串和容器视图
// ============================================

void string_views_demo() {
    std::cout << "=== 字符串视图 ===\n";

    std::string text = "Hello,World,Ranges,Are,Great";

    // split：分割
    std::cout << "用逗号分割: ";
    for (auto part : text | views::split(',')) {
        // part 是一个 subrange，需要转换
        std::cout << "\"";
        for (char c : part) {
            std::cout << c;
        }
        std::cout << "\" ";
    }
    std::cout << "\n";

    // 字符过滤
    std::cout << "只保留大写: ";
    for (char c : text | views::filter([](char c) {
        return std::isupper(static_cast<unsigned char>(c));
    })) {
        std::cout << c;
    }
    std::cout << "\n\n";
}

// ============================================
// 6. 嵌套容器处理
// ============================================

void nested_containers_demo() {
    std::cout << "=== 嵌套容器 ===\n";

    std::vector<std::vector<int>> nested = {
        {1, 2, 3},
        {4, 5},
        {6, 7, 8, 9}
    };

    // join：展平
    std::cout << "展平: ";
    for (int n : nested | views::join) {
        std::cout << n << " ";
    }
    std::cout << "\n";

    // 展平后再处理
    std::cout << "展平后取偶数: ";
    for (int n : nested | views::join
                        | views::filter([](int x) { return x % 2 == 0; })) {
        std::cout << n << " ";
    }
    std::cout << "\n\n";
}

// ============================================
// 7. keys / values 视图
// ============================================

void keys_values_demo() {
    std::cout << "=== keys / values ===\n";

    std::map<std::string, int> scores = {
        {"Alice", 95},
        {"Bob", 87},
        {"Charlie", 92}
    };

    // 只获取键
    std::cout << "学生名单: ";
    for (const auto& name : scores | views::keys) {
        std::cout << name << " ";
    }
    std::cout << "\n";

    // 只获取值
    std::cout << "分数列表: ";
    for (int score : scores | views::values) {
        std::cout << score << " ";
    }
    std::cout << "\n";

    // 过滤后获取键
    std::cout << "90分以上的学生: ";
    for (const auto& [name, score] : scores
         | views::filter([](const auto& p) { return p.second >= 90; })) {
        std::cout << name << " ";
    }
    std::cout << "\n\n";
}

// ============================================
// 8. 范围算法
// ============================================

void range_algorithms_demo() {
    std::cout << "=== 范围算法 ===\n";

    std::vector<int> nums = {3, 1, 4, 1, 5, 9, 2, 6};

    // ranges::sort
    auto sorted = nums;  // 复制一份
    ranges::sort(sorted);
    std::cout << "排序后: ";
    for (int n : sorted) std::cout << n << " ";
    std::cout << "\n";

    // ranges::find
    auto it = ranges::find(nums, 5);
    if (it != nums.end()) {
        std::cout << "找到5，位置: " << std::distance(nums.begin(), it) << "\n";
    }

    // ranges::count_if
    auto even_count = ranges::count_if(nums, [](int x) { return x % 2 == 0; });
    std::cout << "偶数个数: " << even_count << "\n";

    // ranges::any_of / all_of / none_of
    bool has_negative = ranges::any_of(nums, [](int x) { return x < 0; });
    bool all_positive = ranges::all_of(nums, [](int x) { return x > 0; });
    std::cout << "有负数: " << (has_negative ? "是" : "否") << "\n";
    std::cout << "全正数: " << (all_positive ? "是" : "否") << "\n";

    // ranges::minmax
    auto [min_val, max_val] = ranges::minmax(nums);
    std::cout << "最小值: " << min_val << ", 最大值: " << max_val << "\n\n";
}

// ============================================
// 9. 投影 (Projection)
// ============================================

struct Person {
    std::string name;
    int age;
    double salary;
};

void projection_demo() {
    std::cout << "=== 投影 ===\n";

    std::vector<Person> people = {
        {"Alice", 30, 50000},
        {"Bob", 25, 45000},
        {"Charlie", 35, 60000},
        {"Diana", 28, 55000}
    };

    // 按年龄排序
    ranges::sort(people, {}, &Person::age);
    std::cout << "按年龄排序:\n";
    for (const auto& p : people) {
        std::cout << "  " << p.name << ": " << p.age << "岁\n";
    }

    // 按薪资降序排序
    ranges::sort(people, std::greater{}, &Person::salary);
    std::cout << "按薪资降序:\n";
    for (const auto& p : people) {
        std::cout << "  " << p.name << ": $" << p.salary << "\n";
    }

    // 查找年龄最大的人
    auto oldest = ranges::max_element(people, {}, &Person::age);
    std::cout << "年龄最大: " << oldest->name << " (" << oldest->age << "岁)\n\n";
}

// ============================================
// 10. 收集结果到容器
// ============================================

void collect_demo() {
    std::cout << "=== 收集结果 ===\n";

    std::vector<int> nums = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    // 视图是惰性的，需要收集到容器
    auto view = nums
        | views::filter([](int x) { return x % 2 == 0; })
        | views::transform([](int x) { return x * x; });

    // C++20 方式：使用范围构造
    std::vector<int> result1(view.begin(), view.end());
    std::cout << "方式1（范围构造）: ";
    for (int n : result1) std::cout << n << " ";
    std::cout << "\n";

    // 使用 ranges::copy
    std::vector<int> result2;
    ranges::copy(view, std::back_inserter(result2));
    std::cout << "方式2（ranges::copy）: ";
    for (int n : result2) std::cout << n << " ";
    std::cout << "\n";

#if __cpp_lib_ranges_to_container >= 202202L
    // C++23: ranges::to
    auto result3 = nums
        | views::filter([](int x) { return x % 2 == 0; })
        | views::transform([](int x) { return x * x; })
        | ranges::to<std::vector>();
    std::cout << "方式3（C++23 to<>）: ";
    for (int n : result3) std::cout << n << " ";
    std::cout << "\n";
#endif

    std::cout << "\n";
}

// ============================================
// 11. 实际应用：数据处理
// ============================================

struct Employee {
    std::string name;
    std::string department;
    double salary;
};

void data_processing_demo() {
    std::cout << "=== 实际应用：员工数据处理 ===\n";

    std::vector<Employee> employees = {
        {"Alice", "Engineering", 80000},
        {"Bob", "Engineering", 75000},
        {"Charlie", "Sales", 60000},
        {"Diana", "Engineering", 90000},
        {"Eve", "Sales", 55000},
        {"Frank", "HR", 50000}
    };

    // 获取工程部门的薪资
    auto eng_salaries = employees
        | views::filter([](const Employee& e) {
            return e.department == "Engineering";
        })
        | views::transform(&Employee::salary);

    // 计算平均薪资
    double total = 0;
    int count = 0;
    for (double s : eng_salaries) {
        total += s;
        count++;
    }
    double avg = count > 0 ? total / count : 0;
    std::cout << "工程部平均薪资: $" << avg << "\n";

    // 获取高薪员工名单（薪资 > 70000）
    std::cout << "高薪员工: ";
    for (const auto& name : employees
         | views::filter([](const Employee& e) { return e.salary > 70000; })
         | views::transform(&Employee::name)) {
        std::cout << name << " ";
    }
    std::cout << "\n";

    // 按薪资排序获取前3名
    auto sorted_employees = employees;
    ranges::sort(sorted_employees, std::greater{}, &Employee::salary);
    std::cout << "薪资前3名:\n";
    for (const auto& e : sorted_employees | views::take(3)) {
        std::cout << "  " << e.name << ": $" << e.salary << "\n";
    }
    std::cout << "\n";
}

// ============================================
// 12. 自定义视图适配器
// ============================================

void custom_adapters_demo() {
    std::cout << "=== 自定义适配器 ===\n";

    // 简单的自定义适配器：使用 views::transform 封装
    auto square = views::transform([](auto x) { return x * x; });
    auto double_it = views::transform([](auto x) { return x * 2; });
    auto negate = views::transform([](auto x) { return -x; });

    std::vector<int> nums = {1, 2, 3, 4, 5};

    std::cout << "原始: ";
    for (int n : nums) std::cout << n << " ";
    std::cout << "\n";

    std::cout << "square: ";
    for (int n : nums | square) std::cout << n << " ";
    std::cout << "\n";

    std::cout << "square | double: ";
    for (int n : nums | square | double_it) std::cout << n << " ";
    std::cout << "\n";

    std::cout << "square | negate: ";
    for (int n : nums | square | negate) std::cout << n << " ";
    std::cout << "\n\n";
}

// ============================================
// 13. 性能对比
// ============================================

void performance_comparison() {
    std::cout << "=== 性能对比（概念演示）===\n";

    std::vector<int> nums(1000);
    std::iota(nums.begin(), nums.end(), 1);

    // 传统方式：创建中间容器
    std::cout << "传统方式：需要创建中间容器\n";
    std::vector<int> step1;
    for (int n : nums) {
        if (n % 2 == 0) {
            step1.push_back(n);
        }
    }
    std::vector<int> step2;
    for (int n : step1) {
        step2.push_back(n * n);
    }
    int trad_sum = 0;
    for (size_t i = 0; i < 10 && i < step2.size(); ++i) {
        trad_sum += step2[i];
    }
    std::cout << "  前10个偶数平方和: " << trad_sum << "\n";

    // Ranges 方式：零中间容器
    std::cout << "Ranges方式：惰性求值，零中间容器\n";
    int range_sum = 0;
    for (int n : nums
         | views::filter([](int x) { return x % 2 == 0; })
         | views::transform([](int x) { return x * x; })
         | views::take(10)) {
        range_sum += n;
    }
    std::cout << "  前10个偶数平方和: " << range_sum << "\n";

    std::cout << "（两种方式结果相同，但Ranges更高效）\n\n";
}

// ============================================
// main
// ============================================

int main() {
    basic_views_demo();
    pipeline_demo();
    lazy_evaluation_demo();
    iota_demo();
    string_views_demo();
    nested_containers_demo();
    keys_values_demo();
    range_algorithms_demo();
    projection_demo();
    collect_demo();
    data_processing_demo();
    custom_adapters_demo();
    performance_comparison();

    return 0;
}
