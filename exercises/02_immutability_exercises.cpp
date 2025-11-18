#include <algorithm>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include <chrono>

// ============================================
// 练习 1: 不可变 Rectangle
// ============================================

class ImmutableRectangle {
    // TODO: 添加私有成员 width_ 和 height_
public:
    ImmutableRectangle(int width, int height) {
        // TODO: 初始化成员
    }

    int width() const {
        // TODO: 返回 width
        return 0;
    }

    int height() const {
        // TODO: 返回 height
        return 0;
    }

    int area() const {
        // TODO: 返回面积
        return 0;
    }

    // TODO: 实现 resize，返回新的 Rectangle
    ImmutableRectangle resize(int new_width, int new_height) const {
        return ImmutableRectangle(0, 0);
    }

    // TODO: 实现 scale，按比例缩放
    ImmutableRectangle scale(double factor) const {
        return ImmutableRectangle(0, 0);
    }
};

// ============================================
// 练习 2: ImmutableVector::reduce
// ============================================

template<typename T>
class ImmutableVector {
    std::shared_ptr<const std::vector<T>> data_;

public:
    ImmutableVector() : data_(std::make_shared<std::vector<T>>()) {}

    explicit ImmutableVector(std::vector<T> vec)
        : data_(std::make_shared<const std::vector<T>>(std::move(vec))) {}

    const T& operator[](size_t i) const { return (*data_)[i]; }
    size_t size() const { return data_->size(); }

    // TODO: 实现 reduce
    template<typename Acc, typename Func>
    Acc reduce(Acc init, Func f) const {
        // 提示：遍历所有元素，累积结果
        return init;
    }

    template<typename Pred>
    ImmutableVector filter(Pred pred) const {
        std::vector<T> result;
        for (const auto& item : *data_) {
            if (pred(item)) {
                result.push_back(item);
            }
        }
        return ImmutableVector(std::move(result));
    }
};

// ============================================
// 练习 3: 性能对比
// ============================================

// TODO: 实现可变版本的 process_mutable
std::vector<int> process_mutable(std::vector<int> vec) {
    // 过滤偶数，然后平方，修改原数组
    return vec;
}

// TODO: 实现不可变版本的 process_immutable
std::vector<int> process_immutable(const std::vector<int>& vec) {
    // 过滤偶数，然后平方，返回新数组
    return {};
}

void performance_comparison() {
    std::cout << "=== Performance Comparison ===\n";

    std::vector<int> data(10000);
    for (size_t i = 0; i < data.size(); ++i) {
        data[i] = static_cast<int>(i);
    }

    // 测试可变版本
    auto start = std::chrono::high_resolution_clock::now();
    auto result1 = process_mutable(data);
    auto end = std::chrono::high_resolution_clock::now();
    auto mutable_time = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

    // 测试不可变版本
    start = std::chrono::high_resolution_clock::now();
    auto result2 = process_immutable(data);
    end = std::chrono::high_resolution_clock::now();
    auto immutable_time = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

    std::cout << "Mutable version: " << mutable_time.count() << " μs\n";
    std::cout << "Immutable version: " << immutable_time.count() << " μs\n\n";
}

// ============================================
// 练习 4: 简单不可变 Map
// ============================================

template<typename K, typename V>
class ImmutableMap {
    // TODO: 使用 shared_ptr 包装 std::map
    std::shared_ptr<const std::map<K, V>> data_;

public:
    ImmutableMap() {
        // TODO: 初始化空 map
    }

    // TODO: 实现 insert，返回新 map
    ImmutableMap insert(K key, V value) const {
        return ImmutableMap();
    }

    // TODO: 实现 get，返回 optional 或指针
    const V* get(const K& key) const {
        return nullptr;
    }

    // TODO: 实现 remove，返回新 map
    ImmutableMap remove(const K& key) const {
        return ImmutableMap();
    }

    size_t size() const {
        // TODO: 返回大小
        return 0;
    }
};

// ============================================
// 测试代码
// ============================================

void test_exercise_1() {
    std::cout << "=== Exercise 1: Immutable Rectangle ===\n";

    const auto r1 = ImmutableRectangle(10, 20);
    const auto r2 = r1.resize(15, 25);
    const auto r3 = r1.scale(2.0);

    std::cout << "r1: " << r1.width() << "x" << r1.height()
              << " (area: " << r1.area() << ")\n";
    std::cout << "r2 (resized): " << r2.width() << "x" << r2.height()
              << " (area: " << r2.area() << ")\n";
    std::cout << "r3 (scaled 2x): " << r3.width() << "x" << r3.height()
              << " (area: " << r3.area() << ")\n";
    std::cout << "Expected r1: 10x20 (area: 200)\n";
    std::cout << "Expected r2: 15x25 (area: 375)\n";
    std::cout << "Expected r3: 20x40 (area: 800)\n\n";
}

void test_exercise_2() {
    std::cout << "=== Exercise 2: ImmutableVector::reduce ===\n";

    const auto vec = ImmutableVector<int>(std::vector{1, 2, 3, 4, 5});

    auto sum = vec.reduce(0, [](int acc, int x) { return acc + x; });
    auto product = vec.reduce(1, [](int acc, int x) { return acc * x; });

    std::cout << "Sum: " << sum << " (expected: 15)\n";
    std::cout << "Product: " << product << " (expected: 120)\n\n";
}

void test_exercise_4() {
    std::cout << "=== Exercise 4: Immutable Map ===\n";

    const auto m1 = ImmutableMap<std::string, int>();
    const auto m2 = m1.insert("alice", 100);
    const auto m3 = m2.insert("bob", 200);
    const auto m4 = m3.insert("charlie", 300);
    const auto m5 = m4.remove("bob");

    std::cout << "m1 size: " << m1.size() << " (expected: 0)\n";
    std::cout << "m4 size: " << m4.size() << " (expected: 3)\n";
    std::cout << "m5 size: " << m5.size() << " (expected: 2)\n";

    if (auto val = m4.get("alice")) {
        std::cout << "m4['alice'] = " << *val << " (expected: 100)\n";
    }
    if (auto val = m5.get("bob")) {
        std::cout << "m5['bob'] found (ERROR: should be removed!)\n";
    } else {
        std::cout << "m5['bob'] not found (correct)\n";
    }
    std::cout << "\n";
}

int main() {
    std::cout << "C++ Functional Programming - Chapter 02 Exercises\n";
    std::cout << "=================================================\n\n";

    test_exercise_1();
    test_exercise_2();
    performance_comparison();
    test_exercise_4();

    std::cout << "Hint: Check solutions in exercises/solutions/02_immutability_solutions.cpp\n";

    return 0;
}
