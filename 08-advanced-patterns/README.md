# 第八章：高级函数式模式

## 说人话：高级模式是什么？

前面学了基础工具（纯函数、组合、Monad），现在学习如何组合这些工具解决复杂问题。

**本章内容**：
1. **Lazy Evaluation（惰性求值）**：需要时才计算
2. **Memoization（记忆化）**：缓存计算结果
3. **Tail Recursion（尾递归）**：优化递归性能
4. **Trampolines（蹦床）**：避免栈溢出
5. **Continuation（续延）**：控制程序流程
6. **Free Monad（自由单子）**：构建 DSL
7. **Lens（透镜）**：优雅访问嵌套数据
8. **实际应用**：组合这些模式解决真实问题

## 1. Lazy Evaluation（惰性求值）

### 什么是惰性求值？

**普通求值（eager）**：立即计算
**惰性求值（lazy）**：延迟到真正需要时才计算

### 为什么需要？

1. **避免不必要的计算**：可能永远不会用到
2. **处理无限序列**：按需生成
3. **提高性能**：只计算需要的部分

### C++ 实现

```cpp
template<typename T>
class Lazy {
    mutable std::optional<T> cached;
    std::function<T()> thunk;

public:
    explicit Lazy(std::function<T()> f) : thunk(f) {}

    const T& force() const {
        if (!cached) {
            cached = thunk();
        }
        return *cached;
    }
};

// 使用
Lazy<int> expensive_calc([]() {
    std::cout << "Computing...\n";
    return 42 * 42;
});

// 此时还没计算
std::cout << "Before force\n";

// 现在才计算
int result = expensive_calc.force();  // 输出 "Computing..."

// 第二次直接用缓存
int result2 = expensive_calc.force();  // 不输出
```

### 应用：无限序列

```cpp
class InfiniteRange {
    int current;

public:
    InfiniteRange(int start = 0) : current(start) {}

    int next() { return current++; }

    // 惰性 map
    template<typename F>
    auto map(F f) {
        return [this, f]() { return f(next()); };
    }

    // 惰性 filter
    template<typename P>
    int next_if(P predicate) {
        while (true) {
            int val = next();
            if (predicate(val)) return val;
        }
    }
};

// 使用：无限的偶数序列
InfiniteRange range;
for (int i = 0; i < 10; ++i) {
    int even = range.next_if([](int x) { return x % 2 == 0; });
    std::cout << even << " ";
}
// 输出：0 2 4 6 8 10 12 14 16 18
```

## 2. Memoization（记忆化）

### 什么是记忆化？

缓存函数的计算结果，相同输入直接返回缓存。

### 实现

```cpp
template<typename R, typename... Args>
auto memoize(std::function<R(Args...)> func) {
    std::map<std::tuple<Args...>, R> cache;

    return [func, cache](Args... args) mutable -> R {
        auto key = std::make_tuple(args...);
        auto it = cache.find(key);
        if (it != cache.end()) {
            return it->second;
        }
        R result = func(args...);
        cache[key] = result;
        return result;
    };
}

// 使用：斐波那契数列
std::function<int(int)> fib = [&fib](int n) -> int {
    if (n <= 1) return n;
    return fib(n - 1) + fib(n - 2);
};

auto fib_memo = memoize(fib);

// O(n) 而不是 O(2^n)
int result = fib_memo(40);
```

## 3. Tail Recursion（尾递归）

### 什么是尾递归？

递归调用是函数的最后一个操作。编译器可以优化为循环，避免栈溢出。

### 非尾递归 vs 尾递归

```cpp
// 非尾递归：递归后还有操作（加法）
int factorial(int n) {
    if (n <= 1) return 1;
    return n * factorial(n - 1);  // 递归后还要乘 n
}

// 尾递归：递归是最后的操作
int factorial_tail(int n, int acc = 1) {
    if (n <= 1) return acc;
    return factorial_tail(n - 1, n * acc);  // 递归是最后操作
}
```

