/**
 * @file 01_query_dsl.cpp
 * @brief 查询 DSL 实战项目
 *
 * 本文件演示：
 * - Query<T> 惰性查询类
 * - 管道操作符组合
 * - 可组合谓词
 * - 投影和聚合
 */

#include <algorithm>
#include <functional>
#include <iostream>
#include <map>
#include <numeric>
#include <optional>
#include <string>
#include <tuple>
#include <variant>
#include <vector>

// ============================================
// 1. Predicate<T> - 可组合谓词
// ============================================

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

    // 组合：与
    [[nodiscard]] Predicate and_(Predicate other) const {
        auto p1 = pred_;
        auto p2 = other.pred_;
        return Predicate([p1, p2](const T& x) {
            return p1(x) && p2(x);
        });
    }

    // 组合：或
    [[nodiscard]] Predicate or_(Predicate other) const {
        auto p1 = pred_;
        auto p2 = other.pred_;
        return Predicate([p1, p2](const T& x) {
            return p1(x) || p2(x);
        });
    }

    // 组合：非
    [[nodiscard]] Predicate not_() const {
        auto p = pred_;
        return Predicate([p](const T& x) {
            return !p(x);
        });
    }
};

// 谓词工厂函数
template<typename T, typename F>
Predicate<T> make_predicate(F f) {
    return Predicate<T>(std::move(f));
}

void predicate_demo() {
    std::cout << "=== 可组合谓词 ===\n";

    // 基础谓词
    auto is_positive = make_predicate<int>([](int x) { return x > 0; });
    auto is_even = make_predicate<int>([](int x) { return x % 2 == 0; });
    auto is_small = make_predicate<int>([](int x) { return x < 10; });

    // 组合谓词
    auto is_positive_even = is_positive.and_(is_even);
    auto is_positive_or_even = is_positive.or_(is_even);
    auto is_not_positive = is_positive.not_();

    std::vector<int> nums = {-4, -2, 0, 1, 2, 3, 4, 5, 6, 12};

    std::cout << "正偶数: ";
    for (int n : nums) {
        if (is_positive_even(n)) std::cout << n << " ";
    }
    std::cout << "\n";

    std::cout << "正数或偶数: ";
    for (int n : nums) {
        if (is_positive_or_even(n)) std::cout << n << " ";
    }
    std::cout << "\n";

    std::cout << "非正数: ";
    for (int n : nums) {
        if (is_not_positive(n)) std::cout << n << " ";
    }
    std::cout << "\n\n";
}

// ============================================
// 2. Query<T> - 惰性查询类
// ============================================

template<typename T>
class Query {
    std::vector<T> source_;
    std::vector<std::function<bool(const T&)>> predicates_;
    std::optional<std::function<bool(const T&, const T&)>> comparator_;
    std::optional<size_t> limit_count_;
    std::optional<size_t> skip_count_;

    // 私有构造，用于链式调用
    Query(std::vector<T> source,
          std::vector<std::function<bool(const T&)>> preds,
          std::optional<std::function<bool(const T&, const T&)>> comp,
          std::optional<size_t> limit,
          std::optional<size_t> skip)
        : source_(std::move(source))
        , predicates_(std::move(preds))
        , comparator_(std::move(comp))
        , limit_count_(limit)
        , skip_count_(skip) {}

public:
    explicit Query(std::vector<T> source)
        : source_(std::move(source)) {}

    // 惰性添加过滤条件
    [[nodiscard]] Query where(std::function<bool(const T&)> pred) const {
        auto new_preds = predicates_;
        new_preds.push_back(std::move(pred));
        return Query(source_, std::move(new_preds), comparator_, limit_count_, skip_count_);
    }

    // 惰性添加排序
    template<typename F>
    [[nodiscard]] Query order_by(F projection, bool ascending = true) const {
        auto comp = [projection, ascending](const T& a, const T& b) {
            if (ascending) {
                return projection(a) < projection(b);
            }
            return projection(a) > projection(b);
        };
        return Query(source_, predicates_, comp, limit_count_, skip_count_);
    }

    // 降序排序
    template<typename F>
    [[nodiscard]] Query order_by_desc(F projection) const {
        return order_by(projection, false);
    }

    // 惰性添加限制
    [[nodiscard]] Query limit(size_t n) const {
        return Query(source_, predicates_, comparator_, n, skip_count_);
    }

    // 惰性跳过
    [[nodiscard]] Query skip(size_t n) const {
        return Query(source_, predicates_, comparator_, limit_count_, n);
    }

