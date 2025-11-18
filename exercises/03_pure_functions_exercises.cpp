#include <algorithm>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>
#include <cmath>

// ============================================
// 练习 1: 判断纯函数
// ============================================

namespace exercise1 {
    // TODO: 判断以下函数哪些是纯函数，哪些不是，为什么？
    // 在注释中写出你的答案

    // 函数 A
    int function_a(int x) {
        return x * 2;
    }
    // 是否纯函数？

    // 函数 B
    int global_state = 0;
    int function_b(int x) {
        return x + global_state;
    }
    // 是否纯函数？

    // 函数 C
    int function_c(int x) {
        static int counter = 0;
        counter++;
        return x + counter;
    }
    // 是否纯函数？

    // 函数 D
    int function_d(int x) {
        std::cout << x << std::endl;
        return x * 2;
    }
    // 是否纯函数？

    // 函数 E
    int function_e(const std::vector<int>& vec) {
        return std::accumulate(vec.begin(), vec.end(), 0);
    }
    // 是否纯函数？

    // 函数 F
    void function_f(std::vector<int>& vec, int value) {
        vec.push_back(value);
    }
    // 是否纯函数？
}

// ============================================
// 练习 2: 重构不纯函数
// ============================================

namespace exercise2 {
    // TODO: 将以下不纯函数重构为纯函数

    // 不纯：修改全局状态
    int total_sum = 0;
    void add_to_sum(int x) {
        total_sum += x;
    }

    // 重构为纯函数
    // int pure_add_to_sum(???) {
    //     // 你的代码
    // }

    // 不纯：修改参数
    void normalize_vector(std::vector<double>& vec) {
        double sum = std::accumulate(vec.begin(), vec.end(), 0.0);
        for (auto& x : vec) {
            x /= sum;
        }
    }

    // TODO: 实现纯函数版本
    std::vector<double> pure_normalize_vector(const std::vector<double>& vec) {
        // 你的代码
        return {};
    }

    // 不纯：依赖外部状态
    double tax_rate = 0.1;
    double calculate_price_with_tax(double price) {
        return price * (1 + tax_rate);
    }

    // TODO: 实现纯函数版本
    double pure_calculate_price_with_tax(double price, double rate) {
        // 你的代码
        return 0.0;
    }
}

// ============================================
// 练习 3: 数学函数库（全部纯函数）
// ============================================

namespace exercise3 {
    // TODO: 实现以下纯函数

    // 计算向量的模（长度）
    double vector_magnitude(const std::vector<double>& vec) {
        // 提示：sqrt(x1^2 + x2^2 + ... + xn^2)
        return 0.0;
    }

    // 向量点积
    double dot_product(const std::vector<double>& v1,
                      const std::vector<double>& v2) {
        // 提示：v1[0]*v2[0] + v1[1]*v2[1] + ...
        return 0.0;
    }

    // 向量归一化（单位向量）
    std::vector<double> normalize(const std::vector<double>& vec) {
        // 提示：每个元素除以向量模
        return {};
    }

    // 向量加法
    std::vector<double> vector_add(const std::vector<double>& v1,
                                   const std::vector<double>& v2) {
        // 提示：对应元素相加
        return {};
    }

    // 向量数乘
    std::vector<double> scalar_multiply(const std::vector<double>& vec,
                                        double scalar) {
        // 提示：每个元素乘以标量
        return {};
    }
}

// ============================================
// 练习 4: 副作用分离
// ============================================

namespace exercise4 {
    // 给定一个混合了逻辑和 I/O 的函数
    // TODO: 将其拆分为纯函数（逻辑）和不纯函数（I/O）

    // 不好的实现：逻辑和 I/O 混在一起
    void process_scores_bad(const std::vector<int>& scores) {
        std::cout << "Processing scores..." << std::endl;

        int sum = 0;
        for (int score : scores) {
            sum += score;
        }
        double average = static_cast<double>(sum) / scores.size();

        std::cout << "Average score: " << average << std::endl;

        int passed = 0;
        for (int score : scores) {
            if (score >= 60) {
                passed++;
            }
        }
        double pass_rate = static_cast<double>(passed) / scores.size();

        std::cout << "Pass rate: " << (pass_rate * 100) << "%" << std::endl;
    }

