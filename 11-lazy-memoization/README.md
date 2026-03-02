# 11 - 惰性求值与记忆化

> 只在需要时计算，计算过的不再重复。

## 本章目标

学完本章，你将能够：
- 理解惰性求值的概念和优势
- 实现 Lazy<T> 类型
- 掌握记忆化技术
- 避免不必要的计算
- 处理无限数据结构

## 什么是惰性求值

**惰性求值 (Lazy Evaluation)** 是一种计算策略：
- **不立即计算**：表达式在定义时不求值
- **需要时计算**：只在真正使用值时才计算
- **最多计算一次**：计算结果被缓存

### 对比：严格求值 vs 惰性求值

```cpp
// 严格求值：立即计算
int expensive() {
    std::cout << "Computing...\n";
    return 42;
}

int x = expensive();  // 立即输出 "Computing..."
// 即使 x 从未使用，也已经计算了

// 惰性求值：延迟计算
Lazy<int> y = lazy([] { return expensive(); });
// 此时没有输出

int value = y.get();  // 现在才输出 "Computing..."
int again = y.get();  // 不再输出，直接返回缓存值
```

### 优势

1. **避免不必要的计算**：只计算真正需要的值
2. **处理无限结构**：可以定义无限列表、无限树
3. **短路求值**：条件不满足时跳过计算
4. **提高模块化**：生成和消费可以分离

## Lazy<T> 实现

### 基本版本

```cpp
template<typename T>
class Lazy {
    std::function<T()> thunk_;          // 计算函数
    mutable std::optional<T> cached_;   // 缓存结果
    mutable bool evaluated_ = false;    // 是否已求值

public:
    explicit Lazy(std::function<T()> thunk)
        : thunk_(std::move(thunk)) {}

    [[nodiscard]] const T& get() const {
        if (!evaluated_) {
            cached_ = thunk_();
            evaluated_ = true;
        }
        return *cached_;
    }
};

// 辅助函数
template<typename F>
auto lazy(F f) {
    using T = std::invoke_result_t<F>;
    return Lazy<T>(std::move(f));
}
```

### 使用示例

```cpp
auto expensive = lazy([] {
    std::cout << "Heavy computation...\n";
    std::this_thread::sleep_for(std::chrono::seconds(1));
    return 42;
});

std::cout << "Lazy value created\n";  // 立即输出

if (some_condition) {
    int value = expensive.get();       // 只在需要时计算
    std::cout << "Value: " << value << "\n";
}
// 如果 some_condition 为 false，从不计算
```

### 线程安全版本

```cpp
template<typename T>
class ThreadSafeLazy {
    std::function<T()> thunk_;
    mutable std::optional<T> cached_;
    mutable std::once_flag flag_;

public:
    explicit ThreadSafeLazy(std::function<T()> thunk)
        : thunk_(std::move(thunk)) {}

    [[nodiscard]] const T& get() const {
        std::call_once(flag_, [this] {
            cached_ = thunk_();
        });
        return *cached_;
    }
};
```

## Lazy 的 Functor 和 Monad

### fmap：延迟转换

```cpp
template<typename T>
class Lazy {
    // ...

    template<typename F>
    [[nodiscard]] auto fmap(F f) const {
        return lazy([self = *this, f] {
            return f(self.get());
        });
    }
};

// 使用：转换也是惰性的
auto doubled = expensive.fmap([](int x) { return x * 2; });
// 没有任何计算发生

int result = doubled.get();  // 现在计算 expensive 和乘2
```

### bind：链式惰性操作

```cpp
template<typename T>
class Lazy {
    template<typename F>
    [[nodiscard]] auto bind(F f) const {
        return lazy([self = *this, f] {
            return f(self.get()).get();
        });
    }
};

// 使用
auto result = expensive
    .bind([](int x) { return lazy([x] { return x * 2; }); })
    .bind([](int x) { return lazy([x] { return std::to_string(x); }); });
```

## 记忆化 (Memoization)

记忆化是缓存函数调用结果的技术，避免重复计算。

### 基本实现

