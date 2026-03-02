# 12 - 实战项目：查询 DSL

> 将所有概念融合成一个类型安全的查询构建器。

## 本章目标

本章通过构建一个内存数据查询 DSL（领域特定语言），综合运用前11章的所有概念：

- **纯函数**：查询操作不修改数据
- **高阶函数**：where、select 接受函数参数
- **函数组合**：管道操作符链式调用
- **代数类型**：表达查询结构
- **Monad**：错误处理和结果组合
- **惰性求值**：查询构建时不执行

## 目标 API

```cpp
auto query = from(users)
    | where([](const User& u) { return u.age >= 18; })
    | select(&User::name, &User::email)
    | order_by(&User::name)
    | limit(10);

auto results = query.execute();
```

## 核心组件

### 1. Query<T> - 惰性查询表示

```cpp
template<typename T>
class Query {
    std::vector<T> source_;
    std::vector<std::function<bool(const T&)>> predicates_;
    std::optional<std::function<bool(const T&, const T&)>> comparator_;
    std::optional<size_t> limit_count_;

public:
    explicit Query(std::vector<T> source)
        : source_(std::move(source)) {}

    // 惰性添加过滤条件
    [[nodiscard]] Query where(std::function<bool(const T&)> pred) const;

    // 惰性添加排序
    template<typename F>
    [[nodiscard]] Query order_by(F projection) const;

    // 惰性添加限制
    [[nodiscard]] Query limit(size_t n) const;

    // 执行查询
    [[nodiscard]] std::vector<T> execute() const;
};
```

### 2. Predicate<T> - 可组合谓词

```cpp
template<typename T>
class Predicate {
    std::function<bool(const T&)> pred_;

public:
    explicit Predicate(std::function<bool(const T&)> pred)
        : pred_(std::move(pred)) {}

    // 调用
    bool operator()(const T& value) const {
        return pred_(value);
    }

    // 组合
    [[nodiscard]] Predicate and_(Predicate other) const;
    [[nodiscard]] Predicate or_(Predicate other) const;
    [[nodiscard]] Predicate not_() const;
};
```

### 3. Projection<T, U> - 字段投影

```cpp
template<typename T, typename... Fields>
class Projection {
    std::tuple<Fields T::*...> fields_;

public:
    explicit Projection(Fields T::*... fields)
        : fields_(fields...) {}

    // 应用投影
    auto operator()(const T& obj) const {
        return std::apply([&obj](auto... ptrs) {
            return std::make_tuple(obj.*ptrs...);
        }, fields_);
    }
};
```

## 函数式设计要点

### 不可变性

每个操作返回新的 Query 对象：

```cpp
Query<User> original = from(users);
Query<User> filtered = original.where(pred);  // original 不变
Query<User> sorted = filtered.order_by(&User::name);  // filtered 不变
```

### 惰性求值

查询构建时不执行任何计算：

```cpp
auto query = from(users)
    | where(expensive_check)     // 不执行
    | order_by(&User::created);  // 不执行

// 只有调用 execute() 才真正计算
auto results = query.execute();
```

### 类型安全

编译时检查类型错误：

```cpp
auto query = from(users)
    | select(&User::name)           // 结果类型变为 string
    | where([](const User& u) {});  // 编译错误！类型不匹配
```

### 组合性

操作可以自由组合：

```cpp
auto adults = where([](const User& u) { return u.age >= 18; });
auto by_name = order_by(&User::name);
auto first10 = limit(10);

auto query = from(users) | adults | by_name | first10;
```

## 实现详解

### Query 类完整实现

```cpp
template<typename T>
class Query {
    std::vector<T> source_;
    std::vector<std::function<bool(const T&)>> predicates_;
    std::optional<std::function<bool(const T&, const T&)>> comparator_;
    std::optional<size_t> limit_count_;

    // 私有构造，用于链式调用
    Query(std::vector<T> source,
          std::vector<std::function<bool(const T&)>> preds,
          std::optional<std::function<bool(const T&, const T&)>> comp,
          std::optional<size_t> limit)
        : source_(std::move(source))
        , predicates_(std::move(preds))
        , comparator_(std::move(comp))
        , limit_count_(limit) {}

public:
    explicit Query(std::vector<T> source)
        : source_(std::move(source)) {}

    [[nodiscard]] Query where(std::function<bool(const T&)> pred) const {
        auto new_preds = predicates_;
        new_preds.push_back(std::move(pred));
        return Query(source_, std::move(new_preds), comparator_, limit_count_);
    }

    template<typename F>
    [[nodiscard]] Query order_by(F projection, bool ascending = true) const {
        auto comp = [projection, ascending](const T& a, const T& b) {
            if (ascending) {
                return projection(a) < projection(b);
            }
            return projection(a) > projection(b);
        };
        return Query(source_, predicates_, comp, limit_count_);
    }

    [[nodiscard]] Query limit(size_t n) const {
        return Query(source_, predicates_, comparator_, n);
    }

    [[nodiscard]] std::vector<T> execute() const {
        std::vector<T> result;

        // 1. 过滤
        for (const auto& item : source_) {
            bool pass = true;
            for (const auto& pred : predicates_) {
                if (!pred(item)) {
                    pass = false;
                    break;
                }
            }
            if (pass) {
                result.push_back(item);
            }
        }

        // 2. 排序
        if (comparator_) {
            std::sort(result.begin(), result.end(), *comparator_);
        }

        // 3. 限制
        if (limit_count_ && result.size() > *limit_count_) {
            result.resize(*limit_count_);
        }

        return result;
    }
};
```

