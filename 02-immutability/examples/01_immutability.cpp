/**
 * @file 01_immutability.cpp
 * @brief 不可变性与const示例
 *
 * 本文件演示：
 * - const 变量和引用
 * - 指针的 const
 * - const 成员函数
 * - constexpr 和 consteval
 * - 不可变类设计
 * - 持久化数据结构
 */

#include <algorithm>
#include <cmath>
#include <iostream>
#include <memory>
#include <numeric>
#include <string>
#include <vector>

// ============================================
// 1. const 基础
// ============================================

void const_basics_demo() {
    std::cout << "=== const 基础 ===\n";

    // 常量变量
    const int x = 42;
    // x = 43;  // 编译错误！

    std::cout << "const int x = " << x << "\n";

    // 非 const 变量
    int y = 100;
    y = 200;  // OK
    std::cout << "可变 y = " << y << "\n\n";
}

// const 引用示例
void print_string(const std::string& s) {
    std::cout << "字符串: " << s << "\n";
    // s += "!";  // 编译错误：不能通过 const 引用修改
}

void const_reference_demo() {
    std::cout << "=== const 引用 ===\n";

    std::string msg = "Hello, Functional C++";
    print_string(msg);  // 不拷贝，安全传递

    // const 引用可以绑定临时对象
    print_string("临时字符串也可以");

    std::cout << "\n";
}

// ============================================
// 2. 指针的 const
// ============================================

void const_pointer_demo() {
    std::cout << "=== 指针的 const ===\n";

    int value = 10;
    int other = 20;

    // 指向 const int 的指针（不能通过 p1 修改 value）
    const int* p1 = &value;
    // *p1 = 15;    // 编译错误
    p1 = &other;    // OK，可以指向别处
    std::cout << "const int* p1 指向: " << *p1 << "\n";

    // const 指针（p2 不能指向别处）
    int* const p2 = &value;
    *p2 = 15;       // OK，可以修改值
    // p2 = &other; // 编译错误
    std::cout << "int* const p2 指向的值: " << *p2 << "\n";

    // 两者都是 const
    const int* const p3 = &value;
    // *p3 = 20;    // 编译错误
    // p3 = &other; // 编译错误
    std::cout << "const int* const p3: " << *p3 << "\n\n";
}

// ============================================
// 3. const 成员函数
// ============================================

class Point {
    int x_, y_;

public:
    Point(int x, int y) : x_(x), y_(y) {}

    // const 成员函数：承诺不修改对象状态
    [[nodiscard]] int x() const { return x_; }
    [[nodiscard]] int y() const { return y_; }

    [[nodiscard]] double distance() const {
        return std::sqrt(static_cast<double>(x_ * x_ + y_ * y_));
    }

    // 非 const 函数：可以修改状态
    void set_x(int x) { x_ = x; }
    void set_y(int y) { y_ = y; }

    void print() const {
        std::cout << "Point(" << x_ << ", " << y_ << ")";
    }
};

void const_member_function_demo() {
    std::cout << "=== const 成员函数 ===\n";

    const Point p1{3, 4};
    std::cout << "const Point p1: ";
    p1.print();
    std::cout << "\n";
    std::cout << "p1.x() = " << p1.x() << "\n";
    std::cout << "p1.distance() = " << p1.distance() << "\n";
    // p1.set_x(5);  // 编译错误：const 对象不能调用非 const 成员函数

    Point p2{0, 0};
    p2.set_x(6);
    p2.set_y(8);
    std::cout << "非 const Point p2: ";
    p2.print();
    std::cout << " distance = " << p2.distance() << "\n\n";
}

// ============================================
// 4. constexpr：编译期常量
// ============================================

constexpr int square(int x) {
    return x * x;
}

constexpr int factorial(int n) {
    return n <= 1 ? 1 : n * factorial(n - 1);
}

// C++20: consteval 强制编译期求值
consteval int compile_time_only(int x) {
    return x * x * x;
}

void constexpr_demo() {
    std::cout << "=== constexpr ===\n";

    // 编译期常量
    constexpr int max_size = 100;
    constexpr int s = square(5);
    constexpr int f = factorial(5);

    static_assert(s == 25, "编译期计算错误");
    static_assert(f == 120, "阶乘计算错误");

    std::cout << "constexpr max_size = " << max_size << "\n";
    std::cout << "constexpr square(5) = " << s << "\n";
    std::cout << "constexpr factorial(5) = " << f << "\n";

    // 运行时也能调用 constexpr 函数
    int n = 7;
    int result = square(n);  // 运行时计算
    std::cout << "运行时 square(7) = " << result << "\n";

    // consteval 只能编译期
    constexpr int cube = compile_time_only(3);
    std::cout << "consteval compile_time_only(3) = " << cube << "\n";
    // int runtime_cube = compile_time_only(n);  // 编译错误！

    std::cout << "\n";
}

