#include <algorithm>
#include <iostream>
#include <memory>
#include <numeric>
#include <vector>
#include <string>

// ============================================
// 1. 不可变类示例
// ============================================

class ImmutablePoint {
    const int x_;
    const int y_;

public:
    ImmutablePoint(int x, int y) : x_(x), y_(y) {}

    int x() const { return x_; }
    int y() const { return y_; }

    // 返回新对象，不修改当前对象
    ImmutablePoint move(int dx, int dy) const {
        return ImmutablePoint(x_ + dx, y_ + dy);
    }

    ImmutablePoint scale(int factor) const {
        return ImmutablePoint(x_ * factor, y_ * factor);
    }

    void print() const {
        std::cout << "(" << x_ << ", " << y_ << ")";
    }
};

void immutable_class_demo() {
    std::cout << "=== Immutable Class Demo ===\n";

    const auto p1 = ImmutablePoint(3, 4);
    const auto p2 = p1.move(1, 1);
    const auto p3 = p2.scale(2);

    std::cout << "p1: "; p1.print(); std::cout << "\n";
    std::cout << "p2 (p1 moved): "; p2.print(); std::cout << "\n";
    std::cout << "p3 (p2 scaled): "; p3.print(); std::cout << "\n\n";
}

// ============================================
// 2. 不可变 Vector
// ============================================

template<typename T>
class ImmutableVector {
    std::shared_ptr<const std::vector<T>> data_;

public:
    ImmutableVector() : data_(std::make_shared<std::vector<T>>()) {}

    explicit ImmutableVector(std::vector<T> vec)
        : data_(std::make_shared<const std::vector<T>>(std::move(vec))) {}

    // 读取操作
    const T& operator[](size_t i) const { return (*data_)[i]; }
    size_t size() const { return data_->size(); }
    bool empty() const { return data_->empty(); }

    // 修改操作：返回新对象
    ImmutableVector push_back(T value) const {
        auto new_vec = *data_;
        new_vec.push_back(std::move(value));
        return ImmutableVector(std::move(new_vec));
    }

    ImmutableVector update(size_t i, T value) const {
        auto new_vec = *data_;
        new_vec[i] = std::move(value);
        return ImmutableVector(std::move(new_vec));
    }

    template<typename Func>
    auto map(Func f) const {
        using R = decltype(f(std::declval<T>()));
        std::vector<R> result;
        result.reserve(size());
        for (const auto& item : *data_) {
            result.push_back(f(item));
        }
        return ImmutableVector<R>(std::move(result));
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

    void print() const {
        std::cout << "[";
        for (size_t i = 0; i < size(); ++i) {
            std::cout << (*data_)[i];
            if (i < size() - 1) std::cout << ", ";
        }
        std::cout << "]";
    }
};

void immutable_vector_demo() {
    std::cout << "=== Immutable Vector Demo ===\n";

    const auto v1 = ImmutableVector<int>(std::vector{1, 2, 3});
    const auto v2 = v1.push_back(4);
    const auto v3 = v2.update(1, 99);

    std::cout << "v1: "; v1.print(); std::cout << "\n";
    std::cout << "v2 (v1 + 4): "; v2.print(); std::cout << "\n";
    std::cout << "v3 (v2[1] = 99): "; v3.print(); std::cout << "\n";

    // map 和 filter
    const auto v4 = v1.map([](int x) { return x * x; });
    const auto v5 = v2.filter([](int x) { return x % 2 == 0; });

    std::cout << "v4 (v1 squared): "; v4.print(); std::cout << "\n";
    std::cout << "v5 (v2 evens): "; v5.print(); std::cout << "\n\n";
}

// ============================================
// 3. 持久化链表
// ============================================

template<typename T>
class PersistentList {
    struct Node {
        T value;
        std::shared_ptr<Node> next;

        Node(T v, std::shared_ptr<Node> n)
            : value(std::move(v)), next(std::move(n)) {}
    };

    std::shared_ptr<Node> head_;

public:
    PersistentList() = default;

    // O(1) 添加到头部
    PersistentList cons(T value) const {
        PersistentList result;
        result.head_ = std::make_shared<Node>(std::move(value), head_);
        return result;
    }

    const T& head() const { return head_->value; }

