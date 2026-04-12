# 10 - Functor 与 Monad 入门

> 让容器支持函数式组合。

## 本章目标

学完本章，你将能够：
- 理解 Functor 和 Monad 的本质
- 为自定义类型实现 fmap 和 bind
- 理解 Monad 法则
- 用 Monad 优雅地组合操作

## 不要被术语吓到

**Functor** 和 **Monad** 这些词听起来很吓人，但概念其实很简单：

- **Functor**：可以对里面的值应用函数的容器
- **Monad**：可以链式调用返回容器的函数的容器

你已经用过很多次了，只是不知道它们叫这个名字。

## Functor：可映射的容器

### 核心思想

Functor 就是一个实现了 `map`（或 `fmap`）的类型：

```
map: (A -> B) -> F<A> -> F<B>
```

用人话说：给我一个 `A -> B` 的函数，我能把 `F<A>` 变成 `F<B>`。

### 例子：std::optional

```cpp
std::optional<int> maybe_value = 42;

// 概念上的 map（C++23 有 transform）
auto result = maybe_value
    ? std::optional<int>(*maybe_value * 2)
    : std::nullopt;
```

### 例子：std::vector

```cpp
std::vector<int> nums = {1, 2, 3};

// std::transform 就是 map
std::vector<int> doubled;
std::transform(nums.begin(), nums.end(),
               std::back_inserter(doubled),
               [](int x) { return x * 2; });
// doubled = {2, 4, 6}
```

### 自定义 Functor

```cpp
template<typename T>
class Box {
    T value_;

public:
    explicit Box(T value) : value_(std::move(value)) {}

    // fmap: 转换内部值
    template<typename F>
    [[nodiscard]] auto fmap(F f) const {
        using U = std::invoke_result_t<F, const T&>;
        return Box<U>(f(value_));
    }

    [[nodiscard]] const T& get() const { return value_; }
};

// 使用
Box<int> box(42);
auto doubled = box.fmap([](int x) { return x * 2; });  // Box<int>(84)
auto stringed = doubled.fmap([](int x) { return std::to_string(x); });  // Box<string>("84")
```

### Functor 法则

任何 Functor 都应该满足：

**1. 恒等律**：`fmap(id) == id`
```cpp
box.fmap([](auto x) { return x; }) == box  // 映射恒等函数等于不做任何事
```

**2. 组合律**：`fmap(f . g) == fmap(f) . fmap(g)`
```cpp
box.fmap([](int x) { return f(g(x)); })
    ==
box.fmap(g).fmap(f)  // 先映射 g 再映射 f
```

## Monad：可链式组合的容器

### 问题：嵌套容器

用 `fmap` 会产生嵌套：

```cpp
// 假设有这些函数
std::optional<int> parse_int(const std::string& s);
std::optional<double> safe_sqrt(int x);

std::optional<std::string> input = "42";

// 用 fmap 会得到 optional<optional<int>>
auto nested = input.transform([](const std::string& s) {
    return parse_int(s);
});
// nested 的类型是 std::optional<std::optional<int>>
```

### 解决：flatMap / bind

Monad 提供 `flatMap`（也叫 `bind` 或 `>>=`）来解决这个问题：

```
bind: F<A> -> (A -> F<B>) -> F<B>
```

注意区别：
- `fmap`：`(A -> B)` → 普通函数
- `bind`：`(A -> F<B>)` → 返回容器的函数

### 自定义 Maybe Monad

```cpp
template<typename T>
class Maybe {
    std::optional<T> value_;

public:
    // 构造
    static Maybe just(T value) {
        Maybe m;
        m.value_ = std::move(value);
        return m;
    }

    static Maybe nothing() {
        return Maybe{};
    }

    // 查询
    [[nodiscard]] bool has_value() const { return value_.has_value(); }
    [[nodiscard]] const T& get() const { return *value_; }

    // Functor: fmap
    template<typename F>
    [[nodiscard]] auto fmap(F f) const {
        using U = std::invoke_result_t<F, const T&>;
        if (has_value()) {
            return Maybe<U>::just(f(*value_));
        }
        return Maybe<U>::nothing();
    }

    // Monad: bind (也叫 flatMap 或 and_then)
    template<typename F>
    [[nodiscard]] auto bind(F f) const {
        // F: T -> Maybe<U>
        using MaybeU = std::invoke_result_t<F, const T&>;
        if (has_value()) {
            return f(*value_);
        }
        return MaybeU::nothing();
    }
};
```

### 使用 bind 链式调用

```cpp
Maybe<int> parse_int(const std::string& s);
Maybe<double> safe_sqrt(int x);
Maybe<int> safe_divide(int a, int b);

// 链式调用，没有嵌套
auto result = Maybe<std::string>::just("16")
    .bind([](const std::string& s) { return parse_int(s); })
    .bind([](int x) { return safe_sqrt(x); })
    .fmap([](double x) { return x * 2; });  // 最后用 fmap 因为 *2 不会失败
```

### Monad 法则

**1. 左单位元**：`return(a).bind(f) == f(a)`
```cpp
Maybe<int>::just(42).bind(f) == f(42)
```

**2. 右单位元**：`m.bind(return) == m`
```cpp
m.bind([](auto x) { return Maybe<decltype(x)>::just(x); }) == m
```

**3. 结合律**：`m.bind(f).bind(g) == m.bind(x => f(x).bind(g))`
```cpp
m.bind(f).bind(g)
    ==
m.bind([](auto x) { return f(x).bind(g); })
```

