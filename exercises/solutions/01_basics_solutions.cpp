#include <algorithm>
#include <iostream>
#include <memory>
#include <numeric>
#include <string>
#include <vector>

// 练习 1: 过滤长度大于 5 的字符串
std::vector<std::string> filter_long_strings(const std::vector<std::string>& strings) {
    std::vector<std::string> result;
    std::copy_if(strings.begin(), strings.end(),
                 std::back_inserter(result),
                 [](const std::string& s) { return s.length() > 5; });
    return result;
}

// 练习 2: 实现 make_multiplier
auto make_multiplier(int n) {
    return [n](int x) { return x * n; };
}

// 练习 3: 按字段排序结构体
struct Person {
    std::string name;
    int age;
    double salary;
};

void sort_by_age(std::vector<Person>& people) {
    std::sort(people.begin(), people.end(),
              [](const Person& a, const Person& b) { return a.age < b.age; });
}

void sort_by_salary(std::vector<Person>& people) {
    std::sort(people.begin(), people.end(),
              [](const Person& a, const Person& b) { return a.salary > b.salary; });
}

// 练习 4: 链式变换
int sum_of_even_squares(const std::vector<int>& nums) {
    // 方法 1: 使用 std::accumulate 和 lambda
    return std::accumulate(nums.begin(), nums.end(), 0,
                           [](int acc, int x) {
                               return x % 2 == 0 ? acc + x * x : acc;
                           });

    // 方法 2: 先 filter 再 transform 再 sum
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

// 练习 5: 闭包计数器
auto make_counter() {
    // 使用 shared_ptr 在多个 lambda 之间共享可变状态
    auto count = std::make_shared<int>(0);
    return [count]() mutable { return (*count)++; };

    // 或者使用 mutable lambda
    // int count = 0;
    // return [count]() mutable { return count++; };
}

// ============================================
// 测试代码
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
    std::cout << "C++ Functional Programming - Chapter 01 Solutions\n";
    std::cout << "=================================================\n\n";

    test_exercise_1();
    test_exercise_2();
    test_exercise_3();
    test_exercise_4();
    test_exercise_5();

    return 0;
}