    PersistentList tail() const {
        PersistentList result;
        result.head_ = head_->next;
        return result;
    }

    bool empty() const { return head_ == nullptr; }

    void print() const {
        std::cout << "[";
        auto current = head_;
        while (current) {
            std::cout << current->value;
            current = current->next;
            if (current) std::cout << ", ";
        }
        std::cout << "]";
    }
};

void persistent_list_demo() {
    std::cout << "=== Persistent List Demo ===\n";

    const auto list1 = PersistentList<int>().cons(1).cons(2).cons(3);
    const auto list2 = list1.cons(4);
    const auto list3 = list1.tail();

    std::cout << "list1: "; list1.print(); std::cout << "\n";
    std::cout << "list2 (4 :: list1): "; list2.print(); std::cout << "\n";
    std::cout << "list3 (tail of list1): "; list3.print(); std::cout << "\n";
    std::cout << "All lists exist independently!\n\n";
}

// ============================================
// 4. 可变 vs 不可变对比
// ============================================

// 可变版本：修改输入
void mutable_double(std::vector<int>& vec) {
    for (auto& x : vec) {
        x *= 2;
    }
}

// 不可变版本：返回新值
std::vector<int> immutable_double(const std::vector<int>& vec) {
    std::vector<int> result;
    result.reserve(vec.size());
    std::transform(vec.begin(), vec.end(), std::back_inserter(result),
                   [](int x) { return x * 2; });
    return result;
}

void mutable_vs_immutable_demo() {
    std::cout << "=== Mutable vs Immutable Demo ===\n";

    std::vector<int> v1 = {1, 2, 3, 4, 5};
    std::cout << "Original v1: ";
    for (int x : v1) std::cout << x << " ";
    std::cout << "\n";

    mutable_double(v1);
    std::cout << "After mutable_double: ";
    for (int x : v1) std::cout << x << " ";
    std::cout << " (v1 changed!)\n";

    const std::vector<int> v2 = {1, 2, 3, 4, 5};
    const auto v3 = immutable_double(v2);

    std::cout << "Original v2: ";
    for (int x : v2) std::cout << x << " ";
    std::cout << " (unchanged)\n";

    std::cout << "Result v3: ";
    for (int x : v3) std::cout << x << " ";
    std::cout << "\n\n";
}

// ============================================
// 5. const 正确性示例
// ============================================

class Account {
    const std::string name_;
    int balance_;  // 可变

public:
    Account(std::string name, int balance)
        : name_(std::move(name)), balance_(balance) {}

    // const 成员函数：只读
    const std::string& name() const { return name_; }
    int balance() const { return balance_; }

    // 非 const 成员函数：可修改
    void deposit(int amount) { balance_ += amount; }
    void withdraw(int amount) { balance_ -= amount; }
};

// 更好的不可变设计
class ImmutableAccount {
    const std::string name_;
    const int balance_;

public:
    ImmutableAccount(std::string name, int balance)
        : name_(std::move(name)), balance_(balance) {}

    const std::string& name() const { return name_; }
    int balance() const { return balance_; }

    // 返回新账户
    ImmutableAccount deposit(int amount) const {
        return ImmutableAccount(name_, balance_ + amount);
    }

    ImmutableAccount withdraw(int amount) const {
        return ImmutableAccount(name_, balance_ - amount);
    }
};

void const_correctness_demo() {
    std::cout << "=== Const Correctness Demo ===\n";

    // 可变版本
    Account acc1("Alice", 1000);
    std::cout << "Mutable account: " << acc1.balance() << "\n";
    acc1.deposit(200);
    std::cout << "After deposit: " << acc1.balance() << "\n";

    // 不可变版本
    const auto acc2 = ImmutableAccount("Bob", 1000);
    const auto acc3 = acc2.deposit(200);
    const auto acc4 = acc3.withdraw(100);

    std::cout << "Immutable account acc2: " << acc2.balance() << "\n";
    std::cout << "After deposit acc3: " << acc3.balance() << "\n";
    std::cout << "After withdraw acc4: " << acc4.balance() << "\n";
    std::cout << "All versions exist independently!\n\n";
}

int main() {
    immutable_class_demo();
    immutable_vector_demo();
    persistent_list_demo();
    mutable_vs_immutable_demo();
    const_correctness_demo();

    return 0;
}