```cpp
template<typename F>
auto memoize(F f) {
    using ArgTuple = typename function_traits<F>::args_tuple;
    using ReturnType = typename function_traits<F>::return_type;

    auto cache = std::make_shared<std::unordered_map<ArgTuple, ReturnType>>();

    return [f, cache](auto... args) {
        auto key = std::make_tuple(args...);
        auto it = cache->find(key);
        if (it != cache->end()) {
            return it->second;
        }
        auto result = f(args...);
        (*cache)[key] = result;
        return result;
    };
}
```

### 简化版（单参数）

```cpp
template<typename Arg, typename Result>
auto memoize(std::function<Result(Arg)> f) {
    auto cache = std::make_shared<std::unordered_map<Arg, Result>>();

    return [f, cache](Arg arg) -> Result {
        auto it = cache->find(arg);
        if (it != cache->end()) {
            return it->second;
        }
        Result result = f(arg);
        (*cache)[arg] = result;
        return result;
    };
}
```

### 使用示例：斐波那契

```cpp
// 朴素递归：指数级时间复杂度
int fib_naive(int n) {
    if (n <= 1) return n;
    return fib_naive(n - 1) + fib_naive(n - 2);
}

// 记忆化版本：线性时间复杂度
std::function<int(int)> fib_memo;
fib_memo = memoize<int, int>([&](int n) -> int {
    if (n <= 1) return n;
    return fib_memo(n - 1) + fib_memo(n - 2);
});

// 或者使用显式缓存
int fib_cached(int n) {
    static std::vector<int> cache = {0, 1};
    if (n < static_cast<int>(cache.size())) {
        return cache[n];
    }
    while (static_cast<int>(cache.size()) <= n) {
        cache.push_back(cache[cache.size() - 1] + cache[cache.size() - 2]);
    }
    return cache[n];
}
```

## 惰性列表

### 无限序列

```cpp
template<typename T>
class LazyList {
public:
    using Generator = std::function<std::pair<T, LazyList>()>;

private:
    Lazy<std::optional<std::pair<T, LazyList>>> cell_;

public:
    // 空列表
    static LazyList empty() {
        return LazyList(lazy([] { return std::nullopt; }));
    }

    // 构造
    static LazyList cons(T head, LazyList tail) {
        return LazyList(lazy([head, tail] {
            return std::make_optional(std::make_pair(head, tail));
        }));
    }

    // 生成无限列表
    static LazyList iterate(T start, std::function<T(T)> next) {
        return cons(start, lazy([=] {
            return iterate(next(start), next);
        }).get());
    }

    // 获取前 n 个元素
    std::vector<T> take(int n) const {
        std::vector<T> result;
        LazyList current = *this;
        for (int i = 0; i < n; ++i) {
            auto cell = current.cell_.get();
            if (!cell) break;
            result.push_back(cell->first);
            current = cell->second;
        }
        return result;
    }
};

// 使用
auto naturals = LazyList<int>::iterate(0, [](int n) { return n + 1; });
auto first10 = naturals.take(10);  // {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}
```

### 惰性过滤

```cpp
template<typename T>
LazyList<T> filter(std::function<bool(T)> pred, LazyList<T> list) {
    return lazy([=] {
        auto cell = list.cell_.get();
        if (!cell) return LazyList<T>::empty();

        if (pred(cell->first)) {
            return LazyList<T>::cons(cell->first, filter(pred, cell->second));
        }
        return filter(pred, cell->second);
    }).get();
}

// 使用：无限素数列表
auto primes = /* 埃拉托斯特尼筛法的惰性版本 */;
auto first20Primes = primes.take(20);
```

## 实际应用

### 1. 配置延迟加载

```cpp
class Config {
    Lazy<std::string> database_url_;
    Lazy<int> port_;

public:
    Config()
        : database_url_(lazy([] {
              return read_env("DATABASE_URL").value_or("localhost:5432");
          }))
        , port_(lazy([] {
              auto s = read_env("PORT");
              return s ? std::stoi(*s) : 8080;
          })) {}

    [[nodiscard]] const std::string& database_url() const {
        return database_url_.get();
    }

    [[nodiscard]] int port() const {
        return port_.get();
    }
};
```

### 2. 昂贵资源初始化

