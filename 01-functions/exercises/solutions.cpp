/**
 * @file solutions.cpp
 * @brief 第01章练习题参考答案
 */

#include <algorithm>
#include <cassert>
#include <functional>
#include <iostream>
#include <memory>
#include <numeric>
#include <string>
#include <vector>

// ============================================
// 练习 1: filter_long_strings
// ============================================

std::vector<std::string> filter_long_strings(
    const std::vector<std::string>& strings,
    size_t min_length)
{
    std::vector<std::string> result;

    std::copy_if(strings.begin(), strings.end(),
                 std::back_inserter(result),
                 [min_length](const std::string& s) {
                     return s.length() > min_length;
                 });

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

    assert(long_words.size() == 2);
    assert(long_words[0] == "functional");
    assert(long_words[1] == "programming");

    std::cout << "通过!\n\n";
}

// ============================================
// 练习 2: make_multiplier
// ============================================

auto make_multiplier(int n)
{
    // 值捕获 n，返回一个闭包
    return [n](int x) { return x * n; };
}

void test_exercise_2() {
    std::cout << "=== 练习 2: make_multiplier ===\n";

    auto times2 = make_multiplier(2);
    auto times5 = make_multiplier(5);
    auto times10 = make_multiplier(10);

    std::cout << "times2(7) = " << times2(7) << "\n";
    std::cout << "times5(7) = " << times5(7) << "\n";
    std::cout << "times10(7) = " << times10(7) << "\n";

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

void sort_by_age(std::vector<Person>& people)
{
    std::sort(people.begin(), people.end(),
              [](const Person& a, const Person& b) {
                  return a.age < b.age;
              });
}

void sort_by_name_length_desc(std::vector<Person>& people)
{
    std::sort(people.begin(), people.end(),
              [](const Person& a, const Person& b) {
                  return a.name.length() > b.name.length();
              });
}

void test_exercise_3() {
    std::cout << "=== 练习 3: sort_by_field ===\n";

    std::vector<Person> people = {
        {"Alice", 30},
        {"Bob", 25},
        {"Charlie", 35},
        {"David", 28}
    };

    sort_by_age(people);

    std::cout << "按年龄排序: ";
    for (const auto& p : people) {
        std::cout << p.name << "(" << p.age << ") ";
    }
    std::cout << "\n";

    assert(people[0].name == "Bob");
    assert(people[3].name == "Charlie");

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

int sum_of_even_squares(const std::vector<int>& nums)
{
    // 方法1：一次遍历，用 accumulate
    return std::accumulate(nums.begin(), nums.end(), 0,
        [](int acc, int x) {
            return x % 2 == 0 ? acc + x * x : acc;
        });

    // 方法2：分步（可读性更好但效率略低）
    // std::vector<int> evens;
    // std::copy_if(nums.begin(), nums.end(), std::back_inserter(evens),
    //              [](int x) { return x % 2 == 0; });
    //
    // std::vector<int> squares;
    // std::transform(evens.begin(), evens.end(), std::back_inserter(squares),
    //                [](int x) { return x * x; });
    //
    // return std::accumulate(squares.begin(), squares.end(), 0);
}

void test_exercise_4() {
    std::cout << "=== 练习 4: chain_operations ===\n";

    std::vector<int> nums = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    int result = sum_of_even_squares(nums);

    std::cout << "偶数平方和: " << result << "\n";

    assert(result == 220);

    std::cout << "通过!\n\n";
}

// ============================================
// 练习 5: counter_factory
// ============================================

// 方法1：使用 mutable
std::function<int()> make_counter_mutable(int start)
{
    // mutable 允许修改值捕获的变量
    return [count = start]() mutable {
        return count++;
    };
}

// 方法2：使用 shared_ptr（多个副本共享状态）
std::function<int()> make_counter_shared(int start)
{
    auto count = std::make_shared<int>(start);
    return [count]() {
        return (*count)++;
    };
}

// 默认使用 mutable 版本
std::function<int()> make_counter(int start)
{
    return make_counter_mutable(start);
}

void test_exercise_5() {
    std::cout << "=== 练习 5: counter_factory ===\n";

    auto counter = make_counter(10);

    std::cout << "counter() = " << counter() << "\n";  // 10
    std::cout << "counter() = " << counter() << "\n";  // 11
    std::cout << "counter() = " << counter() << "\n";  // 12

    auto another = make_counter(100);
    std::cout << "another() = " << another() << "\n";  // 100

    assert(counter() == 13);
    assert(another() == 101);

    std::cout << "通过!\n\n";

    // 演示 shared_ptr 版本的区别
    std::cout << "--- shared_ptr 版本对比 ---\n";
    auto counter1 = make_counter_shared(0);
    auto counter2 = counter1;  // 复制

    std::cout << "counter1() = " << counter1() << "\n";  // 0
    std::cout << "counter2() = " << counter2() << "\n";  // 1 (共享状态!)
    std::cout << "counter1() = " << counter1() << "\n";  // 2

    auto counter3 = make_counter_mutable(0);
    auto counter4 = counter3;  // 复制

    std::cout << "counter3() = " << counter3() << "\n";  // 0
    std::cout << "counter4() = " << counter4() << "\n";  // 0 (各自独立!)
    std::cout << "counter3() = " << counter3() << "\n";  // 1
    std::cout << "\n";
}

// ============================================
// main
// ============================================

int main() {
    std::cout << "========================================\n";
    std::cout << "    第01章练习参考答案\n";
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
