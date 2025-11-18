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
}

// ============================================
// 练习 2: 柯里化
// ============================================

namespace exercise2 {
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

    template<typename F>
    auto curry4(F f) {
        return [f](auto a) {
            return [f, a](auto b) {
                return [f, a, b](auto c) {
                    return [f, a, b, c](auto d) {
                        return f(a, b, c, d);
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
    template<typename F, typename G>
    auto compose(F f, G g) {
        return [f, g](auto x) { return f(g(x)); };
    }

    template<typename F>
    auto compose_many(F f) {
        return f;
    }

    template<typename F, typename... Rest>
    auto compose_many(F f, Rest... rest) {
        return [f, rest...](auto x) {
            return f(compose_many(rest...)(x));
        };
    }

    template<typename F, typename G>
    auto pipe(F f, G g) {
        return [f, g](auto x) { return g(f(x)); };
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

    std::vector<std::string> get_available_product_names(
        const std::vector<Product>& products
    ) {
        using namespace exercise1;

        auto available = filter(products, [](const Product& p) {
            return p.stock > 0;
        });

        return map(available, [](const Product& p) {
            return p.name;
        });
    }

    double calculate_category_value(
        const std::vector<Product>& products,
        const std::string& category
    ) {
        using namespace exercise1;

        auto in_category = filter(products, [&category](const Product& p) {
            return p.category == category;
        });

        auto values = map(in_category, [](const Product& p) {
            return p.price * p.stock;
        });

        return reduce(values, 0.0, [](double acc, double val) {
            return acc + val;
        });
    }

    std::vector<Product> apply_discount(
        const std::vector<Product>& products
    ) {
        using namespace exercise1;

        return map(products, [](const Product& p) {
            if (p.price > 100) {
                return Product{p.name, p.price * 0.9, p.stock, p.category};
            }
            return p;
        });
    }
}

// ============================================
// 练习 5: 偏函数应用
// ============================================

namespace exercise5 {
    template<typename Func, typename... Args>
    auto partial(Func f, Args... args) {
        return [f, args...](auto... rest) {
            return f(args..., rest...);
        };
    }
}

// ============================================
// 练习 6: 高阶函数组合
// ============================================

namespace exercise6 {
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

    template<typename T, typename Pred>
    std::pair<std::vector<T>, std::vector<T>> partition(
        const std::vector<T>& vec,
        Pred pred
    ) {
        std::vector<T> true_part, false_part;
        for (const auto& item : vec) {
            if (pred(item)) {
                true_part.push_back(item);
            } else {
                false_part.push_back(item);
            }
        }
        return {true_part, false_part};
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
    std::cout << "C++ Functional Programming - Chapter 04 Solutions\n";
    std::cout << "=================================================\n\n";

    test_exercise_1();
    test_exercise_2();
    test_exercise_3();
    test_exercise_4();
    test_exercise_5();
    test_exercise_6();

    return 0;
}
