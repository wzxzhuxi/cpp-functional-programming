# 第七章：Monad 模式

## 说人话：Monad 是什么？

Monad 不是什么神秘的魔法。**Monad 就是一个带有特定操作的容器**，让你能够：
1. 把值放进容器（`return` / `pure`）
2. 对容器里的值进行连续操作，自动处理容器的"副作用"（`bind` / `>>=`）

**核心思想**：把"如何处理包装"的逻辑从业务逻辑中分离出来。

### 类比

想象你在流水线上处理包裹：
- **普通函数**：`int -> int`（拿裸露的物品，返回裸露的物品）
- **Monad**：`Box<int> -> Box<int>`（拿包裹，返回包裹，自动处理包装/拆包装）

## 1. Monad 的三大定律

每个 Monad 必须满足：

### 定律 1：左单位元（Left Identity）

```cpp
return(a).bind(f) == f(a)
```

**含义**：先包装再操作 = 直接操作

### 定律 2：右单位元（Right Identity）

```cpp
m.bind(return) == m
```

**含义**：操作后包装回去 = 什么都不做

### 定律 3：结合律（Associativity）

```cpp
m.bind(f).bind(g) == m.bind(x => f(x).bind(g))
```

**含义**：链式操作的顺序无关紧要

## 2. Optional/Maybe Monad

处理"可能没有值"的情况。

### 实现

```cpp
template<typename T>
class Maybe {
public:
    std::optional<T> value;

    // return / pure
    static Maybe just(T v) {
        return Maybe{std::optional<T>(v)};
    }

    static Maybe nothing() {
        return Maybe{std::nullopt};
    }

    // bind / >>=
    template<typename F>
    auto bind(F f) const {
        using R = decltype(f(std::declval<T>()));
        if (!value) return R::nothing();
        return f(*value);
    }

    // fmap (Functor)
    template<typename F>
    auto fmap(F f) const {
        using NewT = decltype(f(std::declval<T>()));
        if (!value) return Maybe<NewT>::nothing();
        return Maybe<NewT>::just(f(*value));
    }
};
```

### 使用案例

```cpp
// 不使用 Monad（丑陋的嵌套）
std::optional<int> divide(int a, int b) {
    if (b == 0) return std::nullopt;
    return a / b;
}

std::optional<int> safe_sqrt(int x) {
    if (x < 0) return std::nullopt;
    return std::sqrt(x);
}

auto result = divide(100, 5);
if (result) {
    auto result2 = safe_sqrt(*result);
    if (result2) {
        // 使用 result2
    }
}

// 使用 Monad（清晰的链式）
Maybe<int>::just(100)
    .bind(divide_maybe(5))
    .bind(safe_sqrt_maybe);
```

## 3. Result/Either Monad

处理"可能失败"的情况，携带错误信息。

### 实现

```cpp
template<typename T, typename E>
class Result {
public:
    std::variant<T, E> data;

    bool is_ok() const { return std::holds_alternative<T>(data); }
    const T& unwrap() const { return std::get<T>(data); }
    const E& unwrap_err() const { return std::get<E>(data); }

    // return / pure
    static Result ok(T value) {
        return Result{std::variant<T, E>(std::in_place_index<0>, value)};
    }

    static Result err(E error) {
        return Result{std::variant<T, E>(std::in_place_index<1>, error)};
    }

    // bind / >>=
    template<typename F>
    auto bind(F f) const {
        using R = decltype(f(std::declval<T>()));
        if (!is_ok()) {
            return R::err(unwrap_err());
        }
        return f(unwrap());
    }

    // fmap
    template<typename F>
    auto fmap(F f) const {
        using NewT = decltype(f(std::declval<T>()));
        if (!is_ok()) {
            return Result<NewT, E>::err(unwrap_err());
        }
        return Result<NewT, E>::ok(f(unwrap()));
    }
};
```

### 使用案例：错误传播

```cpp
Result<int, std::string> parse_int(const std::string& s);
Result<double, std::string> to_celsius(int fahrenheit);
Result<std::string, std::string> format_temp(double celsius);

// 链式调用，错误自动传播
auto result = parse_int("32")
    .bind(to_celsius)
    .bind(format_temp);

if (result.is_ok()) {
    std::cout << result.unwrap();
} else {
    std::cout << "Error: " << result.unwrap_err();
}
```

## 4. IO Monad 概念

处理副作用（I/O 操作）。

### 为什么需要 IO Monad？

纯函数不应该有副作用，但程序必须与外界交互。IO Monad 将副作用"包装"起来，标记哪些操作是不纯的。

### 简化实现（概念演示）

