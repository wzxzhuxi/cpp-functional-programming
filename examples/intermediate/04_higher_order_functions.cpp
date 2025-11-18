#include <algorithm>
#include <functional>
#include <iostream>
#include <numeric>
#include <vector>
#include <string>
#include <cmath>

// ============================================
// 1. 函数作为参数和返回值
// ============================================

// 接受函数作为参数
int apply_twice(int x, int (*f)(int)) {
    return f(f(x));
}

int square_func(int x) { return x * x; }

// 返回函数
auto make_multiplier(int factor) {
    return [factor](int x) { return x * factor; };
}

auto make_validator(int min, int max) {
    return [min, max](int x) {
        return x >= min && x <= max;
    };
}

void first_class_functions_demo() {
    std::cout << "=== First-Class Functions ===\n";

    // 函数作为参数
    std::cout << "apply_twice(3, square) = " << apply_twice(3, square_func) << "\n";
    std::cout << "apply_twice(3, +1) = " << apply_twice(3, [](int x) { return x + 1; }) << "\n";

    // 函数作为返回值
    auto times5 = make_multiplier(5);
    std::cout << "times5(10) = " << times5(10) << "\n";

    auto is_valid_age = make_validator(0, 150);
    std::cout << "is_valid_age(25) = " << is_valid_age(25) << "\n";
    std::cout << "is_valid_age(200) = " << is_valid_age(200) << "\n\n";
}

// ============================================
// 2. Map、Filter、Reduce
// ============================================

template<typename T, typename Func>
auto map(const std::vector<T>& vec, Func f) {
    using R = decltype(f(std::declval<T>()));
    std::vector<R> result;
    result.reserve(vec.size());
    for (const auto& item : vec) {
        result.push_back(f(item));
    }
    return result;
}

template<typename T, typename Pred>
std::vector<T> filter(const std::vector<T>& vec, Pred pred) {
    std::vector<T> result;
    for (const auto& item : vec) {
        if (pred(item)) {
            result.push_back(item);
        }
    }
    return result;
}

template<typename T, typename Acc, typename Func>
Acc reduce(const std::vector<T>& vec, Acc init, Func f) {
    Acc result = init;
    for (const auto& item : vec) {
        result = f(result, item);
    }
    return result;
}

void map_filter_reduce_demo() {
    std::cout << "=== Map, Filter, Reduce ===\n";

    std::vector<int> nums = {1, 2, 3, 4, 5};

    // Map
    auto squares = map(nums, [](int x) { return x * x; });
    std::cout << "Squares: ";
    for (int x : squares) std::cout << x << " ";
    std::cout << "\n";

    // Filter
    auto evens = filter(nums, [](int x) { return x % 2 == 0; });
    std::cout << "Evens: ";
    for (int x : evens) std::cout << x << " ";
    std::cout << "\n";

    // Reduce
    auto sum = reduce(nums, 0, [](int acc, int x) { return acc + x; });
    auto product = reduce(nums, 1, [](int acc, int x) { return acc * x; });
    std::cout << "Sum: " << sum << "\n";
    std::cout << "Product: " << product << "\n";

    // 链式调用
    auto result = reduce(
        filter(
            map(nums, [](int x) { return x * 2; }),
            [](int x) { return x > 5; }
        ),
        0,
        [](int acc, int x) { return acc + x; }
    );
    std::cout << "Chain result: " << result << " (double, filter >5, sum)\n\n";
}

// ============================================
// 3. 柯里化（Currying）
// ============================================

// 手动柯里化
auto curry_add(int a) {
    return [a](int b) {
        return [a, b](int c) {
            return a + b + c;
        };
    };
}

// 通用两参数柯里化
template<typename F>
auto curry2(F f) {
    return [f](auto a) {
        return [f, a](auto b) {
            return f(a, b);
        };
    };
}

// 通用三参数柯里化
template<typename F>
auto curry3(F f) {
    return [f](auto a) {
        return [f, a](auto b) {
            return [f, a, b](auto c) {
                return f(a, b, c);
            };
        };
    };
}

void currying_demo() {
    std::cout << "=== Currying ===\n";

    // 手动柯里化
    auto step1 = curry_add(1);
    auto step2 = step1(2);
    auto result = step2(3);
    std::cout << "curry_add(1)(2)(3) = " << result << "\n";

    // 通用柯里化
    auto multiply = [](int a, int b) { return a * b; };
    auto curried_mult = curry2(multiply);

    auto times10 = curried_mult(10);
    std::cout << "times10(5) = " << times10(5) << "\n";
    std::cout << "times10(7) = " << times10(7) << "\n";

    // 实用案例：价格计算
    auto calc_price = curry2([](double rate, double price) {
        return price * (1 + rate);
    });

    auto with_vat = calc_price(0.2);
    auto with_sales_tax = calc_price(0.1);

    std::cout << "with_vat(100) = " << with_vat(100) << "\n";
    std::cout << "with_sales_tax(100) = " << with_sales_tax(100) << "\n\n";
}

// ============================================
// 4. 偏函数应用（Partial Application）
// ============================================

template<typename Func, typename Arg>
auto partial(Func f, Arg arg) {
    return [f, arg](auto... rest) {
        return f(arg, rest...);
    };
}