## Result Monad

Result（第09章）也是 Monad：

```cpp
template<typename T, typename E>
class Result {
    // ... 之前的实现 ...

    // Functor: map（已有）
    template<typename F>
    [[nodiscard]] auto map(F f) const;

    // Monad: and_then / bind
    template<typename F>
    [[nodiscard]] auto and_then(F f) const {
        if (is_ok()) {
            return f(unwrap());
        }
        using NewResult = std::invoke_result_t<F, const T&>;
        return NewResult::err(unwrap_err());
    }
};
```

使用：

```cpp
Result<int, std::string> parse_int(const std::string& s);
Result<double, std::string> safe_divide(double a, double b);

auto result = parse_int("10")
    .and_then([](int x) { return safe_divide(100.0, x); })
    .map([](double x) { return x * 2; });
```

## Functor vs Monad 对比

| 特性 | Functor | Monad |
|------|---------|-------|
| 核心操作 | fmap/map | bind/flatMap/and_then |
| 函数签名 | `A -> B` | `A -> F<B>` |
| 解决问题 | 值转换 | 链式可能失败的操作 |
| 结果类型 | `F<B>` | `F<B>`（自动展平） |

**简单记忆**：
- `fmap`：普通函数，不会产生嵌套
- `bind`：返回容器的函数，会展平嵌套

## 其他常见 Monad

### List Monad

```cpp
// 简化的 List Monad（仅展示核心 bind 操作）
template<typename T>
class List {
public:
    using value_type = T;

    List() = default;
    List(std::initializer_list<T> init) : data_(init) {}
    explicit List(std::vector<T> v) : data_(std::move(v)) {}

    auto begin() const { return data_.begin(); }
    auto end() const { return data_.end(); }

    // bind: 对每个元素应用函数，展平结果
    template<typename F>
    [[nodiscard]] auto bind(F f) const {
        using U = typename std::invoke_result_t<F, const T&>::value_type;
        std::vector<U> result;
        for (const auto& x : data_) {
            auto sub = f(x);
            result.insert(result.end(), sub.begin(), sub.end());
        }
        return List<U>(std::move(result));
    }

private:
    std::vector<T> data_;
};

// 使用：生成所有可能组合
auto pairs = List<int>{1, 2}.bind([](int x) {
    return List<std::pair<int, char>>{
        {x, 'a'}, {x, 'b'}
    };
});
// pairs = [(1,'a'), (1,'b'), (2,'a'), (2,'b')]
```

### Reader Monad（依赖注入）

```cpp
// 从环境 E 读取配置
template<typename E, typename A>
class Reader {
    std::function<A(const E&)> run_;

public:
    template<typename F>
    [[nodiscard]] auto bind(F f) const {
        return Reader<E, ...>([=](const E& env) {
            A a = run_(env);
            return f(a).run_(env);
        });
    }
};
```

## 实战：组合多个操作

### 场景：用户注册

```cpp
struct User { std::string name; int age; };

Result<std::string, Error> validate_name(const std::string& name);
Result<int, Error> validate_age(const std::string& age_str);
Result<User, Error> create_user(const std::string& name, int age);

// 使用 bind 组合
Result<User, Error> register_user(
    const std::string& name,
    const std::string& age_str
) {
    return validate_name(name)
        .and_then([&](const std::string& valid_name) {
            return validate_age(age_str)
                .and_then([&, valid_name](int age) {
                    return create_user(valid_name, age);
                });
        });
}
```

### 辅助：join 操作

```cpp
// join: F<F<A>> -> F<A>
template<typename T>
Maybe<T> join(const Maybe<Maybe<T>>& mm) {
    if (mm.has_value() && mm.get().has_value()) {
        return Maybe<T>::just(mm.get().get());
    }
    return Maybe<T>::nothing();
}

// 关系：bind(f) == join(fmap(f))
```

## do 记法模拟

Haskell 有 do 记法简化 Monad 使用。C++ 可以用宏模拟：

```cpp
// 简化版（仅示意）
#define BIND(var, expr) \
    .and_then([&](auto var)

// 使用
auto result = validate_name(name)
    BIND(valid_name,
        return validate_age(age_str)
            BIND(age,
                return create_user(valid_name, age)
            )
        )
    );
```

更好的方案是使用 C++20 协程（超出本章范围）。

## 最佳实践

1. **识别 Monad 模式**：当你发现需要展平嵌套容器时，你需要 bind
2. **优先用 map**：如果函数不返回容器，用 map 更简单
3. **遵守法则**：确保实现满足 Monad 三法则
4. **命名一致**：C++ 常用 `and_then`，函数式语言用 `bind` 或 `>>=`
5. **理解本质**：Monad 就是"可以链式调用返回容器函数的容器"

## 代码示例

完整示例：`examples/01_functors_monads.cpp`

## 练习

1. **实现 Box Functor**：带 fmap 的简单包装类型
2. **实现 Maybe Monad**：支持 fmap 和 bind
3. **验证 Monad 法则**：测试你的实现是否满足三法则
4. **组合操作**：使用 bind 链式调用多个可能失败的操作
5. **实现 List Monad**：支持多结果展平

练习文件：`exercises/exercises.cpp`
参考答案：`exercises/solutions.cpp`

## 下一章

[11 - 惰性求值与记忆化](../11-lazy-memoization/README.md)