```cpp
template<typename T>
class IO {
public:
    std::function<T()> action;

    explicit IO(std::function<T()> f) : action(f) {}

    // return / pure
    static IO pure(T value) {
        return IO([value]() { return value; });
    }

    // bind
    template<typename F>
    auto bind(F f) const {
        return IO([action = this->action, f]() {
            T result = action();
            return f(result).action();
        });
    }

    // 执行副作用
    T run() const {
        return action();
    }
};

// 使用
IO<std::string> read_line() {
    return IO<std::string>([]() {
        std::string line;
        std::getline(std::cin, line);
        return line;
    });
}

IO<void> print(const std::string& s) {
    return IO<void>([s]() {
        std::cout << s << "\n";
    });
}

// 链式 I/O 操作
auto program = read_line()
    .bind([](const std::string& input) {
        return print("You entered: " + input);
    });

program.run();  // 执行副作用
```

## 5. List Monad

处理"多个值"的情况。

### 实现

```cpp
template<typename T>
class List {
public:
    std::vector<T> items;

    static List pure(T value) {
        return List{std::vector<T>{value}};
    }

    template<typename F>
    auto bind(F f) const {
        using R = decltype(f(std::declval<T>()));
        using NewT = typename R::value_type;

        std::vector<NewT> result;
        for (const auto& item : items) {
            auto r = f(item);
            result.insert(result.end(), r.items.begin(), r.items.end());
        }
        return List<NewT>{result};
    }
};

// 使用：生成所有组合
List<int> numbers{{1, 2, 3}};
List<char> letters{{'a', 'b'}};

auto combinations = numbers.bind([&](int n) {
    return letters.bind([n](char c) {
        return List<std::pair<int, char>>::pure({n, c});
    });
});
// 结果：(1,a), (1,b), (2,a), (2,b), (3,a), (3,b)
```

## 6. Monad vs Functor vs Applicative

### Functor（映射器）

只有 `fmap`：
```cpp
fmap :: (a -> b) -> f a -> f b
```

**含义**：对容器里的值应用函数。

### Applicative（应用器）

有 `pure` 和 `ap`：
```cpp
pure :: a -> f a
ap :: f (a -> b) -> f a -> f b
```

**含义**：容器里的函数应用到容器里的值。

### Monad（单子）

有 `return` 和 `bind`：
```cpp
return :: a -> m a
bind :: m a -> (a -> m b) -> m b
```

**含义**：值到容器的函数，可以链式调用。

### 关系

```
Functor < Applicative < Monad
```

每个 Monad 都是 Applicative，每个 Applicative 都是 Functor。

## 7. 实际应用

### 7.1 数据库查询链

```cpp
Result<User, DbError> find_user(int id);
Result<Orders, DbError> find_orders(const User& user);
Result<double, DbError> calculate_total(const Orders& orders);

auto total = find_user(123)
    .bind(find_orders)
    .bind(calculate_total);
```

### 7.2 解析器组合

```cpp
Parser<int> parse_int();
Parser<std::string> parse_string();

auto parser = parse_int()
    .bind([](int n) {
        return parse_string().bind([n](std::string s) {
            return Parser<Pair>::pure({n, s});
        });
    });
```

### 7.3 异步操作链

```cpp
Future<Response> fetch(const std::string& url);
Future<Data> parse(const Response& resp);
Future<void> save(const Data& data);

auto pipeline = fetch("https://api.example.com")
    .bind(parse)
    .bind(save);
```

## 8. 为什么 Monad 有用？

### 1. 错误处理

不需要到处写 `if (error)`，错误自动传播。

### 2. 副作用管理

明确标记哪些代码有副作用（IO Monad）。

### 3. 状态传递

隐式传递状态（State Monad）。

### 4. 代码组合

小的 monadic 函数可以轻松组合成大的。

## 9. Monad 的坏名声

Monad 在初学者中名声不好，因为：

1. **过度抽象的教学**：用范畴论解释，吓跑新人
2. **术语晦涩**：functor、applicative、endofunctor...
3. **实用价值不明显**：小程序看不出优势

**真相**：
- Monad 就是设计模式，和工厂模式、观察者模式一样
- 不需要数学背景
- 用多了就自然理解了

## 10. 最佳实践

1. **不要过度使用**：不是所有代码都要 monadic
2. **从简单开始**：先用 Optional/Result，再考虑其他
3. **实用主义**：能用简单方法解决就不要强行 Monad
4. **命名清晰**：`bind`、`fmap`、`pure` 等术语要统一

## 总结

Monad 的本质：
- **包装值**：把值放进容器
- **链式操作**：自动处理容器逻辑
- **关注业务**：不用关心包装/拆包装

常见 Monad：
- **Maybe/Optional**：处理可能没有值
- **Result/Either**：处理可能失败
- **IO**：处理副作用
- **List**：处理多个值

掌握 Monad，代码会更简洁、更安全、更易组合。

## 练习

参见：`exercises/07_monads_exercises.cpp`
