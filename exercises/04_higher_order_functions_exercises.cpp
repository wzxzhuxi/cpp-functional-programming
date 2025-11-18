#include <algorithm>
#include <functional>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>

// ============================================
// 练习 1: 实现 Map、Filter、Reduce
// ============================================

namespace exercise1 {
    // TODO: 实现通用 map
    template<typename T, typename Func>
    auto map(const std::vector<T>& vec, Func f) {
        // 你的代码
        using R = decltype(f(std::declval<T>()));
        std::vector<R> result;
        return result;
    }

    // TODO: 实现 filter
    template<typename T, typename Pred>
    std::vector<T> filter(const std::vector<T>& vec, Pred pred) {
        // 你的代码
        return {};
    }

    // TODO: 实现 reduce
    template<typename T, typename Acc, typename Func>
    Acc reduce(const std::vector<T>& vec, Acc init, Func f) {
        // 你的代码
        return init;
    }
}

// ============================================
// 练习 2: 柯里化
// ============================================

namespace exercise2 {
    // TODO: 实现三参数柯里化
    template<typename F>
    auto curry3(F f) {
        // 返回嵌套的 lambda
        // curry3(f)(a)(b)(c) 应该等价于 f(a, b, c)
        // 提示：每层 lambda 捕获前面的参数，返回下一层 lambda
        return [f](auto a) {
            return [f, a](auto b) {
                return [f, a, b](auto c) {
                    // 你的代码：调用 f(a, b, c)
                    return decltype(f(a, b, c))();
                };
            };
        };
    }

    // TODO: 实现四参数柯里化（挑战）
    template<typename F>
    auto curry4(F f) {
        // 你的代码：类似 curry3，但多一层嵌套
        return [f](auto a) {
            return [f, a](auto b) {
                return [f, a, b](auto c) {
                    return [f, a, b, c](auto d) {
                        // 你的代码：调用 f(a, b, c, d)
                        return decltype(f(a, b, c, d))();
                    };
                };
            };
        };
    }
}

// ============================================
// 练习 3: 函数组合
// ============================================

namespace exercise3 {
    // TODO: 实现 compose (f ∘ g)
    template<typename F, typename G>
    auto compose(F f, G g) {
        // 你的代码：compose(f, g)(x) = f(g(x))
        return [f, g](auto x) {
            // 你的代码
            (void)f; (void)g;  // 避免未使用警告
            return x;
        };
    }

    // TODO: 实现支持多个函数的 compose
    // compose(f, g, h)(x) = f(g(h(x)))
    template<typename F>
    auto compose_many(F f) {
        return f;
    }

    template<typename F, typename... Rest>
    auto compose_many(F f, Rest... rest) {
        // 提示：递归组合
        return [f, rest...](auto x) {
            // 你的代码：先递归组合 rest，再应用 f
            (void)f;
            return x;
        };
    }

    // TODO: 实现 pipe (反向 compose)
    template<typename F, typename G>
    auto pipe(F f, G g) {
        // pipe(f, g)(x) = g(f(x))
        return [f, g](auto x) {
            // 你的代码
            (void)f; (void)g;
            return x;
        };
    }
}

// ============================================
// 练习 4: 数据处理管道
// ============================================

namespace exercise4 {
    struct Product {
        std::string name;
        double price;
        int stock;
        std::string category;
    };

    // TODO: 使用 map/filter/reduce 实现以下功能

    // 1. 获取所有库存 > 0 的产品名称
    std::vector<std::string> get_available_product_names(
        const std::vector<Product>& products
    ) {
        // 你的代码：filter + map
        return {};
    }

    // 2. 计算特定类别的产品总价值（价格 * 库存）
    double calculate_category_value(
        const std::vector<Product>& products,
        const std::string& category
    ) {
        // 你的代码：filter + map + reduce
        return 0.0;
    }

    // 3. 应用折扣：对所有价格 > 100 的产品打 9 折
    std::vector<Product> apply_discount(
        const std::vector<Product>& products
    ) {
        // 你的代码：map
        return {};
    }
}

// ============================================
// 练习 5: 偏函数应用
// ============================================

namespace exercise5 {
    // TODO: 实现通用 partial 应用
    template<typename Func, typename... Args>
    auto partial(Func f, Args... args) {
        // 固定前几个参数，返回接受剩余参数的函数
        return [f, args...](auto... rest) {
            // 你的代码：调用 f(args..., rest...)
            (void)f;
            return 0;
        };
    }

    // TODO: 使用 partial 创建以下函数
    // 给定函数: power(base, exp)
    // 创建: square = partial(power, ?, 2)
    // 创建: cube = partial(power, ?, 3)
}

// ============================================
// 练习 6: 高阶函数组合
// ============================================

namespace exercise6 {
    // TODO: 实现 take_while
    template<typename T, typename Pred>
    std::vector<T> take_while(const std::vector<T>& vec, Pred pred) {
        // 从头开始取，直到条件不满足
        return {};
    }

    // TODO: 实现 drop_while
    template<typename T, typename Pred>
    std::vector<T> drop_while(const std::vector<T>& vec, Pred pred) {
        // 从头开始丢弃，直到条件不满足
        return {};
    }