// ============================================
// 5. 不可变类设计
// ============================================

class ImmutablePoint {
    const int x_;
    const int y_;

public:
    constexpr ImmutablePoint(int x, int y) : x_(x), y_(y) {}

    [[nodiscard]] constexpr int x() const { return x_; }
    [[nodiscard]] constexpr int y() const { return y_; }

    // "修改"操作返回新对象
    [[nodiscard]] constexpr ImmutablePoint move(int dx, int dy) const {
        return ImmutablePoint{x_ + dx, y_ + dy};
    }

    [[nodiscard]] constexpr ImmutablePoint scale(int factor) const {
        return ImmutablePoint{x_ * factor, y_ * factor};
    }

    [[nodiscard]] constexpr ImmutablePoint negate() const {
        return ImmutablePoint{-x_, -y_};
    }

    // C++20: 默认比较
    auto operator<=>(const ImmutablePoint&) const = default;

    void print() const {
        std::cout << "ImmutablePoint(" << x_ << ", " << y_ << ")";
    }
};

void immutable_class_demo() {
    std::cout << "=== 不可变类 ===\n";

    constexpr auto p1 = ImmutablePoint{3, 4};
    constexpr auto p2 = p1.move(1, 1);   // p1 不变
    constexpr auto p3 = p2.scale(2);     // p2 不变
    constexpr auto p4 = p3.negate();     // p3 不变

    static_assert(p1.x() == 3, "p1 应该不变");
    static_assert(p2.x() == 4, "p2 = p1.move(1,1)");
    static_assert(p3.x() == 8, "p3 = p2.scale(2)");
    static_assert(p4.x() == -8, "p4 = p3.negate()");

    std::cout << "p1 = "; p1.print(); std::cout << "\n";
    std::cout << "p2 = p1.move(1,1) = "; p2.print(); std::cout << "\n";
    std::cout << "p3 = p2.scale(2) = "; p3.print(); std::cout << "\n";
    std::cout << "p4 = p3.negate() = "; p4.print(); std::cout << "\n";

    // C++20 比较
    static_assert(p1 != p2);
    static_assert(p1 < p2);  // 字典序比较
    std::cout << "p1 < p2: " << (p1 < p2 ? "true" : "false") << "\n\n";
}

// ============================================
// 6. 不可变容器
// ============================================

template<typename T>
class ImmutableVector {
    std::shared_ptr<const std::vector<T>> data_;

public:
    ImmutableVector()
        : data_(std::make_shared<const std::vector<T>>()) {}

    explicit ImmutableVector(std::vector<T> vec)
        : data_(std::make_shared<const std::vector<T>>(std::move(vec))) {}

    // 只读访问
    [[nodiscard]] const T& operator[](size_t i) const { return (*data_)[i]; }
    [[nodiscard]] size_t size() const { return data_->size(); }
    [[nodiscard]] bool empty() const { return data_->empty(); }

    // 迭代器支持
    [[nodiscard]] auto begin() const { return data_->begin(); }
    [[nodiscard]] auto end() const { return data_->end(); }

    // "修改"操作返回新容器
    [[nodiscard]] ImmutableVector push_back(T value) const {
        auto new_data = *data_;  // 复制
        new_data.push_back(std::move(value));
        return ImmutableVector{std::move(new_data)};
    }

    [[nodiscard]] ImmutableVector set(size_t i, T value) const {
        auto new_data = *data_;
        new_data[i] = std::move(value);
        return ImmutableVector{std::move(new_data)};
    }

    [[nodiscard]] ImmutableVector pop_back() const {
        if (data_->empty()) return *this;
        auto new_data = *data_;
        new_data.pop_back();
        return ImmutableVector{std::move(new_data)};
    }

    // 函数式操作
    template<typename F>
    [[nodiscard]] auto map(F f) const {
        using R = std::invoke_result_t<F, const T&>;
        std::vector<R> result;
        result.reserve(size());
        for (const auto& item : *data_) {
            result.push_back(f(item));
        }
        return ImmutableVector<R>{std::move(result)};
    }

    template<typename Pred>
    [[nodiscard]] ImmutableVector filter(Pred pred) const {
        std::vector<T> result;
        std::copy_if(data_->begin(), data_->end(),
                     std::back_inserter(result), pred);
        return ImmutableVector{std::move(result)};
    }

    template<typename R, typename F>
    [[nodiscard]] R reduce(R init, F f) const {
        return std::accumulate(data_->begin(), data_->end(), init, f);
    }
};

