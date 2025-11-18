#include <algorithm>
#include <cctype>
#include <cmath>
#include <functional>
#include <iostream>
#include <optional>
#include <string>
#include <vector>

// ============================================
// 练习 1: 实现基础组合函数
// ============================================

namespace exercise1 {
    // TODO: 实现 compose (f ∘ g)(x) = f(g(x))
    template<typename F, typename G>
    auto compose(F f, G g) {
        return [f, g](auto x) {
            // 你的代码：先应用 g，再应用 f
            (void)f; (void)g;
            return x;
        };
    }

    // TODO: 实现 pipe (从左到右)
    template<typename F, typename G>
    auto pipe(F f, G g) {
        return [f, g](auto x) {
            // 你的代码：先应用 f，再应用 g
            (void)f; (void)g;
            return x;
        };
    }

    // TODO: 实现 compose3 (三个函数的组合)
    template<typename F, typename G, typename H>
    auto compose3(F f, G g, H h) {
        return [f, g, h](auto x) {
            // 你的代码：h -> g -> f
            (void)f; (void)g; (void)h;
            return x;
        };
    }
}

// ============================================
// 练习 2: 文本处理管道
// ============================================

namespace exercise2 {
    using namespace exercise1;

    // TODO: 实现 trim (去除首尾空格)
    auto trim = [](std::string s) {
        // 你的代码
        return s;
    };

    // TODO: 实现 to_lower (转小写)
    auto to_lower = [](std::string s) {
        // 你的代码
        return s;
    };

    // TODO: 实现 replace_spaces (空格替换为下划线)
    auto replace_spaces = [](std::string s) {
        // 你的代码
        return s;
    };

    // TODO: 组合上述函数创建 slug 生成器
    // 例如："  Hello World  " -> "hello_world"
    auto to_slug = [](std::string s) {
        // 你的代码：使用 pipe 组合 trim, to_lower, replace_spaces
        (void)trim; (void)to_lower; (void)replace_spaces;
        return s;
    };
}

// ============================================
// 练习 3: 组合子实现
// ============================================

namespace exercise3 {
    // TODO: 实现 flip (翻转二元函数参数)
    template<typename F>
    auto flip(F f) {
        return [f](auto a, auto b) {
            // 你的代码：调用 f(b, a)
            (void)f;
            return a;
        };
    }

    // TODO: 实现 const_fn (常量函数，忽略输入)
    template<typename T>
    auto const_fn(T value) {
        return [value](auto...) {
            // 你的代码：返回 value
            return T();
        };
    }

    // TODO: 实现 apply_twice (应用函数两次)
    template<typename F>
    auto apply_twice(F f) {
        return [f](auto x) {
            // 你的代码：f(f(x))
            (void)f;
            return x;
        };
    }
}

// ============================================
// 练习 4: 数值计算管道
// ============================================

namespace exercise4 {
    using namespace exercise1;

    // TODO: 实现价格计算函数
    // apply_discount: 应用折扣
    auto apply_discount = [](double rate) {
        return [rate](double price) {
            // 你的代码：price * (1 - rate)
            (void)rate;
            return price;
        };
    };

    // apply_tax: 应用税率
    auto apply_tax = [](double rate) {
        return [rate](double price) {
            // 你的代码：price * (1 + rate)
            (void)rate;
            return price;
        };
    };

    // apply_shipping: 加运费
    auto apply_shipping = [](double cost) {
        return [cost](double price) {
            // 你的代码：price + cost
            (void)cost;
            return price;
        };
    };

    // TODO: 组合以上函数创建最终价格计算器
    // 顺序：10% 折扣 -> 8% 税 -> $5 运费
    auto calculate_final_price = [](double price) {
        // 你的代码：使用 pipe 组合
        (void)apply_discount; (void)apply_tax; (void)apply_shipping;
        return price;
    };
}

// ============================================
// 练习 5: 可变参数组合
// ============================================

namespace exercise5 {
    // TODO: 实现可变参数 pipe
    template<typename F>
    auto pipe_many(F f) {
        return f;
    }

    template<typename F, typename... Rest>
    auto pipe_many(F f, Rest... rest) {
        return [f, rest...](auto x) {
            // 你的代码：先应用 f，再递归应用 rest
            (void)f;
            return x;
        };
    }

    // TODO: 实现可变参数 compose
    template<typename F>
    auto compose_many(F f) {
        return f;
    }

    template<typename F, typename... Rest>
    auto compose_many(F f, Rest... rest) {
        return [f, rest...](auto x) {
            // 你的代码：先递归应用 rest，再应用 f
            (void)f;
            return x;
        };
    }
}

// ============================================
// 练习 6: 函数式数据验证
// ============================================

namespace exercise6 {
    struct User {
        std::string name;
        std::string email;
        int age;
    };

    // TODO: 实现验证函数（返回 optional）
    auto validate_name = [](const User& u) -> std::optional<User> {
        // 你的代码：name 不能为空
        (void)u;
        return std::nullopt;
    };

    auto validate_email = [](const User& u) -> std::optional<User> {
        // 你的代码：email 必须包含 '@'
        (void)u;
        return std::nullopt;
    };

