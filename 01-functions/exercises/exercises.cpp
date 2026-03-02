/**
 * @file exercises.cpp
 * @brief 第01章练习题：Lambda 和闭包
 *
 * 完成下面的练习，然后运行程序验证结果。
 * 参考答案在 solutions.cpp
 */

#include <algorithm>
#include <cassert>
#include <functional>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>

// ============================================
// 练习 1: filter_long_strings
// ============================================

/**
 * 实现一个函数，返回长度大于指定值的字符串
 *
 * 提示：使用 std::copy_if 和 lambda
 *
 * @param strings 输入字符串数组
 * @param min_length 最小长度（不包含）
 * @return 长度大于 min_length 的字符串
 */
std::vector<std::string> filter_long_strings(
    const std::vector<std::string>& strings,
    size_t min_length)
{
    // TODO: 实现这个函数
    std::vector<std::string> result;

    // 你的代码写在这里

    return result;
}

void test_exercise_1() {
    std::cout << "=== 练习 1: filter_long_strings ===\n";

    std::vector<std::string> words = {"hi", "hello", "functional", "cpp", "programming"};

    auto long_words = filter_long_strings(words, 5);

    std::cout << "长度 > 5 的单词: ";
    for (const auto& w : long_words) {
        std::cout << w << " ";
    }
    std::cout << "\n";

    // 验证
    assert(long_words.size() == 2);
    assert(long_words[0] == "functional");
    assert(long_words[1] == "programming");

    std::cout << "通过!\n\n";
}

// ============================================
// 练习 2: make_multiplier
// ============================================

/**
 * 实现一个函数工厂，返回一个"乘以 n"的函数
 *
 * 提示：返回一个捕获 n 的 lambda
 *
 * @param n 乘数
 * @return 返回一个函数 f，使得 f(x) = x * n
 */
auto make_multiplier(int n)
{
    // TODO: 实现这个函数
    // 返回一个 lambda，接受 int 参数，返回 int

    return [](int x) { return x; };  // 替换这行
}

void test_exercise_2() {
    std::cout << "=== 练习 2: make_multiplier ===\n";

    auto times2 = make_multiplier(2);
    auto times5 = make_multiplier(5);
    auto times10 = make_multiplier(10);

    std::cout << "times2(7) = " << times2(7) << "\n";
    std::cout << "times5(7) = " << times5(7) << "\n";
    std::cout << "times10(7) = " << times10(7) << "\n";

    // 验证
    assert(times2(7) == 14);
    assert(times5(7) == 35);
    assert(times10(7) == 70);

    std::cout << "通过!\n\n";
}

// ============================================
// 练习 3: sort_by_field
// ============================================

struct Person {
    std::string name;
    int age;
};

/**
 * 按年龄对 Person 数组排序（升序）
 *
 * 提示：使用 std::sort 和 lambda 比较器
 *
 * @param people 待排序数组（会被修改）
 */
void sort_by_age(std::vector<Person>& people)
{
    // TODO: 实现这个函数
    // 使用 std::sort 和 lambda

}

/**
 * 按姓名长度对 Person 数组排序（降序）
 *
 * @param people 待排序数组（会被修改）
 */
void sort_by_name_length_desc(std::vector<Person>& people)
{
    // TODO: 实现这个函数

}

void test_exercise_3() {
    std::cout << "=== 练习 3: sort_by_field ===\n";

    std::vector<Person> people = {
        {"Alice", 30},
        {"Bob", 25},
        {"Charlie", 35},
        {"David", 28}
    };

    // 测试按年龄排序
    sort_by_age(people);

    std::cout << "按年龄排序: ";
    for (const auto& p : people) {
        std::cout << p.name << "(" << p.age << ") ";
    }
    std::cout << "\n";

    assert(people[0].name == "Bob");
    assert(people[3].name == "Charlie");

    // 测试按姓名长度降序
    sort_by_name_length_desc(people);

    std::cout << "按姓名长度降序: ";
    for (const auto& p : people) {
        std::cout << p.name << " ";
    }
    std::cout << "\n";

    assert(people[0].name == "Charlie");

    std::cout << "通过!\n\n";
}

// ============================================
// 练习 4: chain_operations
// ============================================

/**
 * 组合操作：过滤偶数 → 平方 → 求和
 *
 * 给定一个整数数组，计算所有偶数的平方和。
 *
 * 提示：可以用一个 accumulate，或者分步 filter → transform → accumulate
 *
 * @param nums 输入数组
 * @return 偶数平方和
 */
int sum_of_even_squares(const std::vector<int>& nums)
{
    // TODO: 实现这个函数

    return 0;  // 替换这行
}

void test_exercise_4() {
    std::cout << "=== 练习 4: chain_operations ===\n";

    std::vector<int> nums = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    // 偶数: 2, 4, 6, 8, 10
    // 平方: 4, 16, 36, 64, 100
    // 和: 220
    int result = sum_of_even_squares(nums);

    std::cout << "偶数平方和: " << result << "\n";

    assert(result == 220);

    std::cout << "通过!\n\n";
}

// ============================================
// 练习 5: counter_factory
// ============================================

/**
 * 创建一个计数器闭包，每次调用返回递增的值
 *
 * 提示：使用 mutable 关键字允许修改值捕获的变量
 *       或者使用 shared_ptr
 *
 * @param start 起始值
 * @return 返回一个函数，每次调用返回下一个值
 */
std::function<int()> make_counter(int start)
{
    // TODO: 实现这个函数
    // 返回一个闭包，每次调用返回递增的值

    // 方法1：使用 mutable
    // 方法2：使用 shared_ptr<int>

    return []() { return 0; };  // 替换这行
}

void test_exercise_5() {
    std::cout << "=== 练习 5: counter_factory ===\n";

    auto counter = make_counter(10);

    std::cout << "counter() = " << counter() << "\n";  // 10
    std::cout << "counter() = " << counter() << "\n";  // 11
    std::cout << "counter() = " << counter() << "\n";  // 12

    auto another = make_counter(100);
    std::cout << "another() = " << another() << "\n";  // 100

    // 验证
    assert(counter() == 13);
    assert(another() == 101);

    std::cout << "通过!\n\n";
}

// ============================================
// main
// ============================================

int main() {
    std::cout << "========================================\n";
    std::cout << "    第01章练习：Lambda 和闭包\n";
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
