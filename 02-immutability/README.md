# 02 - 不可变性

## 为什么需要不可变性

**可变状态是 bug 的温床。**

```cpp
// 可变状态：难以追踪，容易出错
int balance = 1000;
withdraw(200);   // balance = 800
deposit(100);    // balance = 900
withdraw(300);   // balance = 600
// 现在 balance 是多少？得追溯所有操作

// 不可变：每次操作返回新值
const auto b1 = make_account(1000);
const auto b2 = withdraw(b1, 200);  // b1 不变，b2 = 800
const auto b3 = deposit(b2, 100);   // b2 不变，b3 = 900
// 状态清晰，每个值都能追溯
```

不可变性的好处：
1. **无副作用** - 函数不改变输入，输出可预测
2. **线程安全** - 不可变数据天然线程安全，无需锁
3. **易于测试** - 相同输入永远得到相同输出
4. **时间旅行** - 保留所有历史状态，可回溯
5. **引用透明** - 可以安全地共享数据

## const 的正确使用

`const` 是 C++ 中实现不可变性的基础工具。

### 变量 const

```cpp
// 基本用法
const int x = 42;
// x = 43;  // 编译错误

// const 引用：不拷贝，不修改
void print(const std::string& s) {
    std::cout << s;  // OK
    // s += "!";     // 编译错误
}

// const 指针：两种含义
const int* p1 = &x;      // 指向 const int（数据不可变）
int* const p2 = &y;      // const 指针（指针不可变）
const int* const p3 = &x; // 都不可变
```

### 成员函数 const

```cpp
class Point {
    int x_, y_;
public:
    Point(int x, int y) : x_(x), y_(y) {}

    // const 成员函数：承诺不修改对象
    int x() const { return x_; }
    int y() const { return y_; }

    // 非 const 成员函数：可能修改对象
    void set_x(int x) { x_ = x; }

    // const 版本和非 const 版本可以重载
    int& operator[](size_t i) { return i == 0 ? x_ : y_; }
    const int& operator[](size_t i) const { return i == 0 ? x_ : y_; }
};

const Point p(3, 4);
p.x();      // OK
// p.set_x(5);  // 编译错误：const 对象不能调用非 const 成员
```

### constexpr：编译期常量

```cpp
// constexpr 函数：编译期求值
constexpr int square(int x) {
    return x * x;
}

constexpr int s = square(5);  // 编译期计算，s = 25

// constexpr 对象
constexpr Point origin{0, 0};

// C++17: constexpr if
template<typename T>
auto process(T value) {
    if constexpr (std::is_integral_v<T>) {
        return value * 2;
    } else {
        return value;
    }
}
```

## 不可变数据结构

### 简单不可变类

```cpp
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
};

const auto p1 = ImmutablePoint(3, 4);
const auto p2 = p1.move(1, 1);  // p1 不变，p2 = (4, 5)
const auto p3 = p2.scale(2);     // p2 不变，p3 = (8, 10)
```

### 不可变容器包装

```cpp
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

    // 修改操作：返回新对象
    ImmutableVector push_back(T value) const {
        auto new_vec = *data_;  // 拷贝
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
};
```

## 可变状态的危害

### 示例 1：多线程竞态条件

```cpp
// 危险：可变共享状态
class BadCounter {
    int count_ = 0;
public:
    void increment() { ++count_; }  // 竞态条件！
    int get() const { return count_; }
};

// 安全：不可变
class GoodCounter {
    const int count_;
public:
    explicit GoodCounter(int count = 0) : count_(count) {}
    GoodCounter increment() const { return GoodCounter(count_ + 1); }
    int get() const { return count_; }
};
```

### 示例 2：意外修改

```cpp
// 危险：返回可变引用
class BadContainer {
    std::vector<int> data_;
public:
    std::vector<int>& get_data() { return data_; }  // 外部可修改！
};

BadContainer c;
c.get_data().clear();  // 糟糕，内部数据被清空

// 安全：返回 const 引用或拷贝
class GoodContainer {
    std::vector<int> data_;
public:
    const std::vector<int>& get_data() const { return data_; }
};
```

### 示例 3：函数副作用

```cpp
// 坏：修改输入参数
void bad_normalize(std::vector<int>& vec) {
    int sum = std::accumulate(vec.begin(), vec.end(), 0);
    for (auto& x : vec) x /= sum;  // 修改了输入！
}

// 好：返回新值
std::vector<int> good_normalize(const std::vector<int>& vec) {
    int sum = std::accumulate(vec.begin(), vec.end(), 0);
    std::vector<int> result;
    result.reserve(vec.size());
    std::transform(vec.begin(), vec.end(), std::back_inserter(result),
                   [sum](int x) { return x / sum; });
    return result;
}
```

## 持久化数据结构

持久化数据结构：修改操作返回新版本，同时保留旧版本，且高效共享内存。

### 简单链表实现

```cpp
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

    // 获取头部
    const T& head() const { return head_->value; }

    // 获取尾部
    PersistentList tail() const {
        PersistentList result;
        result.head_ = head_->next;
        return result;
    }

    bool empty() const { return head_ == nullptr; }
};

// 使用示例：多版本共存
const auto list1 = PersistentList<int>().cons(1).cons(2).cons(3);  // [3,2,1]
const auto list2 = list1.cons(4);  // [4,3,2,1]，共享 [3,2,1]
const auto list3 = list1.tail();   // [2,1]，共享 [2,1]
// list1, list2, list3 同时存在，互不影响
```

## 实践原则

1. **默认 const** - 所有变量默认用 `const`，除非必须可变
2. **const 引用传参** - 避免拷贝，保证不修改
3. **const 成员函数** - 不修改状态的函数都标记 `const`
4. **返回新值** - 修改操作返回新对象，不改原对象
5. **共享内存** - 用 `shared_ptr` 高效共享不可变数据

## 代码示例

完整示例见：`examples/basic/02_immutability.cpp`

## 练习

1. 实现不可变的 `Rectangle` 类，包含 `resize()` 方法返回新矩形
2. 实现 `ImmutableVector::filter()` 方法
3. 对比可变和不可变实现的性能差异
4. 实现简单的不可变 Map（持久化哈希表基础版）

练习题在：`exercises/02_immutability_exercises.cpp`