    // TODO: 实现纯函数部分
    struct ScoreStats {
        double average;
        double pass_rate;
    };

    ScoreStats calculate_stats(const std::vector<int>& scores) {
        // 你的代码：计算统计信息
        return {0.0, 0.0};
    }

    // TODO: 实现 I/O 部分
    void print_stats(const ScoreStats& stats) {
        // 你的代码：打印统计信息
    }

    // 使用方式
    void process_scores_good(const std::vector<int>& scores) {
        auto stats = calculate_stats(scores);  // 纯函数
        print_stats(stats);                    // I/O
    }
}

// ============================================
// 练习 5: 函数组合
// ============================================

namespace exercise5 {
    // TODO: 实现通用的函数组合工具

    // compose(f, g)(x) = f(g(x))
    template<typename F, typename G>
    auto compose(F f, G g) {
        // 你的代码
        return [](auto x) { return x; };
    }

    // TODO: 使用 compose 创建新函数
    // 示例：将数字加倍然后加 10
    // auto double_then_add_10 = compose(???, ???);
}

// ============================================
// 测试代码
// ============================================

void test_exercise_1() {
    std::cout << "=== Exercise 1: Identify Pure Functions ===\n";
    std::cout << "Check your answers in the source code comments\n\n";
}

void test_exercise_2() {
    std::cout << "=== Exercise 2: Refactor to Pure Functions ===\n";

    using namespace exercise2;

    std::vector<double> vec = {1.0, 2.0, 3.0, 4.0};
    auto normalized = pure_normalize_vector(vec);

    std::cout << "Original: ";
    for (double x : vec) std::cout << x << " ";
    std::cout << "\nNormalized: ";
    for (double x : normalized) std::cout << x << " ";
    std::cout << "\n";

    std::cout << "Price with tax (100, 0.1): "
              << pure_calculate_price_with_tax(100.0, 0.1) << "\n";
    std::cout << "Expected: 110\n\n";
}

void test_exercise_3() {
    std::cout << "=== Exercise 3: Math Library ===\n";

    using namespace exercise3;

    std::vector<double> v1 = {3.0, 4.0};
    std::vector<double> v2 = {1.0, 2.0};

    std::cout << "v1 magnitude: " << vector_magnitude(v1) << " (expected: 5)\n";
    std::cout << "dot_product(v1, v2): " << dot_product(v1, v2) << " (expected: 11)\n";

    auto v_sum = vector_add(v1, v2);
    std::cout << "v1 + v2: [";
    for (double x : v_sum) std::cout << x << " ";
    std::cout << "] (expected: [4 6])\n";

    auto v_scaled = scalar_multiply(v1, 2.0);
    std::cout << "v1 * 2: [";
    for (double x : v_scaled) std::cout << x << " ";
    std::cout << "] (expected: [6 8])\n\n";
}

void test_exercise_4() {
    std::cout << "=== Exercise 4: Separate Side Effects ===\n";

    using namespace exercise4;

    std::vector<int> scores = {75, 82, 90, 55, 68, 95, 45, 88};
    process_scores_good(scores);
    std::cout << "\n";
}

void test_exercise_5() {
    std::cout << "=== Exercise 5: Function Composition ===\n";

    using namespace exercise5;

    auto double_it = [](int x) { return x * 2; };
    auto add_10 = [](int x) { return x + 10; };

    auto double_then_add_10 = compose(add_10, double_it);
    std::cout << "double_then_add_10(5) = " << double_then_add_10(5)
              << " (expected: 20)\n\n";
}

int main() {
    std::cout << "C++ Functional Programming - Chapter 03 Exercises\n";
    std::cout << "=================================================\n\n";

    test_exercise_1();
    test_exercise_2();
    test_exercise_3();
    test_exercise_4();
    test_exercise_5();

    std::cout << "Hint: Check solutions in exercises/solutions/03_pure_functions_solutions.cpp\n";

    return 0;
}
