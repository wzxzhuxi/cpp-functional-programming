/**
 * @file solutions.cpp
 * @brief 第02章练习题参考答案
 */

#include <algorithm>
#include <cassert>
#include <cmath>
#include <iostream>
#include <memory>
#include <numeric>
#include <string>
#include <vector>

// ============================================
// 练习 1: ImmutableRectangle
// ============================================

class ImmutableRectangle {
    const int x_;
    const int y_;
    const int width_;
    const int height_;

public:
    constexpr ImmutableRectangle(int x, int y, int width, int height)
        : x_(x), y_(y), width_(width), height_(height) {}

    [[nodiscard]] constexpr int x() const { return x_; }
    [[nodiscard]] constexpr int y() const { return y_; }
    [[nodiscard]] constexpr int width() const { return width_; }
    [[nodiscard]] constexpr int height() const { return height_; }

    [[nodiscard]] constexpr int area() const {
        return width_ * height_;
    }

    [[nodiscard]] constexpr int perimeter() const {
        return 2 * (width_ + height_);
    }

    [[nodiscard]] constexpr ImmutableRectangle resize(int new_width, int new_height) const {
        return ImmutableRectangle{x_, y_, new_width, new_height};
    }

    [[nodiscard]] constexpr ImmutableRectangle move(int dx, int dy) const {
        return ImmutableRectangle{x_ + dx, y_ + dy, width_, height_};
    }

    [[nodiscard]] constexpr ImmutableRectangle scale(int factor) const {
        return ImmutableRectangle{x_, y_, width_ * factor, height_ * factor};
    }

    // C++20 默认比较
    auto operator<=>(const ImmutableRectangle&) const = default;
};

void test_exercise_1() {
    std::cout << "=== 练习 1: ImmutableRectangle ===\n";

    constexpr auto r1 = ImmutableRectangle{0, 0, 10, 5};

    // 基本属性
    static_assert(r1.width() == 10);
    static_assert(r1.height() == 5);
    static_assert(r1.area() == 50);
    static_assert(r1.perimeter() == 30);

    std::cout << "r1: (" << r1.x() << ", " << r1.y() << ") "
              << r1.width() << "x" << r1.height()
              << " area=" << r1.area()
              << " perimeter=" << r1.perimeter() << "\n";

    // resize - 不改变原对象
    constexpr auto r2 = r1.resize(20, 10);
    static_assert(r2.width() == 20);
    static_assert(r2.height() == 10);
    static_assert(r1.width() == 10);  // r1 不变

    std::cout << "r2 = r1.resize(20, 10): " << r2.width() << "x" << r2.height() << "\n";
    std::cout << "r1 仍然是: " << r1.width() << "x" << r1.height() << "\n";

    // move - 不改变原对象
    constexpr auto r3 = r1.move(5, 5);
    static_assert(r3.x() == 5);
    static_assert(r3.y() == 5);
    static_assert(r1.x() == 0);

    std::cout << "r3 = r1.move(5, 5): (" << r3.x() << ", " << r3.y() << ")\n";

    // scale - 不改变原对象
    constexpr auto r4 = r1.scale(2);
    static_assert(r4.width() == 20);
    static_assert(r4.height() == 10);

    std::cout << "r4 = r1.scale(2): " << r4.width() << "x" << r4.height() << "\n";

    // C++20 比较
    static_assert(r1 != r2);
    static_assert(r1 < r2);  // 字典序

    std::cout << "通过!\n\n";
}

// ============================================
// 练习 2: ImmutableVector::reduce
// ============================================

template<typename T>
class ImmutableVector {
    std::shared_ptr<const std::vector<T>> data_;

public:
    ImmutableVector()
        : data_(std::make_shared<const std::vector<T>>()) {}

    explicit ImmutableVector(std::vector<T> vec)
        : data_(std::make_shared<const std::vector<T>>(std::move(vec))) {}

    [[nodiscard]] const T& operator[](size_t i) const { return (*data_)[i]; }
    [[nodiscard]] size_t size() const { return data_->size(); }
    [[nodiscard]] bool empty() const { return data_->empty(); }
    [[nodiscard]] auto begin() const { return data_->begin(); }
    [[nodiscard]] auto end() const { return data_->end(); }