    auto validate_age = [](const User& u) -> std::optional<User> {
        // 你的代码：age 必须在 0-150 之间
        (void)u;
        return std::nullopt;
    };

    // TODO: 实现链式验证
    auto validate_user = [](const User& u) -> std::optional<User> {
        // 你的代码：依次应用所有验证
        // 提示：如果前一个返回 nullopt，停止验证
        (void)validate_name; (void)validate_email; (void)validate_age;
        return u;
    };
}

// ============================================
// 测试代码
// ============================================

void test_exercise_1() {
    std::cout << "=== Exercise 1: Basic Composition ===\n";

    using namespace exercise1;

    auto add10 = [](int x) { return x + 10; };
    auto mul2 = [](int x) { return x * 2; };
    auto sub5 = [](int x) { return x - 5; };

    auto f = compose(add10, mul2);
    std::cout << "compose(+10, *2)(5) = " << f(5)
              << " (expected: 20)\n";

    auto g = pipe(mul2, add10);
    std::cout << "pipe(*2, +10)(5) = " << g(5)
              << " (expected: 20)\n";

    auto h = compose3(add10, mul2, sub5);
    std::cout << "compose3(+10, *2, -5)(10) = " << h(10)
              << " (expected: 20)\n\n";
}

void test_exercise_2() {
    std::cout << "=== Exercise 2: Text Pipeline ===\n";

    using namespace exercise2;

    std::string input1 = "  Hello World  ";
    std::cout << "to_slug(\"" << input1 << "\") = \""
              << to_slug(input1) << "\" (expected: \"hello_world\")\n";

    std::string input2 = "My Blog Post";
    std::cout << "to_slug(\"" << input2 << "\") = \""
              << to_slug(input2) << "\" (expected: \"my_blog_post\")\n\n";
}

void test_exercise_3() {
    std::cout << "=== Exercise 3: Combinators ===\n";

    using namespace exercise3;

    auto subtract = [](int a, int b) { return a - b; };
    auto flipped = flip(subtract);
    std::cout << "flip(subtract)(10, 3) = " << flipped(10, 3)
              << " (expected: -7)\n";

    auto always_42 = const_fn(42);
    std::cout << "const_fn(42)(1, 2, 3) = " << always_42(1, 2, 3)
              << " (expected: 42)\n";

    auto double_it = [](int x) { return x * 2; };
    auto quad = apply_twice(double_it);
    std::cout << "apply_twice(*2)(5) = " << quad(5)
              << " (expected: 20)\n\n";
}

void test_exercise_4() {
    std::cout << "=== Exercise 4: Price Calculation ===\n";

    using namespace exercise4;

    double price = 100.0;
    auto final = calculate_final_price(price);
    std::cout << "calculate_final_price(100) = " << final
              << " (expected: 102.2)\n";
    std::cout << "  Steps: 100 -> 90 (10% off) -> 97.2 (8% tax) -> 102.2 (+$5 shipping)\n\n";
}

void test_exercise_5() {
    std::cout << "=== Exercise 5: Variadic Composition ===\n";

    using namespace exercise5;

    auto add5 = [](int x) { return x + 5; };
    auto mul3 = [](int x) { return x * 3; };
    auto sub2 = [](int x) { return x - 2; };

    auto f = pipe_many(sub2, mul3, add5);
    std::cout << "pipe_many(-2, *3, +5)(10) = " << f(10)
              << " (expected: 29)\n";

    auto g = compose_many(add5, mul3, sub2);
    std::cout << "compose_many(+5, *3, -2)(10) = " << g(10)
              << " (expected: 29)\n\n";
}

void test_exercise_6() {
    std::cout << "=== Exercise 6: Data Validation ===\n";

    using namespace exercise6;

    User valid_user{"Alice", "alice@example.com", 30};
    auto result1 = validate_user(valid_user);
    std::cout << "Valid user: "
              << (result1.has_value() ? "passed" : "failed")
              << " (expected: passed)\n";

    User invalid_name{"", "bob@example.com", 25};
    auto result2 = validate_user(invalid_name);
    std::cout << "Empty name: "
              << (result2.has_value() ? "passed" : "failed")
              << " (expected: failed)\n";

    User invalid_email{"Charlie", "charlie.com", 35};
    auto result3 = validate_user(invalid_email);
    std::cout << "Invalid email: "
              << (result3.has_value() ? "passed" : "failed")
              << " (expected: failed)\n";

    User invalid_age{"Diana", "diana@example.com", 200};
    auto result4 = validate_user(invalid_age);
    std::cout << "Invalid age: "
              << (result4.has_value() ? "passed" : "failed")
              << " (expected: failed)\n\n";
}

int main() {
    std::cout << "C++ Functional Programming - Chapter 05 Exercises\n";
    std::cout << "=================================================\n\n";

    test_exercise_1();
    test_exercise_2();
    test_exercise_3();
    test_exercise_4();
    test_exercise_5();
    test_exercise_6();

    std::cout << "Hint: Check solutions in exercises/solutions/05_functional_composition_solutions.cpp\n";

    return 0;
}
