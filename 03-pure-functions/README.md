# 03 - 纯函数

## 什么是纯函数

**纯函数（Pure Function）**：满足两个条件的函数。

1. **相同输入 → 相同输出** - 无论调用多少次，结果始终一致
2. **无副作用** - 不修改外部状态，不依赖外部可变状态

```cpp
// 纯函数
int add(int a, int b) {
    return a + b;
}

// 不纯：依赖外部可变状态
int counter = 0;
int bad_add(int a, int b) {
    return a + b + counter;  // 结果依赖 counter
}

// 不纯：修改外部状态
int worse_add(int a, int b) {
    counter++;  // 副作用！
    return a + b;
}

// 不纯：I/O 操作
int terrible_add(int a, int b) {
    std::cout << a + b;  // 副作用：输出
    return a + b;
}
```

## 副作用（Side Effects）

副作用是指函数执行时对外部世界的任何可观察变化：

- 修改全局变量或静态变量
- 修改函数参数（非 const 引用）
- I/O 操作（打印、文件读写、网络）
- 抛出异常
- 调用有副作用的函数
- 修改数据库状态
- 获取当前时间、随机数

**纯函数不产生副作用。**

### 副作用示例对比

```cpp
// 有副作用：修改参数
void bad_normalize(std::vector<int>& vec) {
    int sum = std::accumulate(vec.begin(), vec.end(), 0);
    for (auto& x : vec) x /= sum;
}

// 纯函数：返回新值
std::vector<int> normalize(const std::vector<int>& vec) {
    int sum = std::accumulate(vec.begin(), vec.end(), 0);
    std::vector<int> result;
    result.reserve(vec.size());
    for (int x : vec) {
        result.push_back(x / sum);
    }
    return result;
}

// 有副作用：依赖和修改全局状态
int request_id = 0;
int get_next_id() {
    return ++request_id;  // 不纯！
}

// 纯函数：明确传递状态
int next_id(int current) {
    return current + 1;
}
```

## 引用透明性（Referential Transparency）

**引用透明**：表达式可以被其求值结果替换，而不改变程序行为。

纯函数具有引用透明性。

```cpp
// 纯函数
int square(int x) { return x * x; }

// 引用透明：可以直接替换
int a = square(5);           // 25
int b = square(5) + square(5);  // 50

// 等价于
int a = 25;
int b = 25 + 25;  // 编译器可以优化

// 不纯函数
int global = 10;
int bad_square(int x) {
    return x * x + global;  // 依赖外部状态
}

// 非引用透明：不能直接替换
int c = bad_square(5);  // 结果依赖 global 当前值
global = 20;
int d = bad_square(5);  // 结果改变！
```

引用透明性的好处：
- **编译器优化** - 可以缓存、重排、并行化
- **易于推理** - 函数调用可以直接看作值
- **可缓存** - 相同输入的结果可以记住（memoization）

## 纯函数的好处

### 1. 易于测试

```cpp
// 纯函数：测试简单直接
int multiply(int a, int b) {
    return a * b;
}

// 测试：不需要 setup/teardown
assert(multiply(2, 3) == 6);
assert(multiply(0, 100) == 0);
assert(multiply(-2, 3) == -6);

// 不纯函数：测试困难
class Database {
    int query_count = 0;
public:
    int query(const std::string& sql) {
        query_count++;  // 副作用
        // ... 数据库操作
        return 42;
    }
};

// 测试需要复杂的 mock/stub
```

### 2. 易于并行

```cpp
// 纯函数：天然线程安全
std::vector<int> data = {1, 2, 3, 4, 5};
std::vector<int> result(data.size());

// 可以安全并行
std::transform(std::execution::par,
               data.begin(), data.end(), result.begin(),
               [](int x) { return x * x; });  // 纯函数

// 不纯函数：数据竞争
int sum = 0;
std::for_each(data.begin(), data.end(),
              [&sum](int x) { sum += x; });  // 危险！多线程会出错
```

### 3. 易于组合

```cpp
// 纯函数可以随意组合
auto f = [](int x) { return x * 2; };
auto g = [](int x) { return x + 1; };
auto h = [](int x) { return g(f(x)); };  // 组合：先 f 后 g

std::vector<int> nums = {1, 2, 3};
auto result = nums
    | std::views::transform(f)
    | std::views::transform(g);
```

### 4. 易于推理和调试

