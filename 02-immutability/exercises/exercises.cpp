/**
 * @file exercises.cpp
 * @brief 第02章练习题：不可变性与const
 *
 * 完成下面的练习，然后运行程序验证结果。
 * 参考答案在 solutions.cpp
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

/**
 * 实现一个不可变矩形类
 *
 * 要求：
 * - 成员变量 width_ 和 height_ 应该是 const
 * - 提供 width(), height(), area(), perimeter() 方法
 * - resize(new_width, new_height) 返回新矩形
 * - move(dx, dy) 返回新矩形（移动左上角坐标）
 * - scale(factor) 返回宽高都乘以 factor 的新矩形
 * - 使用 [[nodiscard]] 和 const 正确标记
 * - 实现 C++20 默认比较 operator<=>
 */
class ImmutableRectangle {
    // TODO: 添加成员变量
    // const int x_, y_;        // 左上角坐标
    // const int width_, height_;

public:
    // TODO: 实现构造函数
    constexpr ImmutableRectangle(int x, int y, int width, int height)
        : x_(0), y_(0), width_(0), height_(0) {
        // 替换这个实现
    }

    // TODO: 实现访问器
    [[nodiscard]] constexpr int x() const { return 0; }
    [[nodiscard]] constexpr int y() const { return 0; }
    [[nodiscard]] constexpr int width() const { return 0; }
    [[nodiscard]] constexpr int height() const { return 0; }
    [[nodiscard]] constexpr int area() const { return 0; }
    [[nodiscard]] constexpr int perimeter() const { return 0; }

    // TODO: 实现"修改"操作（返回新对象）
    [[nodiscard]] constexpr ImmutableRectangle resize(int new_width, int new_height) const {
        return ImmutableRectangle{0, 0, 0, 0};  // 替换这行
    }

    [[nodiscard]] constexpr ImmutableRectangle move(int dx, int dy) const {
        return ImmutableRectangle{0, 0, 0, 0};  // 替换这行
    }

    [[nodiscard]] constexpr ImmutableRectangle scale(int factor) const {
        return ImmutableRectangle{0, 0, 0, 0};  // 替换这行
    }

    // TODO: 添加 C++20 默认比较
    // auto operator<=>(const ImmutableRectangle&) const = default;

private:
    // 临时占位，完成后删除
    int x_ = 0, y_ = 0, width_ = 0, height_ = 0;
};

void test_exercise_1() {
    std::cout << "=== 练习 1: ImmutableRectangle ===\n";

    constexpr auto r1 = ImmutableRectangle{0, 0, 10, 5};

    // 基本属性
    assert(r1.width() == 10);
    assert(r1.height() == 5);
    assert(r1.area() == 50);
    assert(r1.perimeter() == 30);

    // 不可变操作
    constexpr auto r2 = r1.resize(20, 10);
    assert(r2.width() == 20);
    assert(r2.height() == 10);
    assert(r1.width() == 10);  // r1 不变

    constexpr auto r3 = r1.move(5, 5);
    assert(r3.x() == 5);
    assert(r3.y() == 5);
    assert(r1.x() == 0);  // r1 不变

    constexpr auto r4 = r1.scale(2);
    assert(r4.width() == 20);
    assert(r4.height() == 10);

    std::cout << "通过!\n\n";
}

// ============================================
// 练习 2: ImmutableVector::reduce
// ============================================

/**
 * 为 ImmutableVector 添加 reduce 方法
 *
 * reduce 应该：
 * - 接受初始值和二元函数
 * - 从左到右累积所有元素
 * - 返回累积结果
 */
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

    /**
     * TODO: 实现 reduce 方法
     *
     * 示例：
     *   ImmutableVector<int> v{{1, 2, 3, 4}};
     *   int sum = v.reduce(0, [](int acc, int x) { return acc + x; });
     *   // sum = 10
     *
     * 提示：使用 std::accumulate 或手动遍历
     *
     * @param init 初始值
     * @param f 二元函数 (累积值, 当前元素) -> 新累积值
     * @return 最终累积值
     */
    template<typename R, typename F>
    [[nodiscard]] R reduce(R init, F f) const {
        // TODO: 实现这个函数
        return init;  // 替换这行
    }
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

    // 字符串连接
    const auto words = ImmutableVector<std::string>{
        std::vector<std::string>{"Hello", " ", "World"}
    };
    std::string joined = words.reduce(std::string{},
        [](const std::string& acc, const std::string& s) { return acc + s; });
    assert(joined == "Hello World");
    std::cout << "joined = \"" << joined << "\"\n";

    std::cout << "通过!\n\n";
}

// ============================================
// 练习 3: 修复危险代码
// ============================================