### 累加器技巧

将中间结果作为参数传递，避免递归后的操作。

```cpp
// 非尾递归求和
int sum(const std::vector<int>& vec, int index = 0) {
    if (index >= vec.size()) return 0;
    return vec[index] + sum(vec, index + 1);
}

// 尾递归求和
int sum_tail(const std::vector<int>& vec, int index = 0, int acc = 0) {
    if (index >= vec.size()) return acc;
    return sum_tail(vec, index + 1, acc + vec[index]);
}
```

## 4. Trampolines（蹦床）

### 什么是蹦床？

手动实现尾递归优化，将递归转换为循环。

### 为什么需要？

C++ 编译器不保证尾递归优化，大数据时可能栈溢出。

### 实现

```cpp
template<typename T>
struct Bounce {
    std::variant<T, std::function<Bounce<T>()>> data;

    bool is_done() const { return data.index() == 0; }
    T get_value() const { return std::get<0>(data); }
    Bounce<T> next() const { return std::get<1>(data)(); }

    static Bounce done(T value) {
        return Bounce{std::variant<T, std::function<Bounce<T>()>>(
            std::in_place_index<0>, value
        )};
    }

    static Bounce more(std::function<Bounce<T>()> f) {
        return Bounce{std::variant<T, std::function<Bounce<T>()>>(
            std::in_place_index<1>, f
        )};
    }
};

template<typename T>
T trampoline(Bounce<T> bounce) {
    while (!bounce.is_done()) {
        bounce = bounce.next();
    }
    return bounce.get_value();
}

// 使用：不会栈溢出的递归
Bounce<int> factorial_bounce(int n, int acc = 1) {
    if (n <= 1) {
        return Bounce<int>::done(acc);
    }
    return Bounce<int>::more([=]() {
        return factorial_bounce(n - 1, n * acc);
    });
}

int result = trampoline(factorial_bounce(100000));
```

## 5. Continuation（续延）

### 什么是续延？

将"接下来要做什么"作为参数传递。

### Continuation Passing Style (CPS)

```cpp
// 普通风格
int add(int a, int b) {
    return a + b;
}

// CPS 风格
void add_cps(int a, int b, std::function<void(int)> cont) {
    cont(a + b);
}

// 使用
add_cps(3, 4, [](int result) {
    std::cout << "Result: " << result << "\n";
});
```

### 为什么有用？

1. **控制流程**：显式控制下一步做什么
2. **异步编程**：回调就是 continuation
3. **异常处理**：可以传递多个 continuation（成功、失败）

## 6. Lens（透镜）

### 什么是 Lens？

函数式方式访问和修改嵌套数据结构。

### 问题

```cpp
struct Address {
    std::string street;
    std::string city;
};

struct Person {
    std::string name;
    Address address;
};

// 修改嵌套数据很麻烦
Person update_city(const Person& p, const std::string& new_city) {
    Person new_p = p;
    new_p.address.city = new_city;
    return new_p;
}
```

### Lens 解决方案

```cpp
template<typename S, typename A>
struct Lens {
    std::function<A(const S&)> getter;
    std::function<S(const S&, const A&)> setter;

    A get(const S& s) const { return getter(s); }

    S set(const S& s, const A& a) const { return setter(s, a); }

    S modify(const S& s, std::function<A(const A&)> f) const {
        return set(s, f(get(s)));
    }

    // 组合 Lens
    template<typename B>
    Lens<S, B> compose(const Lens<A, B>& other) const {
        return Lens<S, B>{
            [this, other](const S& s) { return other.get(this->get(s)); },
            [this, other](const S& s, const B& b) {
                return this->set(s, other.set(this->get(s), b));
            }
        };
    }
};

// 定义 Lens
Lens<Person, Address> address_lens{
    [](const Person& p) { return p.address; },
    [](const Person& p, const Address& a) {
        Person new_p = p;
        new_p.address = a;
        return new_p;
    }
};

Lens<Address, std::string> city_lens{
    [](const Address& a) { return a.city; },
    [](const Address& a, const std::string& c) {
        Address new_a = a;
        new_a.city = c;
        return new_a;
    }
};

// 组合使用
auto person_city_lens = address_lens.compose(city_lens);
Person updated = person_city_lens.set(person, "New York");
```

