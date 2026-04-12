/**
 * @file 01_higher_order_functions.cpp
 * @brief 高阶函数示例
 *
 * 本文件演示：
 * - 函数作为参数和返回值
 * - map、filter、reduce
 * - 柯里化和偏应用
 * - 函数组合
 * - 实用高阶函数
 */

#include <algorithm>
#include <cmath>
#include <functional>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>

// ============================================
// 1. 函数作为参数和返回值
// ============================================

// 接受函数作为参数
template<typename F>
int apply_twice(int x, F f) {
    return f(f(x));
}

// 返回函数
[[nodiscard]] auto make_multiplier(int factor) {
    return [factor](int x) { return x * factor; };
}

[[nodiscard]] auto make_range_validator(int min, int max) {
    return [min, max](int x) {
        return x >= min && x <= max;
    };
}

void first_class_functions_demo() {
    std::cout << "=== 函数作为一等公民 ===\n";

    // 函数作为参数
    auto square = [](int x) { return x * x; };
    std::cout << "apply_twice(3, square) = " << apply_twice(3, square) << "\n";
    std::cout << "apply_twice(3, +1) = " << apply_twice(3, [](int x) { return x + 1; }) << "\n";

    // 函数作为返回值
    auto times5 = make_multiplier(5);
    auto times10 = make_multiplier(10);
    std::cout << "times5(7) = " << times5(7) << "\n";
    std::cout << "times10(7) = " << times10(7) << "\n";

    // 验证器工厂
    auto is_valid_age = make_range_validator(0, 150);
    auto is_valid_score = make_range_validator(0, 100);
    std::cout << "is_valid_age(25) = " << is_valid_age(25) << "\n";
    std::cout << "is_valid_age(200) = " << is_valid_age(200) << "\n";
    std::cout << "is_valid_score(85) = " << is_valid_score(85) << "\n\n";
}

// ============================================
// 2. Map、Filter、Reduce
// ============================================

template<typename T, typename F>
[[nodiscard]] auto map(const std::vector<T>& vec, F f) {
    using R = std::invoke_result_t<F, const T&>;
    std::vector<R> result;
    result.reserve(vec.size());
    for (const auto& item : vec) {
        result.push_back(f(item));
    }
    return result;
}

template<typename T, typename Pred>
[[nodiscard]] std::vector<T> filter(const std::vector<T>& vec, Pred pred) {
    std::vector<T> result;
    for (const auto& item : vec) {
        if (pred(item)) {
            result.push_back(item);
        }
    }
    return result;
}

template<typename T, typename Acc, typename F>
[[nodiscard]] Acc reduce(const std::vector<T>& vec, Acc init, F f) {
    Acc result = init;
    for (const auto& item : vec) {
        result = f(result, item);
    }
    return result;
}

void map_filter_reduce_demo() {
    std::cout << "=== Map, Filter, Reduce ===\n";

    std::vector<int> nums = {1, 2, 3, 4, 5};

    // Map: 转换
    auto squares = map(nums, [](int x) { return x * x; });
    std::cout << "map(square): ";
    for (int x : squares) std::cout << x << " ";
    std::cout << "\n";

    // Map: 类型转换
    auto strings = map(nums, [](int x) { return std::to_string(x); });
    std::cout << "map(to_string): ";
    for (const auto& s : strings) std::cout << "\"" << s << "\" ";
    std::cout << "\n";

    // Filter: 过滤
    auto evens = filter(nums, [](int x) { return x % 2 == 0; });
    std::cout << "filter(偶数): ";
    for (int x : evens) std::cout << x << " ";
    std::cout << "\n";

    // Reduce: 归约
    auto sum = reduce(nums, 0, [](int acc, int x) { return acc + x; });
    auto product = reduce(nums, 1, [](int acc, int x) { return acc * x; });
    std::cout << "reduce(+) = " << sum << "\n";
    std::cout << "reduce(*) = " << product << "\n";

    // 链式调用
    auto chain_result = reduce(
        map(
            filter(nums, [](int x) { return x % 2 == 0; }),
            [](int x) { return x * x; }
        ),
        0,
        [](int acc, int x) { return acc + x; }
    );
    std::cout << "filter(偶数) -> map(²) -> sum = " << chain_result << "\n\n";
}