void immutable_vector_demo() {
    std::cout << "=== 不可变容器 ===\n";

    const auto v1 = ImmutableVector<int>{std::vector{1, 2, 3}};
    const auto v2 = v1.push_back(4);     // v1 不变
    const auto v3 = v2.set(0, 10);       // v2 不变
    const auto v4 = v3.pop_back();       // v3 不变

    auto print_vec = [](const auto& v, const char* name) {
        std::cout << name << " = [";
        for (size_t i = 0; i < v.size(); ++i) {
            if (i > 0) std::cout << ", ";
            std::cout << v[i];
        }
        std::cout << "]\n";
    };

    print_vec(v1, "v1");
    print_vec(v2, "v2 = v1.push_back(4)");
    print_vec(v3, "v3 = v2.set(0, 10)");
    print_vec(v4, "v4 = v3.pop_back()");

    // 函数式操作
    const auto doubled = v1.map([](int x) { return x * 2; });
    const auto evens = v2.filter([](int x) { return x % 2 == 0; });
    const auto sum = v2.reduce(0, [](int acc, int x) { return acc + x; });

    print_vec(doubled, "v1.map(*2)");
    print_vec(evens, "v2.filter(偶数)");
    std::cout << "v2.reduce(+) = " << sum << "\n\n";
}

// ============================================
// 7. 可变状态的危害
// ============================================

// 危险示例：暴露可变引用
class BadContainer {
    std::vector<int> data_{1, 2, 3};
public:
    std::vector<int>& data() { return data_; }  // 危险！
};

// 安全示例：只暴露 const 引用
class SafeContainer {
    std::vector<int> data_{1, 2, 3};
public:
    [[nodiscard]] const std::vector<int>& data() const { return data_; }

    // 如需修改，提供明确的方法
    void append(int value) { data_.push_back(value); }
};

void mutable_dangers_demo() {
    std::cout << "=== 可变状态的危害 ===\n";

    // BadContainer 的问题
    BadContainer bad;
    std::cout << "BadContainer 初始: ";
    for (int n : bad.data()) std::cout << n << " ";
    std::cout << "\n";

    bad.data().clear();  // 外部代码清空了内部数据！
    std::cout << "bad.data().clear() 后: size = " << bad.data().size() << "\n";

    // SafeContainer 更安全
    SafeContainer safe;
    // safe.data().clear();  // 编译错误：const 引用
    safe.append(4);
    std::cout << "SafeContainer: ";
    for (int n : safe.data()) std::cout << n << " ";
    std::cout << "\n\n";
}

// ============================================
// 8. 持久化数据结构：链表
// ============================================

template<typename T>
class PersistentList {
    struct Node {
        T value;
        std::shared_ptr<const Node> next;
    };

    std::shared_ptr<const Node> head_;
    size_t size_;

    explicit PersistentList(std::shared_ptr<const Node> head, size_t size)
        : head_(std::move(head)), size_(size) {}

public:
    PersistentList() : head_(nullptr), size_(0) {}

    // O(1) 在头部添加
    [[nodiscard]] PersistentList cons(T value) const {
        return PersistentList{
            std::make_shared<const Node>(Node{std::move(value), head_}),
            size_ + 1
        };
    }

    [[nodiscard]] const T& head() const {
        if (!head_) throw std::runtime_error("empty list");
        return head_->value;
    }

    [[nodiscard]] PersistentList tail() const {
        if (!head_) return PersistentList{};
        return PersistentList{head_->next, size_ - 1};
    }

    [[nodiscard]] bool empty() const { return head_ == nullptr; }
    [[nodiscard]] size_t size() const { return size_; }

    // 遍历
    template<typename F>
    void for_each(F f) const {
        auto current = head_;
        while (current) {
            f(current->value);
            current = current->next;
        }
    }
};

void persistent_list_demo() {
    std::cout << "=== 持久化链表 ===\n";

    // 构建链表：[3, 2, 1]
    const auto list1 = PersistentList<int>{}.cons(1).cons(2).cons(3);

    auto print_list = [](const auto& list, const char* name) {
        std::cout << name << " = [";
        bool first = true;
        list.for_each([&first](int x) {
            if (!first) std::cout << ", ";
            first = false;
            std::cout << x;
        });
        std::cout << "] (size=" << list.size() << ")\n";
    };

    print_list(list1, "list1");

    // 多版本共存
    const auto list2 = list1.cons(4);   // [4, 3, 2, 1]
    const auto list3 = list1.tail();    // [2, 1]

    print_list(list2, "list2 = list1.cons(4)");
    print_list(list3, "list3 = list1.tail()");

    // list1 没有被修改
    print_list(list1, "list1 (未改变)");

    std::cout << "\n内存共享示意:\n";
    std::cout << "list1: 3 -> 2 -> 1 -> null\n";
    std::cout << "            ^\n";
    std::cout << "list2: 4 ---┘\n";
    std::cout << "\n";
    std::cout << "list3:      2 -> 1 -> null\n";
    std::cout << "\n";
}

// ============================================
// main
// ============================================

int main() {
    const_basics_demo();
    const_reference_demo();
    const_pointer_demo();
    const_member_function_demo();
    constexpr_demo();
    immutable_class_demo();
    immutable_vector_demo();
    mutable_dangers_demo();
    persistent_list_demo();

    return 0;
}
