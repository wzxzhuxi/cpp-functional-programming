# 03 - 纯函数

> 纯函数是函数式编程的基石。

## 本章目标

学完本章，你将能够：
- 理解什么是纯函数及其两个核心条件
- 识别副作用并理解它们的危害
- 掌握引用透明性的概念
- 学会隔离副作用的技巧
- 将不纯函数重构为纯函数

## 什么是纯函数

**纯函数（Pure Function）** 满足两个条件：

1. **确定性**：相同输入永远得到相同输出
2. **无副作用**：不修改外部状态，不依赖外部可变状态

```cpp
// 纯函数：满足两个条件
int add(int a, int b) {
    return a + b;
}

// 不纯：依赖外部可变状态
int factor = 10;
int bad_multiply(int x) {
    return x * factor;  // 结果取决于 factor 当前值
}

// 不纯：修改外部状态
int counter = 0;
int bad_count(int x) {
    counter++;  // 副作用
    return x;
}

// 不纯：I/O 操作
int bad_log(int x) {
    std::cout << x;  // 副作用：输出
    return x;
}
```

## 副作用（Side Effects）

副作用是函数执行时对外部世界的可观察变化：

| 副作用类型 | 示例 |
|-----------|------|
| 修改全局/静态变量 | `counter++` |
| 修改传入的引用参数 | `vec.push_back(x)` |
| I/O 操作 | `std::cout`, 文件读写 |
| 抛出异常 | `throw std::runtime_error(...)` |
| 获取系统状态 | `time()`, `rand()` |
| 数据库/网络操作 | 查询、发送请求 |

### 副作用示例对比

```cpp
// 有副作用：修改参数
void bad_normalize(std::vector<double>& vec) {
    double sum = std::accumulate(vec.begin(), vec.end(), 0.0);
    for (auto& x : vec) x /= sum;  // 修改了调用者的数据
}

// 纯函数：返回新值
[[nodiscard]] std::vector<double> normalize(const std::vector<double>& vec) {
    double sum = std::accumulate(vec.begin(), vec.end(), 0.0);
    std::vector<double> result;
    result.reserve(vec.size());
    for (double x : vec) {
        result.push_back(x / sum);
    }
    return result;
}
```

## 引用透明性

**引用透明**：表达式可以被其结果替换，而不改变程序行为。

纯函数具有引用透明性。

```cpp
// 纯函数
int square(int x) { return x * x; }

// 引用透明：可以直接替换
int a = square(5);              // 25
int b = square(5) + square(5);  // 50

// 等价于
int a = 25;
int b = 25 + 25;  // 编译器可以优化

// 不纯函数
int global = 10;
int bad_square(int x) { return x * x + global; }

// 非引用透明：不能替换
int c = bad_square(5);  // 35
global = 20;
int d = bad_square(5);  // 45（结果改变！）
```

**引用透明性的好处：**
- **编译器优化**：可以缓存结果、重排计算、自动并行化
- **易于推理**：函数调用等于值
- **可记忆化**：相同输入的结果可以缓存

## 纯函数的好处

### 1. 易于测试

```cpp
// 纯函数：测试简单
int multiply(int a, int b) { return a * b; }

// 不需要 setup/teardown
assert(multiply(2, 3) == 6);
assert(multiply(0, 100) == 0);
assert(multiply(-2, 3) == -6);

// 不纯函数测试困难
class Database {
    int query_count = 0;
public:
    int query(const std::string& sql) {
        query_count++;  // 状态改变
        return 42;
    }
};
// 需要复杂的 mock/reset
```

### 2. 易于并行

```cpp
std::vector<int> data = {1, 2, 3, 4, 5};
std::vector<int> result(data.size());

// 纯函数：天然线程安全，可以并行
std::transform(std::execution::par,
               data.begin(), data.end(), result.begin(),
               [](int x) { return x * x; });  // 无副作用

// 不纯：数据竞争
int sum = 0;
std::for_each(data.begin(), data.end(),
              [&sum](int x) { sum += x; });  // 多线程危险！
```

### 3. 易于组合

