#include <algorithm>
#include <chrono>
#include <iostream>
#include <map>
#include <memory>
#include <numeric>
#include <string>
#include <vector>

// ============================================
// 练习 1: 不可变 Rectangle
// ============================================

class ImmutableRectangle {
    const int width_;
    const int height_;

public:
    ImmutableRectangle(int width, int height)
        : width_(width), height_(height) {}

    int width() const { return width_; }
    int height() const { return height_; }
    int area() const { return width_ * height_; }

    ImmutableRectangle resize(int new_width, int new_height) const {
        return ImmutableRectangle(new_width, new_height);
    }

    ImmutableRectangle scale(double factor) const {
        return ImmutableRectangle(
            static_cast<int>(width_ * factor),
            static_cast<int>(height_ * factor)
        );
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

    template<typename Acc, typename Func>
    Acc reduce(Acc init, Func f) const {
        Acc result = init;
        for (const auto& item : *data_) {
            result = f(result, item);
        }
        return result;
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

std::vector<int> process_mutable(std::vector<int> vec) {
    // 原地过滤
    vec.erase(
        std::remove_if(vec.begin(), vec.end(), [](int x) { return x % 2 != 0; }),
        vec.end()
    );

    // 原地平方
    for (auto& x : vec) {
        x = x * x;
    }

    return vec;
}

std::vector<int> process_immutable(const std::vector<int>& vec) {
    // 创建新的过滤结果
    std::vector<int> filtered;
    std::copy_if(vec.begin(), vec.end(), std::back_inserter(filtered),
                 [](int x) { return x % 2 == 0; });

    // 创建新的转换结果
    std::vector<int> result;
    result.reserve(filtered.size());
    std::transform(filtered.begin(), filtered.end(), std::back_inserter(result),
                   [](int x) { return x * x; });

    return result;
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
    std::cout << "Immutable version: " << immutable_time.count() << " μs\n";
    std::cout << "Results size: " << result1.size() << " vs " << result2.size() << "\n\n";
}

// ============================================
// 练习 4: 简单不可变 Map
// ============================================

template<typename K, typename V>
class ImmutableMap {
    std::shared_ptr<const std::map<K, V>> data_;

public:
    ImmutableMap() : data_(std::make_shared<std::map<K, V>>()) {}

    explicit ImmutableMap(std::map<K, V> m)
        : data_(std::make_shared<const std::map<K, V>>(std::move(m))) {}

    ImmutableMap insert(K key, V value) const {
        auto new_map = *data_;
        new_map[std::move(key)] = std::move(value);
        return ImmutableMap(std::move(new_map));
    }

    const V* get(const K& key) const {
        auto it = data_->find(key);
        if (it != data_->end()) {
            return &(it->second);
        }
        return nullptr;
    }

    ImmutableMap remove(const K& key) const {
        auto new_map = *data_;
        new_map.erase(key);
        return ImmutableMap(std::move(new_map));
    }

    size_t size() const { return data_->size(); }
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
    std::cout << "C++ Functional Programming - Chapter 02 Solutions\n";
    std::cout << "=================================================\n\n";

    test_exercise_1();
    test_exercise_2();
    performance_comparison();
    test_exercise_4();

    return 0;
}