## 7. Y Combinator（Y 组合子）

### 什么是 Y Combinator？

在没有命名递归的情况下实现递归。

### 实现

```cpp
template<typename F>
struct Y {
    F f;

    template<typename... Args>
    auto operator()(Args&&... args) const {
        return f(*this, std::forward<Args>(args)...);
    }
};

template<typename F>
Y<std::decay_t<F>> make_y(F&& f) {
    return {std::forward<F>(f)};
}

// 使用：匿名递归
auto factorial = make_y([](auto self, int n) -> int {
    if (n <= 1) return 1;
    return n * self(n - 1);
});

int result = factorial(5);  // 120
```

## 8. 模式对比

| 模式 | 用途 | 优点 | 缺点 |
|------|------|------|------|
| Lazy Evaluation | 延迟计算 | 性能优化，处理无限序列 | 调试困难 |
| Memoization | 缓存结果 | 避免重复计算 | 内存开销 |
| Tail Recursion | 优化递归 | 避免栈溢出 | 需要改写算法 |
| Trampoline | 手动优化递归 | 确保不栈溢出 | 代码复杂 |
| Continuation | 控制流程 | 灵活的异步处理 | 难以理解 |
| Lens | 访问嵌套数据 | 不可变更新优雅 | 模板代码多 |
| Y Combinator | 匿名递归 | 理论优雅 | 实际用处有限 |

## 9. 何时使用这些模式？

### Lazy Evaluation
- 处理大数据集
- 可能不需要全部数据
- 构建无限序列

### Memoization
- 纯函数且计算昂贵
- 相同输入会重复调用
- 输入空间不是无限大

### Tail Recursion / Trampoline
- 递归深度很大
- 担心栈溢出
- 可以改写成尾递归形式

### Continuation
- 异步编程
- 需要精确控制流程
- 实现协程或生成器

### Lens
- 频繁更新嵌套不可变数据
- 需要组合多个访问路径
- 数据结构层次深

## 10. 最佳实践

1. **不要过度使用**：这些模式很强大，但不是银弹
2. **先简单后复杂**：先用简单方法，遇到问题再优化
3. **性能测试**：抽象有代价，确保值得
4. **文档注释**：这些模式不直观，需要解释
5. **团队共识**：确保团队理解这些模式

## 11. 性能考虑

### Lazy Evaluation
- **优势**：避免不必要的计算
- **劣势**：thunk 的函数调用开销

### Memoization
- **优势**：O(1) 查找缓存
- **劣势**：内存占用，可能需要 LRU 策略

### Trampoline
- **优势**：避免栈溢出
- **劣势**：堆分配开销，比真正的尾递归优化慢

## 12. 调试技巧

1. **Lazy Evaluation**：添加日志记录计算时机
2. **Memoization**：记录缓存命中率
3. **Trampoline**：限制最大跳转次数，防止死循环
4. **Continuation**：打印 continuation 链

## 总结

高级函数式模式是工具箱中的高级工具：

- **Lazy Evaluation**：延迟到需要时
- **Memoization**：记住计算过的
- **Tail Recursion**：递归转循环
- **Trampoline**：手动优化递归
- **Continuation**：控制下一步
- **Lens**：优雅访问嵌套
- **Y Combinator**：匿名递归

这些模式不是必须的，但在合适的场景下能显著提升代码质量。关键是理解每个模式解决什么问题，而不是盲目使用。

## 练习

参见：`exercises/08_advanced_patterns_exercises.cpp`
