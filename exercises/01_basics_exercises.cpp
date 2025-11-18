#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

// 练习 1: 过滤长度大于 5 的字符串
// TODO: 实现 filter_long_strings
std::vector<std::string> filter_long_strings(const std::vector<std::string>& strings) {
    // 你的代码在这里
    return {};
}

// 练习 2: 实现 make_multiplier
// TODO: 返回一个 lambda，它将输入乘以 n
auto make_multiplier(int n) {
    // 你的代码在这里
    return [](int x) { return x; };
}

// 练习 3: 按字段排序结构体
struct Person {
    std::string name;
    int age;
    double salary;
};

// TODO: 实现 sort_by_age，按年龄升序排序
void sort_by_age(std::vector<Person>& people) {
    // 你的代码在这里
}

// TODO: 实现 sort_by_salary，按薪水降序排序
void sort_by_salary(std::vector<Person>& people) {
    // 你的代码在这里
}

// 练习 4: 链式变换
// TODO: 实现一个函数，接收整数数组，返回所有偶数的平方之和
int sum_of_even_squares(const std::vector<int>& nums) {
    // 提示：过滤 -> 转换 -> 求和
    return 0;
}

// 练习 5: 闭包计数器
// TODO: 实现一个计数器生成器，每次调用返回递增的值
auto make_counter() {
    // 你的代码在这里
    return []() { return 0; };
}

// ============================================
// 测试代码 - 不要修改
// ============================================

void test_exercise_1() {
    std::cout << "=== Exercise 1: Filter Long Strings ===\n";
    std::vector<std::string> words = {"hi", "hello", "world", "functional", "cpp", "programming"};
    auto result = filter_long_strings(words);

    std::cout << "Input: ";
    for (const auto& w : words) std::cout << w << " ";
    std::cout << "\nFiltered (len > 5): ";
    for (const auto& w : result) std::cout << w << " ";
    std::cout << "\n";
    std::cout << "Expected: functional programming\n\n";
}

void test_exercise_2() {
    std::cout << "=== Exercise 2: Make Multiplier ===\n";
    auto times3 = make_multiplier(3);
    auto times7 = make_multiplier(7);

    std::cout << "times3(5) = " << times3(5) << " (expected: 15)\n";
    std::cout << "times7(5) = " << times7(5) << " (expected: 35)\n\n";
}

void test_exercise_3() {
    std::cout << "=== Exercise 3: Sort Structs ===\n";
    std::vector<Person> people = {
        {"Alice", 30, 75000.0},
        {"Bob", 25, 60000.0},
        {"Charlie", 35, 90000.0},
        {"Diana", 28, 70000.0}
    };

    auto print_people = [](const std::vector<Person>& p) {
        for (const auto& person : p) {
            std::cout << person.name << " (age: " << person.age
                      << ", salary: " << person.salary << ")\n";
        }
    };

    std::cout << "Original:\n";
    print_people(people);

    auto people_by_age = people;
    sort_by_age(people_by_age);
    std::cout << "\nSorted by age:\n";
    print_people(people_by_age);

    auto people_by_salary = people;
    sort_by_salary(people_by_salary);
    std::cout << "\nSorted by salary (desc):\n";
    print_people(people_by_salary);
    std::cout << "\n";
}

void test_exercise_4() {
    std::cout << "=== Exercise 4: Sum of Even Squares ===\n";
    std::vector<int> nums = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    int result = sum_of_even_squares(nums);
    std::cout << "Input: ";
    for (int n : nums) std::cout << n << " ";
    std::cout << "\nSum of even squares: " << result << " (expected: 220)\n";
    std::cout << "Calculation: 2^2 + 4^2 + 6^2 + 8^2 + 10^2 = 4 + 16 + 36 + 64 + 100 = 220\n\n";
}

void test_exercise_5() {
    std::cout << "=== Exercise 5: Counter Closure ===\n";
    auto counter1 = make_counter();
    auto counter2 = make_counter();

    std::cout << "counter1: " << counter1() << " " << counter1() << " " << counter1() << "\n";
    std::cout << "Expected: 0 1 2\n";
    std::cout << "counter2: " << counter2() << " " << counter2() << "\n";
    std::cout << "Expected: 0 1\n";
    std::cout << "(Each counter maintains its own state)\n\n";
}

int main() {
    std::cout << "C++ Functional Programming - Chapter 01 Exercises\n";
    std::cout << "=================================================\n\n";

    test_exercise_1();
    test_exercise_2();
    test_exercise_3();
    test_exercise_4();
    test_exercise_5();

    std::cout << "Hint: Check solutions in exercises/solutions/01_basics_solutions.cpp\n";

    return 0;
}