    [[nodiscard]] ImmutableVector push_back(T value) const {
        auto new_data = *data_;
        new_data.push_back(std::move(value));
        return ImmutableVector{std::move(new_data)};
    }

    // 方法1: 使用 std::accumulate
    template<typename R, typename F>
    [[nodiscard]] R reduce(R init, F f) const {
        return std::accumulate(data_->begin(), data_->end(), init, f);
    }

    // 方法2: 手动遍历（可选）
    // template<typename R, typename F>
    // [[nodiscard]] R reduce(R init, F f) const {
    //     R acc = init;
    //     for (const auto& item : *data_) {
    //         acc = f(acc, item);
    //     }
    //     return acc;
    // }
};

void test_exercise_2() {
    std::cout << "=== 练习 2: ImmutableVector::reduce ===\n";

    const auto nums = ImmutableVector<int>{std::vector{1, 2, 3, 4, 5}};

    // 求和
    int sum = nums.reduce(0, [](int acc, int x) { return acc + x; });
    assert(sum == 15);
    std::cout << "sum = " << sum << "\n";

    // 求积
    int product = nums.reduce(1, [](int acc, int x) { return acc * x; });
    assert(product == 120);
    std::cout << "product = " << product << "\n";

    // 最大值
    int max_val = nums.reduce(nums[0], [](int acc, int x) { return std::max(acc, x); });
    assert(max_val == 5);
    std::cout << "max = " << max_val << "\n";

    // 字符串连接
    const auto words = ImmutableVector<std::string>{
        std::vector<std::string>{"Hello", " ", "World"}
    };
    std::string joined = words.reduce(std::string{},
        [](const std::string& acc, const std::string& s) { return acc + s; });
    assert(joined == "Hello World");
    std::cout << "joined = \"" << joined << "\"\n";

    // 复杂类型：计算平均值
    auto [total, count] = nums.reduce(
        std::pair{0.0, 0},
        [](std::pair<double, int> acc, int x) {
            return std::pair{acc.first + x, acc.second + 1};
        });
    double average = total / count;
    std::cout << "average = " << average << "\n";

    std::cout << "通过!\n\n";
}

// ============================================
// 练习 3: 修复危险代码
// ============================================

class SafeAccount {
    std::string name_;
    double balance_ = 0.0;
    std::vector<double> transactions_;

public:
    explicit SafeAccount(std::string name) : name_(std::move(name)) {}

    // 只读访问，返回 const 引用
    [[nodiscard]] const std::string& name() const { return name_; }
    [[nodiscard]] double balance() const { return balance_; }
    [[nodiscard]] const std::vector<double>& transactions() const { return transactions_; }

    // 只有这些方法可以修改状态
    void deposit(double amount) {
        if (amount > 0) {
            balance_ += amount;
            transactions_.push_back(amount);
        }
    }

    void withdraw(double amount) {
        if (amount > 0 && amount <= balance_) {
            balance_ -= amount;
            transactions_.push_back(-amount);
        }
    }
};

void test_exercise_3() {
    std::cout << "=== 练习 3: 修复危险代码 ===\n";

    SafeAccount safe{"Bob"};
    safe.deposit(100.0);
    safe.deposit(50.0);
    safe.withdraw(30.0);

    // 这些操作现在会编译错误：
    // safe.name() = "Hacker";         // 错误：不能修改 const 引用
    // safe.transactions().clear();    // 错误：不能修改 const 引用

    assert(safe.name() == "Bob");
    assert(safe.balance() == 120.0);  // 100 + 50 - 30
    assert(safe.transactions().size() == 3);

    std::cout << "账户: " << safe.name() << "\n";
    std::cout << "余额: " << safe.balance() << "\n";
    std::cout << "交易记录:\n";
    for (double t : safe.transactions()) {
        std::cout << "  " << (t >= 0 ? "+" : "") << t << "\n";
    }

    std::cout << "通过!\n\n";
}