void partial_application_demo() {
    std::cout << "=== Partial Application ===\n";

    // 使用 std::bind
    auto multiply = [](int a, int b) { return a * b; };
    auto double_it = std::bind(multiply, 2, std::placeholders::_1);
    std::cout << "double_it(5) = " << double_it(5) << "\n";

    // 手动 partial
    auto power = [](int base, int exp) {
        int result = 1;
        for (int i = 0; i < exp; ++i) result *= base;
        return result;
    };

    auto powers_of_2 = partial(power, 2);
    std::cout << "powers_of_2(3) = " << powers_of_2(3) << " (2^3)\n";
    std::cout << "powers_of_2(5) = " << powers_of_2(5) << " (2^5)\n";

    auto cube_of = partial([](int exp, int base) {
        return static_cast<int>(std::pow(base, exp));
    }, 3);
    std::cout << "cube_of(2) = " << cube_of(2) << " (2^3)\n";
    std::cout << "cube_of(4) = " << cube_of(4) << " (4^3)\n\n";
}

// ============================================
// 5. 函数组合
// ============================================

template<typename F, typename G>
auto compose(F f, G g) {
    return [f, g](auto x) { return f(g(x)); };
}

template<typename F, typename G>
auto pipe(F f, G g) {
    return [f, g](auto x) { return g(f(x)); };
}

void composition_demo() {
    std::cout << "=== Function Composition ===\n";

    auto add10 = [](int x) { return x + 10; };
    auto double_it = [](int x) { return x * 2; };
    auto square = [](int x) { return x * x; };

    // compose: f(g(x))
    auto double_then_add10 = compose(add10, double_it);
    std::cout << "double_then_add10(5) = " << double_then_add10(5)
              << " ((5*2)+10)\n";

    // pipe: g(f(x))
    auto add10_then_double = pipe(add10, double_it);
    std::cout << "add10_then_double(5) = " << add10_then_double(5)
              << " ((5+10)*2)\n";

    // 多层组合
    auto complex = compose(add10, compose(double_it, square));
    std::cout << "complex(3) = " << complex(3)
              << " (((3^2)*2)+10)\n\n";
}

// ============================================
// 6. 实用高阶函数
// ============================================

template<typename T, typename Pred>
bool any(const std::vector<T>& vec, Pred pred) {
    for (const auto& item : vec) {
        if (pred(item)) return true;
    }
    return false;
}

template<typename T, typename Pred>
bool all(const std::vector<T>& vec, Pred pred) {
    for (const auto& item : vec) {
        if (!pred(item)) return false;
    }
    return true;
}

template<typename T, typename Pred>
std::vector<T> take_while(const std::vector<T>& vec, Pred pred) {
    std::vector<T> result;
    for (const auto& item : vec) {
        if (!pred(item)) break;
        result.push_back(item);
    }
    return result;
}

template<typename T, typename Pred>
std::vector<T> drop_while(const std::vector<T>& vec, Pred pred) {
    std::vector<T> result;
    bool dropping = true;
    for (const auto& item : vec) {
        if (dropping && pred(item)) continue;
        dropping = false;
        result.push_back(item);
    }
    return result;
}

void utility_functions_demo() {
    std::cout << "=== Utility Higher-Order Functions ===\n";

    std::vector<int> nums = {1, 2, 3, 4, 5, 6};

    std::cout << "any > 5: " << any(nums, [](int x) { return x > 5; }) << "\n";
    std::cout << "all > 0: " << all(nums, [](int x) { return x > 0; }) << "\n";

    auto taken = take_while(nums, [](int x) { return x < 4; });
    std::cout << "take_while < 4: ";
    for (int x : taken) std::cout << x << " ";
    std::cout << "\n";

    auto dropped = drop_while(nums, [](int x) { return x < 4; });
    std::cout << "drop_while < 4: ";
    for (int x : dropped) std::cout << x << " ";
    std::cout << "\n\n";
}

// ============================================
// 7. 存储和执行函数
// ============================================

void function_storage_demo() {
    std::cout << "=== Function Storage ===\n";

    std::vector<std::function<int(int)>> operations;

    operations.push_back([](int x) { return x * 2; });
    operations.push_back([](int x) { return x + 10; });
    operations.push_back([](int x) { return x * x; });

    int value = 5;
    std::cout << "Starting value: " << value << "\n";

    for (const auto& op : operations) {
        value = op(value);
        std::cout << "After operation: " << value << "\n";
    }

    std::cout << "Final: " << value << " (((5*2)+10)^2 = 400)\n\n";
}

// ============================================
// 8. 实际应用：数据处理管道
// ============================================

struct Person {
    std::string name;
    int age;
    double salary;
};

void pipeline_demo() {
    std::cout << "=== Real-World Pipeline ===\n";

    std::vector<Person> people = {
        {"Alice", 30, 75000},
        {"Bob", 25, 60000},
        {"Charlie", 35, 90000},
        {"Diana", 28, 70000},
        {"Eve", 32, 85000}
    };

    // 管道：过滤年龄 > 28 -> 提取薪水 -> 求和
    auto salaries = map(
        filter(people, [](const Person& p) { return p.age > 28; }),
        [](const Person& p) { return p.salary; }
    );

    auto total = reduce(salaries, 0.0, [](double acc, double s) { return acc + s; });

    std::cout << "Total salary (age > 28): " << total << "\n";

    // 另一个管道：薪水加 10% -> 过滤 > 80000
    auto raised_salaries = map(people, [](const Person& p) {
        return Person{p.name, p.age, p.salary * 1.1};
    });

    auto high_earners = filter(raised_salaries, [](const Person& p) {
        return p.salary > 80000;
    });

    std::cout << "High earners after 10% raise:\n";
    for (const auto& p : high_earners) {
        std::cout << "  " << p.name << ": " << p.salary << "\n";
    }

    std::cout << "\n";
}

int main() {
    first_class_functions_demo();
    map_filter_reduce_demo();
    currying_demo();
    partial_application_demo();
    composition_demo();
    utility_functions_demo();
    function_storage_demo();
    pipeline_demo();

    return 0;
}