// ============================================
// 3. 柯里化
// ============================================

// 手动柯里化
[[nodiscard]] auto curry_add3(int a) {
    return [a](int b) {
        return [a, b](int c) {
            return a + b + c;
        };
    };
}

// 通用两参数柯里化
template<typename F>
[[nodiscard]] auto curry2(F f) {
    return [f](auto a) {
        return [f, a](auto b) {
            return f(a, b);
        };
    };
}

// 通用三参数柯里化
template<typename F>
[[nodiscard]] auto curry3(F f) {
    return [f](auto a) {
        return [f, a](auto b) {
            return [f, a, b](auto c) {
                return f(a, b, c);
            };
        };
    };
}

void currying_demo() {
    std::cout << "=== 柯里化 ===\n";

    // 手动柯里化
    auto step1 = curry_add3(1);
    auto step2 = step1(2);
    auto result = step2(3);
    std::cout << "curry_add3(1)(2)(3) = " << result << "\n";

    // 一步到位
    std::cout << "curry_add3(10)(20)(30) = " << curry_add3(10)(20)(30) << "\n";

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

    auto with_vat = calc_price(0.2);        // 20% 增值税
    auto with_sales_tax = calc_price(0.08); // 8% 销售税

    std::cout << "with_vat(100) = " << with_vat(100) << "\n";
    std::cout << "with_sales_tax(100) = " << with_sales_tax(100) << "\n\n";
}

// ============================================
// 4. 偏函数应用
// ============================================

template<typename F, typename Arg>
[[nodiscard]] auto partial(F f, Arg arg) {
    return [f, arg](auto... rest) {
        return f(arg, rest...);
    };
}

void partial_application_demo() {
    std::cout << "=== 偏函数应用 ===\n";

    // 使用 std::bind
    auto multiply = [](int a, int b) { return a * b; };
    auto double_it = std::bind(multiply, 2, std::placeholders::_1);
    std::cout << "double_it(5) = " << double_it(5) << "\n";

    // 使用 partial 模板
    auto power = [](int base, int exp) {
        return static_cast<int>(std::pow(base, exp));
    };

    auto powers_of_2 = partial(power, 2);
    std::cout << "powers_of_2(3) = " << powers_of_2(3) << " (2³)\n";
    std::cout << "powers_of_2(5) = " << powers_of_2(5) << " (2⁵)\n";

    // 反过来：固定指数
    auto cube = partial([](int exp, int base) {
        return static_cast<int>(std::pow(base, exp));
    }, 3);
    std::cout << "cube(2) = " << cube(2) << " (2³)\n";
    std::cout << "cube(4) = " << cube(4) << " (4³)\n\n";
}

// ============================================
// 5. 函数组合
// ============================================

template<typename F, typename G>
[[nodiscard]] auto compose(F f, G g) {
    return [f, g](auto x) { return f(g(x)); };
}

template<typename F, typename G>
[[nodiscard]] auto pipe(F f, G g) {
    return [f, g](auto x) { return g(f(x)); };
}

void composition_demo() {
    std::cout << "=== 函数组合 ===\n";

    auto add10 = [](int x) { return x + 10; };
    auto double_it = [](int x) { return x * 2; };
    auto square = [](int x) { return x * x; };

    // compose: f(g(x)) - 从右到左
    auto double_then_add = compose(add10, double_it);
    std::cout << "compose(add10, double)(5) = " << double_then_add(5)
              << " ((5*2)+10)\n";

    // pipe: g(f(x)) - 从左到右
    auto add_then_double = pipe(add10, double_it);
    std::cout << "pipe(add10, double)(5) = " << add_then_double(5)
              << " ((5+10)*2)\n";

    // 多层组合
    auto complex = compose(add10, compose(double_it, square));
    std::cout << "compose(add10, compose(double, square))(3) = " << complex(3)
              << " (((3²)*2)+10)\n\n";
}

// ============================================
// 6. 实用高阶函数
// ============================================