    // TODO: 实现 partition
    template<typename T, typename Pred>
    std::pair<std::vector<T>, std::vector<T>> partition(
        const std::vector<T>& vec,
        Pred pred
    ) {
        // 分成两组：满足条件的和不满足的
        return {{}, {}};
    }
}

// ============================================
// 测试代码
// ============================================

void test_exercise_1() {
    std::cout << "=== Exercise 1: Map/Filter/Reduce ===\n";

    using namespace exercise1;

    std::vector<int> nums = {1, 2, 3, 4, 5};

    auto squares = map(nums, [](int x) { return x * x; });
    std::cout << "Squares: ";
    for (int x : squares) std::cout << x << " ";
    std::cout << " (expected: 1 4 9 16 25)\n";

    auto evens = filter(nums, [](int x) { return x % 2 == 0; });
    std::cout << "Evens: ";
    for (int x : evens) std::cout << x << " ";
    std::cout << " (expected: 2 4)\n";

    auto sum = reduce(nums, 0, [](int acc, int x) { return acc + x; });
    std::cout << "Sum: " << sum << " (expected: 15)\n\n";
}

void test_exercise_2() {
    std::cout << "=== Exercise 2: Currying ===\n";

    using namespace exercise2;

    auto add3 = [](int a, int b, int c) { return a + b + c; };
    auto curried = curry3(add3);

    std::cout << "curry3(add)(1)(2)(3) = " << curried(1)(2)(3)
              << " (expected: 6)\n";

    auto add1 = curried(1);
    auto add1_2 = add1(2);
    std::cout << "Partial application: " << add1_2(10)
              << " (expected: 13)\n\n";
}

void test_exercise_3() {
    std::cout << "=== Exercise 3: Function Composition ===\n";

    using namespace exercise3;

    auto add10 = [](int x) { return x + 10; };
    auto double_it = [](int x) { return x * 2; };
    auto square = [](int x) { return x * x; };

    auto f = compose(add10, double_it);
    std::cout << "compose(+10, *2)(5) = " << f(5)
              << " (expected: 20)\n";

    auto g = compose_many(add10, double_it, square);
    std::cout << "compose_many(+10, *2, ^2)(3) = " << g(3)
              << " (expected: 28)\n";

    auto h = pipe(square, double_it);
    std::cout << "pipe(^2, *2)(3) = " << h(3)
              << " (expected: 18)\n\n";
}

void test_exercise_4() {
    std::cout << "=== Exercise 4: Data Pipeline ===\n";

    using namespace exercise4;

    std::vector<Product> products = {
        {"Laptop", 999.99, 5, "Electronics"},
        {"Mouse", 29.99, 0, "Electronics"},
        {"Keyboard", 79.99, 10, "Electronics"},
        {"Desk", 299.99, 3, "Furniture"},
        {"Chair", 199.99, 0, "Furniture"}
    };

    auto available = get_available_product_names(products);
    std::cout << "Available products: ";
    for (const auto& name : available) std::cout << name << " ";
    std::cout << "\n";

    auto electronics_value = calculate_category_value(products, "Electronics");
    std::cout << "Electronics total value: " << electronics_value << "\n";

    auto discounted = apply_discount(products);
    std::cout << "After discount:\n";
    for (const auto& p : discounted) {
        std::cout << "  " << p.name << ": " << p.price << "\n";
    }
    std::cout << "\n";
}

void test_exercise_5() {
    std::cout << "=== Exercise 5: Partial Application ===\n";

    using namespace exercise5;

    auto multiply = [](int a, int b, int c) { return a * b * c; };
    auto times2 = partial(multiply, 2);

    std::cout << "partial(multiply, 2)(3, 4) = " << times2(3, 4)
              << " (expected: 24)\n\n";
}

void test_exercise_6() {
    std::cout << "=== Exercise 6: Advanced HOF ===\n";

    using namespace exercise6;

    std::vector<int> nums = {1, 2, 3, 4, 5, 6};

    auto taken = take_while(nums, [](int x) { return x < 4; });
    std::cout << "take_while < 4: ";
    for (int x : taken) std::cout << x << " ";
    std::cout << " (expected: 1 2 3)\n";

    auto dropped = drop_while(nums, [](int x) { return x < 4; });
    std::cout << "drop_while < 4: ";
    for (int x : dropped) std::cout << x << " ";
    std::cout << " (expected: 4 5 6)\n";

    auto [evens, odds] = partition(nums, [](int x) { return x % 2 == 0; });
    std::cout << "Partition evens: ";
    for (int x : evens) std::cout << x << " ";
    std::cout << "\nPartition odds: ";
    for (int x : odds) std::cout << x << " ";
    std::cout << "\n\n";
}

int main() {
    std::cout << "C++ Functional Programming - Chapter 04 Exercises\n";
    std::cout << "=================================================\n\n";

    test_exercise_1();
    test_exercise_2();
    test_exercise_3();
    test_exercise_4();
    test_exercise_5();
    test_exercise_6();

    std::cout << "Hint: Check solutions in exercises/solutions/04_higher_order_functions_solutions.cpp\n";

    return 0;
}