```cpp
auto f = [](int x) { return x * 2; };
auto g = [](int x) { return x + 1; };

// 纯函数可以随意组合
auto h = [&](int x) { return g(f(x)); };  // 先 f 后 g

// C++20 Ranges 管道
auto result = nums
    | std::views::transform(f)
    | std::views::transform(g);
```

### 4. 易于调试

```cpp
// 纯函数：bug 只可能在这个函数内
int calculate(int x, int y) {
    return x * 2 + y * 3;
}

// 不纯函数：bug 可能来自任何地方
extern int hidden_state;
int bad_calculate(int x, int y) {
    hidden_state += x;
    return x * 2 + y * 3 + hidden_state;
}
// 结果错了？可能是其他代码改了 hidden_state
```

## 副作用隔离

实际程序需要副作用（否则没有输出）。关键是**隔离副作用**。

### 核心原则

1. **副作用推到边界**：核心逻辑纯函数，I/O 在外层
2. **明确标记**：有副作用的函数清晰命名
3. **最小化范围**：副作用代码越少越好

### 架构模式：纯核心 + 不纯外壳

```cpp
// 纯函数：所有业务逻辑
[[nodiscard]] std::vector<int> process(const std::vector<int>& data) {
    auto view = data
        | std::views::filter([](int x) { return x > 0; })
        | std::views::transform([](int x) { return x * 2; });
    return {view.begin(), view.end()};  // C++20 兼容的收集方式
    // C++23 可用: | std::ranges::to<std::vector>()
}

// 不纯：只负责 I/O
int main() {
    auto input = read_file("input.txt");   // 副作用：读取
    auto result = process(input);          // 纯函数
    write_file("output.txt", result);      // 副作用：写入
}
```

### 状态分离

```cpp
// 差：状态和逻辑耦合
class BadCounter {
    int count = 0;
public:
    void process(int x) {
        count += x;  // 修改状态 + 业务逻辑
    }
};

// 好：分离状态和逻辑
// 纯函数
[[nodiscard]] int add(int current, int delta) {
    return current + delta;
}

// 状态管理
class Counter {
    int count = 0;
public:
    void update(int x) {
        count = add(count, x);  // 调用纯函数
    }
    [[nodiscard]] int get() const { return count; }
};
```

## 记忆化（Memoization）

纯函数可以安全缓存结果。

```cpp
// 通用记忆化包装器
template<typename F>
auto memoize(F f) {
    using ArgType = typename detail::function_traits<F>::arg_type;
    using RetType = typename detail::function_traits<F>::return_type;

    auto cache = std::make_shared<std::unordered_map<ArgType, RetType>>();

    return [f, cache](ArgType arg) {
        auto it = cache->find(arg);
        if (it != cache->end()) {
            return it->second;
        }
        auto result = f(arg);
        (*cache)[arg] = result;
        return result;
    };
}

// 使用
int expensive_fib(int n) {
    if (n <= 1) return n;
    return expensive_fib(n-1) + expensive_fib(n-2);  // O(2^n)
}

auto fast_fib = memoize(expensive_fib);  // O(n)
```

**注意**：记忆化在内部有缓存（技术上是副作用），但对外仍是纯函数——相同输入永远得到相同输出。

## 实践原则

1. **默认写纯函数**——除非必须有副作用
2. **用 `const`**——参数和成员函数都加 `const`
3. **用 `[[nodiscard]]`**——纯函数的结果必须被使用
4. **返回新值**——不修改输入，返回计算结果
5. **副作用集中**——I/O、状态修改在明确的地方
6. **命名清晰**——`get_*` 应该是纯的，`set_*`/`do_*` 可以不纯

## 代码示例

完整示例：`examples/01_pure_functions.cpp`

## 练习

1. **判断纯度**：识别哪些函数是纯函数，解释为什么
2. **重构为纯**：将不纯函数改写为纯函数
3. **实现记忆化**：为递归函数添加缓存
4. **分离副作用**：将混合逻辑拆分为纯函数 + I/O

练习文件：`exercises/exercises.cpp`
参考答案：`exercises/solutions.cpp`

## 下一章

[04 - 高阶函数](../04-higher-order-functions/README.md)