```cpp
// 纯函数：出错就是这个函数的问题
int calculate(int x, int y) {
    return x * 2 + y * 3;
}

// 不纯：问题可能在任何地方
extern int secret_global;
int bad_calculate(int x, int y) {
    secret_global += x;
    return x * 2 + y * 3 + secret_global;
}
// 出错了？可能是其他地方改了 secret_global
```

## 副作用隔离

实际程序不可能完全没有副作用（否则没有输出）。关键是**隔离副作用**。

### 核心原则

1. **副作用推到边界** - 核心逻辑纯函数，I/O 在外层
2. **明确标记** - 有副作用的函数清晰命名或注释
3. **最小化范围** - 副作用的代码越少越好

### 示例：隔离 I/O

```cpp
// 差：逻辑和 I/O 混在一起
void process_and_print() {
    auto data = read_file("input.txt");  // I/O
    auto result = transform(data);
    write_file("output.txt", result);   // I/O
}

// 好：分离逻辑和 I/O
// 纯函数：核心逻辑
std::vector<int> transform(const std::vector<int>& data) {
    std::vector<int> result;
    for (int x : data) {
        result.push_back(x * 2);
    }
    return result;
}

// 不纯：只负责 I/O
void run() {
    auto data = read_file("input.txt");     // 副作用
    auto result = transform(data);          // 纯函数
    write_file("output.txt", result);       // 副作用
}
```

### 示例：隔离状态

```cpp
// 差：状态和逻辑耦合
class BadCounter {
    int count = 0;
public:
    void process(int x) {
        count += x;  // 修改状态
        // ... 其他逻辑
    }
    int get() const { return count; }
};

// 好：状态分离
// 纯函数
int add(int current, int delta) {
    return current + delta;
}

// 状态管理
class Counter {
    int count = 0;
public:
    void update(int x) {
        count = add(count, x);  // 调用纯函数
    }
    int get() const { return count; }
};
```

## 不纯函数的合理使用

有些场景必须不纯，但可以最小化影响：

### 1. 日志和调试

```cpp
// 纯函数主逻辑
int complex_calculation(int x, int y) {
    int step1 = x * 2;
    int step2 = y + 10;
    int result = step1 + step2;

    // 副作用：调试日志（可以移除）
    #ifdef DEBUG
    std::cerr << "Debug: " << result << "\n";
    #endif

    return result;
}
```

### 2. 性能优化（缓存）

```cpp
// 纯函数，但内部缓存（对外透明）
int expensive_calculation(int n) {
    static std::unordered_map<int, int> cache;

    auto it = cache.find(n);
    if (it != cache.end()) {
        return it->second;  // 缓存命中
    }

    // 计算
    int result = /* ... */;
    cache[n] = result;  // 副作用：修改缓存
    return result;
}
// 对外仍是纯函数：相同输入 → 相同输出
```

### 3. I/O 必须在某处发生

```cpp
// 架构：纯函数核心 + 不纯外壳
int main() {
    // 不纯：读取输入
    auto input = read_input();

    // 纯函数：所有业务逻辑
    auto result = process(input);

    // 不纯：输出结果
    write_output(result);

    return 0;
}
```

## const 与纯函数

C++ 用 `const` 帮助实现纯函数：

```cpp
// const 成员函数：承诺不修改对象
class Point {
    int x_, y_;
public:
    // 纯：只读
    int x() const { return x_; }
    int distance_from_origin() const {
        return std::sqrt(x_ * x_ + y_ * y_);
    }

    // 不纯：修改状态
    void move(int dx, int dy) {
        x_ += dx;
        y_ += dy;
    }
};

// const 参数：承诺不修改输入
int sum(const std::vector<int>& nums) {
    return std::accumulate(nums.begin(), nums.end(), 0);
}
```

## 实践建议

1. **默认写纯函数** - 除非必须有副作用
2. **用 const** - 参数和成员函数都加 `const`
3. **返回新值** - 不修改输入，返回计算结果
4. **副作用集中** - I/O、状态修改在明确的地方
5. **命名清晰** - `get_*` 应该是纯的，`set_*`/`update_*` 可以不纯

## 代码示例

完整示例见：`examples/basic/03_pure_functions.cpp`

## 练习

1. 判断哪些函数是纯函数，为什么
2. 重构不纯函数为纯函数
3. 实现一个纯函数的数学计算库
4. 分离副作用：将混合逻辑拆分为纯函数 + I/O

练习题在：`exercises/03_pure_functions_exercises.cpp`