    // 执行查询
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

        // 3. 跳过
        if (skip_count_ && *skip_count_ < result.size()) {
            result.erase(result.begin(), result.begin() + static_cast<long>(*skip_count_));
        } else if (skip_count_) {
            result.clear();
        }

        // 4. 限制
        if (limit_count_ && result.size() > *limit_count_) {
            result.resize(*limit_count_);
        }

        return result;
    }

    // 计数
    [[nodiscard]] size_t count() const {
        return execute().size();
    }

    // 是否存在
    [[nodiscard]] bool any() const {
        for (const auto& item : source_) {
            bool pass = true;
            for (const auto& pred : predicates_) {
                if (!pred(item)) {
                    pass = false;
                    break;
                }
            }
            if (pass) return true;
        }
        return false;
    }

    // 所有元素是否满足条件
    [[nodiscard]] bool all(std::function<bool(const T&)> pred) const {
        auto results = execute();
        return std::all_of(results.begin(), results.end(), pred);
    }

    // 获取第一个元素
    [[nodiscard]] std::optional<T> first() const {
        auto results = limit(1).execute();
        if (results.empty()) return std::nullopt;
        return results[0];
    }

    // 获取源数据
    [[nodiscard]] const std::vector<T>& source() const { return source_; }
};

// 工厂函数
template<typename T>
Query<T> from(std::vector<T> source) {
    return Query<T>(std::move(source));
}

// ============================================
// 3. 管道操作符
// ============================================

// where 操作
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

// order_by 操作
template<typename T, typename F>
struct OrderByOp {
    F projection;
    bool ascending;
};

template<typename T, typename F>
OrderByOp<T, F> order_by(F projection, bool ascending = true) {
    return OrderByOp<T, F>{std::move(projection), ascending};
}

template<typename T, typename F>
Query<T> operator|(Query<T> query, OrderByOp<T, F> op) {
    return query.order_by(op.projection, op.ascending);
}

// limit 操作
struct LimitOp {
    size_t count;
};

inline LimitOp limit(size_t n) {
    return LimitOp{n};
}

template<typename T>
Query<T> operator|(Query<T> query, LimitOp op) {
    return query.limit(op.count);
}

// skip 操作
struct SkipOp {
    size_t count;
};

inline SkipOp skip(size_t n) {
    return SkipOp{n};
}

template<typename T>
Query<T> operator|(Query<T> query, SkipOp op) {
    return query.skip(op.count);
}

// ============================================
// 4. 投影和转换
// ============================================

template<typename T, typename F>
auto select(const Query<T>& query, F projection) {
    using U = std::invoke_result_t<F, const T&>;
    auto results = query.execute();

    std::vector<U> projected;
    projected.reserve(results.size());
    for (const auto& item : results) {
        projected.push_back(projection(item));
    }
    return projected;
}

// 选择多个字段
template<typename T, typename... Fs>
auto select_many(const Query<T>& query, Fs... projections) {
    auto results = query.execute();

    std::vector<std::tuple<std::invoke_result_t<Fs, const T&>...>> projected;
    projected.reserve(results.size());
    for (const auto& item : results) {
        projected.push_back(std::make_tuple(projections(item)...));
    }
    return projected;
}

// ============================================
// 5. 分组和聚合
// ============================================

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

// 聚合函数
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
    static double avg(const std::vector<T>& data, F field) {
        if (data.empty()) return 0.0;
        return static_cast<double>(sum(data, field)) / static_cast<double>(count(data));
    }

    template<typename F>
    static auto min(const std::vector<T>& data, F field) {
        using R = std::invoke_result_t<F, const T&>;
        if (data.empty()) return R{};
        R result = field(data[0]);
        for (size_t i = 1; i < data.size(); ++i) {
            R val = field(data[i]);
            if (val < result) result = val;
        }
        return result;
    }

    template<typename F>
    static auto max(const std::vector<T>& data, F field) {
        using R = std::invoke_result_t<F, const T&>;
        if (data.empty()) return R{};
        R result = field(data[0]);
        for (size_t i = 1; i < data.size(); ++i) {
            R val = field(data[i]);
            if (val > result) result = val;
        }
        return result;
    }
};