/**
 * 下面的类有安全问题，找出并修复它们
 *
 * 问题1: BadAccount 暴露了可变引用
 * 问题2: BadConfig 的 getter 没有标记 const
 */

// 问题代码 - 不要修改这个
class BadAccount {
    std::string name_;
    double balance_ = 0.0;
    std::vector<double> transactions_;

public:
    BadAccount(std::string name) : name_(std::move(name)) {}

    // 问题1: 返回可变引用
    std::string& name() { return name_; }

    // 问题2: 返回可变引用
    std::vector<double>& transactions() { return transactions_; }

    void deposit(double amount) {
        balance_ += amount;
        transactions_.push_back(amount);
    }
};

/**
 * TODO: 实现 SafeAccount 修复上述问题
 *
 * 要求：
 * - name() 返回 const 引用
 * - transactions() 返回 const 引用
 * - 所有 getter 标记 const
 * - 只有 deposit() 可以修改状态
 */
class SafeAccount {
    std::string name_;
    double balance_ = 0.0;
    std::vector<double> transactions_;

public:
    SafeAccount(std::string name) : name_(std::move(name)) {}

    // TODO: 实现安全的访问器
    // const std::string& name() const { ... }
    // double balance() const { ... }
    // const std::vector<double>& transactions() const { ... }

    void deposit(double amount) {
        balance_ += amount;
        transactions_.push_back(amount);
    }
};

void test_exercise_3() {
    std::cout << "=== 练习 3: 修复危险代码 ===\n";

    // BadAccount 的问题演示
    BadAccount bad{"Alice"};
    bad.deposit(100.0);
    bad.name() = "Hacker";           // 名字被篡改！
    bad.transactions().clear();       // 交易记录被清空！
    std::cout << "BadAccount 被篡改后: name=" << bad.name()
              << ", transactions.size=" << bad.transactions().size() << "\n";

    // SafeAccount 应该阻止这些操作
    SafeAccount safe{"Bob"};
    safe.deposit(100.0);
    // safe.name() = "Hacker";        // 应该编译错误
    // safe.transactions().clear();   // 应该编译错误

    // 验证（取消注释下面的代码来测试）
    // assert(safe.name() == "Bob");
    // assert(safe.balance() == 100.0);
    // assert(safe.transactions().size() == 1);

    std::cout << "SafeAccount 实现后取消注释验证代码\n";
    std::cout << "通过!\n\n";
}

// ============================================
// 练习 4: constexpr 计算
// ============================================

/**
 * 实现编译期计算的几何函数
 *
 * 要求：
 * - 所有函数都是 constexpr
 * - 能在 static_assert 中使用
 */

// TODO: 实现 constexpr 圆面积（使用 pi = 3.14159265359）
constexpr double circle_area(double radius) {
    return 0.0;  // 替换这行
}

// TODO: 实现 constexpr 三角形面积（海伦公式）
// area = sqrt(s * (s-a) * (s-b) * (s-c)) 其中 s = (a+b+c)/2
// 提示：需要实现一个 constexpr sqrt，或使用近似值
constexpr double triangle_area(double a, double b, double c) {
    return 0.0;  // 替换这行
}

// TODO: 实现 constexpr 判断是否为素数
constexpr bool is_prime(int n) {
    return false;  // 替换这行
}

void test_exercise_4() {
    std::cout << "=== 练习 4: constexpr 计算 ===\n";

    // 圆面积
    constexpr double area1 = circle_area(1.0);
    // static_assert(area1 > 3.14 && area1 < 3.15, "circle_area(1) should be ~pi");
    std::cout << "circle_area(1.0) = " << area1 << "\n";

    constexpr double area2 = circle_area(2.0);
    std::cout << "circle_area(2.0) = " << area2 << "\n";

    // 三角形面积（3-4-5 直角三角形面积 = 6）
    constexpr double tri_area = triangle_area(3.0, 4.0, 5.0);
    // static_assert(tri_area > 5.9 && tri_area < 6.1, "3-4-5 triangle area should be 6");
    std::cout << "triangle_area(3, 4, 5) = " << tri_area << "\n";

    // 素数判断
    // static_assert(is_prime(2), "2 is prime");
    // static_assert(is_prime(17), "17 is prime");
    // static_assert(!is_prime(4), "4 is not prime");
    // static_assert(!is_prime(15), "15 is not prime");

    std::cout << "is_prime(2) = " << is_prime(2) << "\n";
    std::cout << "is_prime(17) = " << is_prime(17) << "\n";
    std::cout << "is_prime(4) = " << is_prime(4) << "\n";

    std::cout << "实现后取消 static_assert 注释来验证编译期计算\n";
    std::cout << "通过!\n\n";
}

// ============================================
// main
// ============================================

int main() {
    std::cout << "========================================\n";
    std::cout << "    第02章练习：不可变性与const\n";
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