```cpp
class DatabaseConnection {
    Lazy<Connection> conn_;

public:
    DatabaseConnection(const std::string& url)
        : conn_(lazy([url] {
              std::cout << "Connecting to database...\n";
              return Connection::create(url);
          })) {}

    void query(const std::string& sql) {
        conn_.get().execute(sql);  // 第一次访问时才连接
    }
};
```

### 3. 缓存计算密集型结果

```cpp
class Report {
    Lazy<Statistics> stats_;
    Lazy<std::vector<Chart>> charts_;

public:
    Report(const Data& data)
        : stats_(lazy([&data] {
              return compute_statistics(data);  // 昂贵计算
          }))
        , charts_(lazy([this] {
              return generate_charts(stats_.get());  // 依赖 stats
          })) {}

    void print_summary() {
        // 只计算统计，不生成图表
        std::cout << stats_.get().summary() << "\n";
    }

    void full_report() {
        print_summary();
        for (const auto& chart : charts_.get()) {  // 现在计算图表
            chart.render();
        }
    }
};
```

### 4. 短路求值优化

```cpp
template<typename T>
class LazyAnd {
    std::vector<Lazy<bool>> conditions_;

public:
    void add(std::function<bool()> cond) {
        conditions_.push_back(lazy(std::move(cond)));
    }

    [[nodiscard]] bool evaluate() const {
        for (const auto& cond : conditions_) {
            if (!cond.get()) return false;  // 短路
        }
        return true;
    }
};

// 使用
LazyAnd validator;
validator.add([] { return check_basic(); });      // 快速检查
validator.add([] { return check_database(); });   // 数据库检查
validator.add([] { return check_external(); });   // 外部服务检查

if (!validator.evaluate()) {
    // 第一个失败的检查后面的都不会执行
}
```

## 注意事项

### mutable 的使用

`Lazy<T>` 通常需要 `mutable` 来缓存结果：

```cpp
template<typename T>
class Lazy {
    mutable std::optional<T> cached_;  // mutable 允许在 const 方法中修改
    mutable bool evaluated_ = false;

    [[nodiscard]] const T& get() const {  // const 方法
        if (!evaluated_) {
            cached_ = thunk_();           // 修改 mutable 成员
            evaluated_ = true;
        }
        return *cached_;
    }
};
```

**为什么这是可接受的？**
- 从调用者角度看，`get()` 是纯函数（相同输入→相同输出）
- 缓存是优化细节，不改变语义
- 这被称为"透明引用"（referential transparency）

### 避免捕获引用

```cpp
// 危险：捕获局部变量引用
int local = 42;
auto bad = lazy([&local] { return local * 2; });
// local 可能在 get() 之前已销毁

// 安全：值捕获
auto good = lazy([local] { return local * 2; });
```

### 递归记忆化

递归函数的记忆化需要特殊处理：

```cpp
// 错误：lambda 无法直接递归
auto fib = memoize([](int n) {
    if (n <= 1) return n;
    return fib(n-1) + fib(n-2);  // 编译错误：fib 尚未定义
});

// 正确：使用 std::function
std::function<int(int)> fib;
fib = memoize<int, int>([&fib](int n) -> int {
    if (n <= 1) return n;
    return fib(n-1) + fib(n-2);
});
```

## 最佳实践

1. **识别昂贵计算**：只对真正需要优化的部分使用惰性
2. **注意内存**：缓存会占用内存，确保不会无限增长
3. **线程安全**：多线程环境使用 `std::call_once`
4. **避免副作用**：惰性计算的函数应该是纯的
5. **明确生命周期**：确保 thunk 捕获的变量有效

## 代码示例

完整示例：`examples/01_lazy_memoization.cpp`

## 练习

1. **实现 Lazy<T>**：支持 get、fmap
2. **实现 memoize**：单参数记忆化
3. **斐波那契优化**：比较朴素和记忆化版本
4. **惰性列表**：实现 take、filter、map
5. **配置系统**：使用 Lazy 延迟加载配置

练习文件：`exercises/exercises.cpp`
参考答案：`exercises/solutions.cpp`

## 下一章

[12 - 实战项目：查询 DSL](../12-capstone/README.md)