// ============================================
// 练习 4: constexpr 计算
// ============================================

// constexpr 圆面积
constexpr double PI = 3.14159265358979323846;

constexpr double circle_area(double radius) {
    return PI * radius * radius;
}

// constexpr 开方（牛顿迭代法）
constexpr double constexpr_sqrt(double x) {
    if (x < 0) return -1;  // 错误情况
    if (x == 0) return 0;

    double guess = x;
    double prev = 0;

    // 迭代直到收敛
    while (guess != prev) {
        prev = guess;
        guess = (guess + x / guess) / 2;
    }

    return guess;
}

// constexpr 三角形面积（海伦公式）
constexpr double triangle_area(double a, double b, double c) {
    double s = (a + b + c) / 2;
    return constexpr_sqrt(s * (s - a) * (s - b) * (s - c));
}

// constexpr 素数判断
constexpr bool is_prime(int n) {
    if (n <= 1) return false;
    if (n <= 3) return true;
    if (n % 2 == 0 || n % 3 == 0) return false;

    // 只检查 6k±1 形式的数
    for (int i = 5; i * i <= n; i += 6) {
        if (n % i == 0 || n % (i + 2) == 0) {
            return false;
        }
    }

    return true;
}

void test_exercise_4() {
    std::cout << "=== 练习 4: constexpr 计算 ===\n";

    // 圆面积 - 编译期计算
    constexpr double area1 = circle_area(1.0);
    static_assert(area1 > 3.14 && area1 < 3.15, "circle_area(1) should be ~pi");
    std::cout << "circle_area(1.0) = " << area1 << "\n";

    constexpr double area2 = circle_area(2.0);
    static_assert(area2 > 12.56 && area2 < 12.57, "circle_area(2) should be ~4pi");
    std::cout << "circle_area(2.0) = " << area2 << "\n";

    // 开方 - 编译期计算
    constexpr double sqrt_2 = constexpr_sqrt(2.0);
    static_assert(sqrt_2 > 1.41 && sqrt_2 < 1.42, "sqrt(2) should be ~1.414");
    std::cout << "constexpr_sqrt(2.0) = " << sqrt_2 << "\n";

    // 三角形面积 - 编译期计算
    constexpr double tri_area = triangle_area(3.0, 4.0, 5.0);
    static_assert(tri_area > 5.99 && tri_area < 6.01, "3-4-5 triangle area should be 6");
    std::cout << "triangle_area(3, 4, 5) = " << tri_area << "\n";

    // 等边三角形（边长2）面积 = sqrt(3) ≈ 1.732
    constexpr double equilateral = triangle_area(2.0, 2.0, 2.0);
    std::cout << "triangle_area(2, 2, 2) = " << equilateral << "\n";

    // 素数判断 - 编译期计算
    static_assert(is_prime(2), "2 is prime");
    static_assert(is_prime(3), "3 is prime");
    static_assert(!is_prime(4), "4 is not prime");
    static_assert(is_prime(5), "5 is prime");
    static_assert(!is_prime(6), "6 is not prime");
    static_assert(is_prime(7), "7 is prime");
    static_assert(is_prime(17), "17 is prime");
    static_assert(is_prime(97), "97 is prime");
    static_assert(!is_prime(100), "100 is not prime");

    std::cout << "素数判断:\n";
    for (int n = 2; n <= 20; ++n) {
        if (is_prime(n)) {
            std::cout << n << " ";
        }
    }
    std::cout << "\n";

    // 使用 consteval 强制编译期（C++20）
    // consteval 版本
    // consteval bool must_be_prime(int n) { return is_prime(n); }
    // static_assert(must_be_prime(17));

    std::cout << "通过!\n\n";
}

// ============================================
// main
// ============================================

int main() {
    std::cout << "========================================\n";
    std::cout << "    第02章练习参考答案\n";
    std::cout << "========================================\n\n";

    test_exercise_1();
    test_exercise_2();
    test_exercise_3();
    test_exercise_4();

    std::cout << "========================================\n";
    std::cout << "    所有练习完成!\n";
    std::cout << "========================================\n";

    return 0;
}