// ============================================
// 6. QueryResult - 错误处理
// ============================================

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

    [[nodiscard]] const std::vector<T>& get() const {
        return std::get<0>(data_);
    }

    [[nodiscard]] const std::string& error() const {
        return std::get<1>(data_);
    }

    // fmap
    template<typename F>
    [[nodiscard]] auto map(F f) const {
        using U = std::invoke_result_t<F, const T&>;
        if (is_success()) {
            std::vector<U> result;
            result.reserve(get().size());
            for (const auto& item : get()) {
                result.push_back(f(item));
            }
            return QueryResult<U>::success(std::move(result));
        }
        return QueryResult<U>::error(error());
    }
};

// ============================================
// 7. 示例数据类型
// ============================================

struct User {
    int id;
    std::string name;
    std::string email;
    int age;
    std::string department;

    // 用于输出
    friend std::ostream& operator<<(std::ostream& os, const User& u) {
        os << "User{id=" << u.id << ", name=\"" << u.name
           << "\", age=" << u.age << ", dept=\"" << u.department << "\"}";
        return os;
    }
};

struct Product {
    int id;
    std::string name;
    double price;
    std::string category;
    int stock;
};

// ============================================
// 8. 演示函数
// ============================================

void query_basic_demo() {
    std::cout << "=== 基本查询 ===\n";

    std::vector<User> users = {
        {1, "Alice", "alice@example.com", 30, "Engineering"},
        {2, "Bob", "bob@example.com", 25, "Marketing"},
        {3, "Charlie", "charlie@example.com", 35, "Engineering"},
        {4, "Diana", "diana@example.com", 28, "Sales"},
        {5, "Eve", "eve@example.com", 32, "Engineering"},
        {6, "Frank", "frank@example.com", 22, "Marketing"},
        {7, "Grace", "grace@example.com", 40, "Engineering"},
        {8, "Henry", "henry@example.com", 27, "Sales"},
    };

    // 方法链式调用
    auto results = from(users)
        .where([](const User& u) { return u.department == "Engineering"; })
        .where([](const User& u) { return u.age >= 30; })
        .order_by([](const User& u) { return u.age; })
        .limit(3)
        .execute();

    std::cout << "工程部30岁以上，按年龄排序，前3人:\n";
    for (const auto& u : results) {
        std::cout << "  " << u << "\n";
    }
    std::cout << "\n";
}

void query_pipe_demo() {
    std::cout << "=== 管道操作符 ===\n";

    std::vector<int> numbers = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 15, 20};

    // 使用管道操作符
    auto evens = from(numbers)
        | where<int>([](int n) { return n % 2 == 0; })
        | limit(5);

    std::cout << "偶数前5个: ";
    for (int n : evens.execute()) {
        std::cout << n << " ";
    }
    std::cout << "\n\n";
}

void projection_demo() {
    std::cout << "=== 投影 ===\n";

    std::vector<User> users = {
        {1, "Alice", "alice@example.com", 30, "Engineering"},
        {2, "Bob", "bob@example.com", 25, "Marketing"},
        {3, "Charlie", "charlie@example.com", 35, "Engineering"},
    };

    // 选择单个字段
    auto names = select(from(users), [](const User& u) { return u.name; });
    std::cout << "所有名字: ";
    for (const auto& name : names) {
        std::cout << name << " ";
    }
    std::cout << "\n";

    // 选择多个字段
    auto name_ages = select_many(
        from(users),
        [](const User& u) { return u.name; },
        [](const User& u) { return u.age; }
    );
    std::cout << "名字和年龄:\n";
    for (const auto& [name, age] : name_ages) {
        std::cout << "  " << name << ": " << age << "\n";
    }
    std::cout << "\n";
}

void group_aggregate_demo() {
    std::cout << "=== 分组和聚合 ===\n";

    std::vector<User> users = {
        {1, "Alice", "alice@example.com", 30, "Engineering"},
        {2, "Bob", "bob@example.com", 25, "Marketing"},
        {3, "Charlie", "charlie@example.com", 35, "Engineering"},
        {4, "Diana", "diana@example.com", 28, "Sales"},
        {5, "Eve", "eve@example.com", 32, "Engineering"},
        {6, "Frank", "frank@example.com", 22, "Marketing"},
    };

    // 按部门分组
    auto by_dept = group_by<User, std::string>(
        users,
        [](const User& u) { return u.department; }
    );

    std::cout << "按部门分组:\n";
    for (const auto& [dept, members] : by_dept) {
        std::cout << "  " << dept << ": " << members.size() << " 人\n";
    }
    std::cout << "\n";

    // 聚合
    std::cout << "聚合统计:\n";
    std::cout << "  总人数: " << Aggregates<User>::count(users) << "\n";
    std::cout << "  年龄总和: " << Aggregates<User>::sum(users, [](const User& u) { return u.age; }) << "\n";
    std::cout << "  平均年龄: " << Aggregates<User>::avg(users, [](const User& u) { return u.age; }) << "\n";
    std::cout << "  最小年龄: " << Aggregates<User>::min(users, [](const User& u) { return u.age; }) << "\n";
    std::cout << "  最大年龄: " << Aggregates<User>::max(users, [](const User& u) { return u.age; }) << "\n";
    std::cout << "\n";
}