template<typename T, typename Pred>
[[nodiscard]] bool any(const std::vector<T>& vec, Pred pred) {
    return std::any_of(vec.begin(), vec.end(), pred);
}

template<typename T, typename Pred>
[[nodiscard]] bool all(const std::vector<T>& vec, Pred pred) {
    return std::all_of(vec.begin(), vec.end(), pred);
}

template<typename T, typename Pred>
[[nodiscard]] bool none(const std::vector<T>& vec, Pred pred) {
    return std::none_of(vec.begin(), vec.end(), pred);
}

template<typename T, typename Pred>
[[nodiscard]] std::vector<T> take_while(const std::vector<T>& vec, Pred pred) {
    std::vector<T> result;
    for (const auto& item : vec) {
        if (!pred(item)) break;
        result.push_back(item);
    }
    return result;
}

template<typename T, typename Pred>
[[nodiscard]] std::vector<T> drop_while(const std::vector<T>& vec, Pred pred) {
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
    std::cout << "=== 实用高阶函数 ===\n";

    std::vector<int> nums = {1, 2, 3, 4, 5, 6};

    std::cout << "any(> 5) = " << any(nums, [](int x) { return x > 5; }) << "\n";
    std::cout << "all(> 0) = " << all(nums, [](int x) { return x > 0; }) << "\n";
    std::cout << "none(< 0) = " << none(nums, [](int x) { return x < 0; }) << "\n";

    auto taken = take_while(nums, [](int x) { return x < 4; });
    std::cout << "take_while(< 4): ";
    for (int x : taken) std::cout << x << " ";
    std::cout << "\n";

    auto dropped = drop_while(nums, [](int x) { return x < 4; });
    std::cout << "drop_while(< 4): ";
    for (int x : dropped) std::cout << x << " ";
    std::cout << "\n\n";
}

// ============================================
// 7. 函数存储与执行
// ============================================

void function_storage_demo() {
    std::cout << "=== 函数存储与执行 ===\n";

    std::vector<std::function<int(int)>> pipeline;

    pipeline.push_back([](int x) { return x * 2; });
    pipeline.push_back([](int x) { return x + 10; });
    pipeline.push_back([](int x) { return x * x; });

    int value = 5;
    std::cout << "初始值: " << value << "\n";

    for (size_t i = 0; i < pipeline.size(); ++i) {
        value = pipeline[i](value);
        std::cout << "操作 " << i + 1 << " 后: " << value << "\n";
    }

    std::cout << "最终结果: " << value << " (((5*2)+10)² = 400)\n\n";
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
    std::cout << "=== 数据处理管道 ===\n";

    std::vector<Person> people = {
        {"Alice", 30, 75000},
        {"Bob", 25, 60000},
        {"Charlie", 35, 90000},
        {"Diana", 28, 70000},
        {"Eve", 32, 85000}
    };

    // 管道1: 过滤年龄 > 28 -> 提取薪水 -> 求和
    auto filtered = filter(people, [](const Person& p) { return p.age > 28; });
    auto salaries = map(filtered, [](const Person& p) { return p.salary; });
    auto total = reduce(salaries, 0.0, [](double acc, double s) { return acc + s; });

    std::cout << "年龄 > 28 的总薪水: " << total << "\n";

    // 管道2: 薪水加 10% -> 过滤 > 80000
    auto raised = map(people, [](const Person& p) {
        return Person{p.name, p.age, p.salary * 1.1};
    });

    auto high_earners = filter(raised, [](const Person& p) {
        return p.salary > 80000;
    });

    std::cout << "加薪 10% 后薪水 > 80000 的人:\n";
    for (const auto& p : high_earners) {
        std::cout << "  " << p.name << ": " << p.salary << "\n";
    }

    // 管道3: 统计平均年龄
    auto ages = map(people, [](const Person& p) { return p.age; });
    auto sum_ages = reduce(ages, 0, [](int acc, int a) { return acc + a; });
    double avg_age = static_cast<double>(sum_ages) / people.size();
    std::cout << "平均年龄: " << avg_age << "\n\n";
}

// ============================================
// main
// ============================================

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
