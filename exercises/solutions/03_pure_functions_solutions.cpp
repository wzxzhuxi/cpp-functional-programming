#include <algorithm>
#include <cmath>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>

// ============================================
// 练习 1: 判断纯函数
// ============================================

namespace exercise1 {
    // 函数 A: 纯函数
    // 理由：相同输入 -> 相同输出，无副作用
    int function_a(int x) {
        return x * 2;
    }

    // 函数 B: 不纯
    // 理由：依赖全局可变状态 global_state
    int global_state = 0;
    int function_b(int x) {
        return x + global_state;
    }

    // 函数 C: 不纯
    // 理由：static 变量在调用间保持状态，每次调用结果不同
    int function_c(int x) {
        static int counter = 0;
        counter++;
        return x + counter;
    }

    // 函数 D: 不纯
    // 理由：有副作用（I/O 操作：std::cout）
    int function_d(int x) {
        std::cout << x << std::endl;
        return x * 2;
    }

    // 函数 E: 纯函数
    // 理由：相同输入 -> 相同输出，不修改参数（const 引用）
    int function_e(const std::vector<int>& vec) {
        return std::accumulate(vec.begin(), vec.end(), 0);
    }

    // 函数 F: 不纯
    // 理由：修改参数（副作用）
    void function_f(std::vector<int>& vec, int value) {
        vec.push_back(value);
    }
}

// ============================================
// 练习 2: 重构不纯函数
// ============================================

namespace exercise2 {
    // 纯函数版本：显式传递状态
    int pure_add_to_sum(int current_sum, int x) {
        return current_sum + x;
    }

    // 纯函数版本：返回新向量
    std::vector<double> pure_normalize_vector(const std::vector<double>& vec) {
        double sum = std::accumulate(vec.begin(), vec.end(), 0.0);
        std::vector<double> result;
        result.reserve(vec.size());
        for (double x : vec) {
            result.push_back(x / sum);
        }
        return result;
    }

    // 纯函数版本：税率作为参数
    double pure_calculate_price_with_tax(double price, double rate) {
        return price * (1 + rate);
    }
}

// ============================================
// 练习 3: 数学函数库
// ============================================

namespace exercise3 {
    double vector_magnitude(const std::vector<double>& vec) {
        double sum_of_squares = std::accumulate(
            vec.begin(), vec.end(), 0.0,
            [](double acc, double x) { return acc + x * x; }
        );
        return std::sqrt(sum_of_squares);
    }

    double dot_product(const std::vector<double>& v1,
                      const std::vector<double>& v2) {
        double result = 0.0;
        for (size_t i = 0; i < std::min(v1.size(), v2.size()); ++i) {
            result += v1[i] * v2[i];
        }
        return result;
    }

    std::vector<double> normalize(const std::vector<double>& vec) {
        double mag = vector_magnitude(vec);
        std::vector<double> result;
        result.reserve(vec.size());
        for (double x : vec) {
            result.push_back(x / mag);
        }
        return result;
    }

    std::vector<double> vector_add(const std::vector<double>& v1,
                                   const std::vector<double>& v2) {
        std::vector<double> result;
        size_t size = std::max(v1.size(), v2.size());
        result.reserve(size);

        for (size_t i = 0; i < size; ++i) {
            double val1 = i < v1.size() ? v1[i] : 0.0;
            double val2 = i < v2.size() ? v2[i] : 0.0;
            result.push_back(val1 + val2);
        }
        return result;
    }

    std::vector<double> scalar_multiply(const std::vector<double>& vec,
                                        double scalar) {
        std::vector<double> result;
        result.reserve(vec.size());
        for (double x : vec) {
            result.push_back(x * scalar);
        }
        return result;
    }
}

// ============================================
// 练习 4: 副作用分离
// ============================================

namespace exercise4 {
    struct ScoreStats {
        double average;
        double pass_rate;
    };

    // 纯函数：计算统计
    ScoreStats calculate_stats(const std::vector<int>& scores) {
        if (scores.empty()) {
            return {0.0, 0.0};
        }

        int sum = std::accumulate(scores.begin(), scores.end(), 0);
        double average = static_cast<double>(sum) / scores.size();

        int passed = std::count_if(scores.begin(), scores.end(),
                                    [](int score) { return score >= 60; });
        double pass_rate = static_cast<double>(passed) / scores.size();

        return {average, pass_rate};
    }

    // 不纯：I/O 操作
    void print_stats(const ScoreStats& stats) {
        std::cout << "Average score: " << stats.average << "\n";
        std::cout << "Pass rate: " << (stats.pass_rate * 100) << "%\n";
    }

    void process_scores_good(const std::vector<int>& scores) {
        auto stats = calculate_stats(scores);  // 纯函数
        print_stats(stats);                    // I/O
    }
}

// ============================================
// 练习 5: 函数组合
// ============================================

namespace exercise5 {
    template<typename F, typename G>
    auto compose(F f, G g) {
        return [f, g](auto x) { return f(g(x)); };
    }
}

// ============================================
// 测试代码
// ============================================

void test_exercise_1() {
    std::cout << "=== Exercise 1: Identify Pure Functions ===\n";
    std::cout << "A: Pure (deterministic, no side effects)\n";
    std::cout << "B: Impure (depends on global state)\n";
    std::cout << "C: Impure (static state changes between calls)\n";
    std::cout << "D: Impure (I/O side effect)\n";
    std::cout << "E: Pure (deterministic, const input)\n";
    std::cout << "F: Impure (modifies parameter)\n\n";
}

void test_exercise_2() {
    std::cout << "=== Exercise 2: Refactor to Pure Functions ===\n";

    using namespace exercise2;

    // 测试累加
    int sum = 0;
    sum = pure_add_to_sum(sum, 10);
    sum = pure_add_to_sum(sum, 20);
    std::cout << "Sum: " << sum << " (expected: 30)\n";

    // 测试归一化
    std::vector<double> vec = {1.0, 2.0, 3.0, 4.0};
    auto normalized = pure_normalize_vector(vec);

    std::cout << "Original: ";
    for (double x : vec) std::cout << x << " ";
    std::cout << "\nNormalized: ";
    for (double x : normalized) std::cout << x << " ";
    std::cout << "\n";

    // 测试价格计算
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
    std::cout << "] (expected: [6 8])\n";

    auto v_norm = normalize(v1);
    std::cout << "normalized v1: [";
    for (double x : v_norm) std::cout << x << " ";
    std::cout << "] (magnitude should be 1)\n";
    std::cout << "magnitude of normalized: " << vector_magnitude(v_norm) << "\n\n";
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
    auto square = [](int x) { return x * x; };

    auto double_then_add_10 = compose(add_10, double_it);
    auto square_then_double = compose(double_it, square);

    std::cout << "double_then_add_10(5) = " << double_then_add_10(5)
              << " (expected: 20)\n";
    std::cout << "square_then_double(5) = " << square_then_double(5)
              << " (expected: 50)\n\n";
}

int main() {
    std::cout << "C++ Functional Programming - Chapter 03 Solutions\n";
    std::cout << "=================================================\n\n";

    test_exercise_1();
    test_exercise_2();
    test_exercise_3();
    test_exercise_4();
    test_exercise_5();

    return 0;
}