void lazy_evaluation_demo() {
    std::cout << "=== 惰性求值 ===\n";

    int filter_count = 0;

    std::vector<int> numbers = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    // 构建查询（不执行）
    auto query = from(numbers)
        .where([&filter_count](int n) {
            filter_count++;
            std::cout << "  检查: " << n << "\n";
            return n % 2 == 0;
        })
        .limit(2);

    std::cout << "查询已构建，filter_count = " << filter_count << "\n";
    std::cout << "现在执行...\n";

    auto results = query.execute();

    std::cout << "结果: ";
    for (int n : results) {
        std::cout << n << " ";
    }
    std::cout << "\n";
    std::cout << "filter_count = " << filter_count << "\n\n";
}

void complex_query_demo() {
    std::cout << "=== 复杂查询 ===\n";

    std::vector<Product> products = {
        {1, "Laptop", 999.99, "Electronics", 50},
        {2, "Mouse", 29.99, "Electronics", 200},
        {3, "Keyboard", 79.99, "Electronics", 150},
        {4, "Desk", 299.99, "Furniture", 30},
        {5, "Chair", 199.99, "Furniture", 45},
        {6, "Monitor", 399.99, "Electronics", 75},
        {7, "Lamp", 49.99, "Furniture", 100},
        {8, "Headphones", 149.99, "Electronics", 120},
    };

    // 复杂查询：电子产品，价格100-500，按价格降序，跳过第一个，取3个
    auto expensive_electronics = from(products)
        .where([](const Product& p) { return p.category == "Electronics"; })
        .where([](const Product& p) { return p.price >= 100 && p.price <= 500; })
        .order_by_desc([](const Product& p) { return p.price; })
        .skip(1)
        .limit(3)
        .execute();

    std::cout << "电子产品(100-500元)，价格降序，跳过1取3:\n";
    for (const auto& p : expensive_electronics) {
        std::cout << "  " << p.name << ": $" << p.price << "\n";
    }
    std::cout << "\n";

    // 统计
    auto electronics = from(products)
        .where([](const Product& p) { return p.category == "Electronics"; })
        .execute();

    std::cout << "电子产品统计:\n";
    std::cout << "  数量: " << electronics.size() << "\n";
    std::cout << "  总库存: " << Aggregates<Product>::sum(electronics,
        [](const Product& p) { return p.stock; }) << "\n";
    std::cout << "  平均价格: $" << Aggregates<Product>::avg(electronics,
        [](const Product& p) { return p.price; }) << "\n";
    std::cout << "\n";
}

void query_result_demo() {
    std::cout << "=== QueryResult 错误处理 ===\n";

    auto success_result = QueryResult<int>::success({1, 2, 3, 4, 5});
    auto error_result = QueryResult<int>::error("数据源为空");

    if (success_result.is_success()) {
        std::cout << "成功结果: ";
        for (int n : success_result.get()) {
            std::cout << n << " ";
        }
        std::cout << "\n";
    }

    if (!error_result.is_success()) {
        std::cout << "错误: " << error_result.error() << "\n";
    }

    // fmap
    auto doubled = success_result.map([](int n) { return n * 2; });
    if (doubled.is_success()) {
        std::cout << "加倍后: ";
        for (int n : doubled.get()) {
            std::cout << n << " ";
        }
        std::cout << "\n";
    }

    std::cout << "\n";
}

// ============================================
// main
// ============================================

int main() {
    std::cout << "========================================\n";
    std::cout << "    第12章：查询 DSL 实战项目\n";
    std::cout << "========================================\n\n";

    predicate_demo();
    query_basic_demo();
    query_pipe_demo();
    projection_demo();
    group_aggregate_demo();
    lazy_evaluation_demo();
    complex_query_demo();
    query_result_demo();

    std::cout << "========================================\n";
    std::cout << "    演示完成!\n";
    std::cout << "========================================\n";

    return 0;
}