### 管道操作符

```cpp
template<typename T>
Query<T> from(std::vector<T> source) {
    return Query<T>(std::move(source));
}

// 管道辅助类
template<typename T>
struct WhereOp {
    std::function<bool(const T&)> pred;
};

template<typename T>
WhereOp<T> where(std::function<bool(const T&)> pred) {
    return WhereOp<T>{std::move(pred)};
}

template<typename T>
Query<T> operator|(Query<T> query, WhereOp<T> op) {
    return query.where(std::move(op.pred));
}
```

## 扩展功能

### select：投影转换

```cpp
template<typename T, typename F>
auto select(Query<T> query, F projection) {
    using U = std::invoke_result_t<F, const T&>;
    auto results = query.execute();

    std::vector<U> projected;
    projected.reserve(results.size());
    for (const auto& item : results) {
        projected.push_back(projection(item));
    }
    return projected;
}
```

### group_by：分组

```cpp
template<typename T, typename K>
std::map<K, std::vector<T>> group_by(
    const std::vector<T>& data,
    std::function<K(const T&)> key_fn
) {
    std::map<K, std::vector<T>> groups;
    for (const auto& item : data) {
        groups[key_fn(item)].push_back(item);
    }
    return groups;
}
```

### 聚合函数

```cpp
template<typename T>
struct Aggregates {
    static size_t count(const std::vector<T>& data) {
        return data.size();
    }

    template<typename F>
    static auto sum(const std::vector<T>& data, F field) {
        using R = std::invoke_result_t<F, const T&>;
        R total{};
        for (const auto& item : data) {
            total += field(item);
        }
        return total;
    }

    template<typename F>
    static auto avg(const std::vector<T>& data, F field) {
        return static_cast<double>(sum(data, field)) / count(data);
    }
};
```

## 使用示例

```cpp
struct User {
    int id;
    std::string name;
    std::string email;
    int age;
    std::string department;
};

// 数据
std::vector<User> users = {
    {1, "Alice", "alice@example.com", 30, "Engineering"},
    {2, "Bob", "bob@example.com", 25, "Marketing"},
    {3, "Charlie", "charlie@example.com", 35, "Engineering"},
    // ...
};

// 查询：工程部门的成年用户，按年龄排序，取前5个
auto query = from(users)
    | where([](const User& u) { return u.department == "Engineering"; })
    | where([](const User& u) { return u.age >= 18; })
    | order_by(&User::age)
    | limit(5);

auto results = query.execute();

// 分组统计
auto by_dept = group_by(users, [](const User& u) { return u.department; });
for (const auto& [dept, members] : by_dept) {
    std::cout << dept << ": " << members.size() << " 人\n";
}

// 聚合
double avg_age = Aggregates<User>::avg(results, &User::age);
```

## 错误处理

使用 Result 类型处理错误：

```cpp
template<typename T>
class QueryResult {
    std::variant<std::vector<T>, std::string> data_;

public:
    static QueryResult success(std::vector<T> data) {
        QueryResult r;
        r.data_ = std::move(data);
        return r;
    }

    static QueryResult error(std::string msg) {
        QueryResult r;
        r.data_ = std::move(msg);
        return r;
    }

    [[nodiscard]] bool is_success() const { return data_.index() == 0; }
    [[nodiscard]] const std::vector<T>& get() const { return std::get<0>(data_); }
    [[nodiscard]] const std::string& error() const { return std::get<1>(data_); }
};
```

## 代码示例

完整示例：`examples/01_query_dsl.cpp`

## 练习

1. **基础 Query**：实现 where、order_by、limit
2. **管道操作符**：实现 `|` 操作符
3. **投影 select**：实现类型转换的 select
4. **聚合函数**：实现 count、sum、avg、min、max
5. **group_by**：实现分组操作

练习文件：`exercises/exercises.cpp`
参考答案：`exercises/solutions.cpp`

## 总结

通过这个项目，我们综合运用了：

| 章节 | 概念 | 应用 |
|------|------|------|
| 01 | Lambda | 谓词、投影函数 |
| 02 | 不可变性 | Query 返回新对象 |
| 03 | 纯函数 | 查询不修改数据 |
| 04 | 高阶函数 | where/select 接受函数 |
| 05 | 柯里化 | 部分应用的过滤条件 |
| 06 | 函数组合 | 管道操作符 |
| 07 | Ranges | 惰性视图思想 |
| 08 | 代数类型 | 查询结构表示 |
| 09 | 错误处理 | QueryResult |
| 10 | Monad | 链式组合 |
| 11 | 惰性求值 | 延迟执行 |

这就是函数式编程在 C++ 中的实际应用。
