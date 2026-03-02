/**
 * @file exercises.cpp
 * @brief 第12章练习题：查询 DSL 实战
 *
 * 完成下面的练习，然后运行程序验证结果。
 * 参考答案在 solutions.cpp
 */

#include <algorithm>
#include <cassert>
#include <functional>
#include <iostream>
#include <map>
#include <optional>
#include <string>
#include <tuple>
#include <variant>
#include <vector>

// ============================================
// 练习 1: 实现 Predicate<T> 的组合操作
// ============================================

/**
 * Predicate<T> 表示一个可组合的谓词
 *
 * 要求：
 * - and_: 返回两个谓词的逻辑与
 * - or_: 返回两个谓词的逻辑或
 * - not_: 返回谓词的逻辑非
 */
template<typename T>
class Predicate {
    std::function<bool(const T&)> pred_;

public:
    explicit Predicate(std::function<bool(const T&)> pred)
        : pred_(std::move(pred)) {}

    bool operator()(const T& value) const {
        return pred_(value);
    }

    // TODO: 实现 and_
    [[nodiscard]] Predicate and_(Predicate other) const {
        (void)other;
        // 修改这里
        return Predicate([](const T&) { return false; });
    }

    // TODO: 实现 or_
    [[nodiscard]] Predicate or_(Predicate other) const {
        (void)other;
        // 修改这里
        return Predicate([](const T&) { return false; });
    }

    // TODO: 实现 not_
    [[nodiscard]] Predicate not_() const {
        // 修改这里
        return Predicate([](const T&) { return false; });
    }
};

template<typename T, typename F>
Predicate<T> make_predicate(F f) {
    return Predicate<T>(std::move(f));
}

void test_exercise_1() {
    std::cout << "=== 练习 1: Predicate 组合 ===\n";

    auto is_positive = make_predicate<int>([](int x) { return x > 0; });
    auto is_even = make_predicate<int>([](int x) { return x % 2 == 0; });

    auto is_positive_even = is_positive.and_(is_even);
    auto is_positive_or_even = is_positive.or_(is_even);
    auto is_not_positive = is_positive.not_();

    // 测试 and_
    assert(is_positive_even(4));
    assert(!is_positive_even(3));
    assert(!is_positive_even(-2));
    assert(!is_positive_even(-3));

    // 测试 or_
    assert(is_positive_or_even(4));   // 正偶
    assert(is_positive_or_even(3));   // 正奇
    assert(is_positive_or_even(-2));  // 负偶
    assert(!is_positive_or_even(-3)); // 负奇

    // 测试 not_
    assert(!is_not_positive(5));
    assert(is_not_positive(-5));
    assert(is_not_positive(0));

    std::cout << "练习 1 通过\n\n";
}

// ============================================
// 练习 2: 实现 Query<T> 的基本方法
// ============================================

/**
 * Query<T> 表示惰性查询
 *
 * 要求：
 * - where: 添加过滤条件
 * - order_by: 添加排序
 * - limit: 限制结果数量
 * - execute: 执行查询返回结果
 */
template<typename T>
class Query {
    std::vector<T> source_;
    std::vector<std::function<bool(const T&)>> predicates_;
    std::optional<std::function<bool(const T&, const T&)>> comparator_;
    std::optional<size_t> limit_count_;

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

    // TODO: 实现 where
    [[nodiscard]] Query where(std::function<bool(const T&)> pred) const {
        (void)pred;
        // 修改这里
        return Query(source_, predicates_, comparator_, limit_count_);
    }

    // TODO: 实现 order_by
    template<typename F>
    [[nodiscard]] Query order_by(F projection) const {
        (void)projection;
        // 修改这里
        return Query(source_, predicates_, comparator_, limit_count_);
    }

    // TODO: 实现 limit
    [[nodiscard]] Query limit(size_t n) const {
        (void)n;
        // 修改这里
        return Query(source_, predicates_, comparator_, limit_count_);
    }

    // TODO: 实现 execute
    [[nodiscard]] std::vector<T> execute() const {
        // 修改这里
        // 步骤：
        // 1. 过滤：应用所有 predicates_
        // 2. 排序：如果 comparator_ 有值，排序结果
        // 3. 限制：如果 limit_count_ 有值，截取结果
        return source_;  // 这只是返回原数据
    }
};

template<typename T>
Query<T> from(std::vector<T> source) {
    return Query<T>(std::move(source));
}

void test_exercise_2() {
    std::cout << "=== 练习 2: Query 基本方法 ===\n";

    std::vector<int> numbers = {5, 2, 8, 1, 9, 3, 7, 4, 6, 10};

    // 测试 where
    auto evens = from(numbers)
        .where([](int n) { return n % 2 == 0; })
        .execute();
    assert(evens.size() == 5);

    // 测试 order_by
    auto sorted = from(numbers)
        .order_by([](int n) { return n; })
        .execute();
    assert(sorted[0] == 1);
    assert(sorted[9] == 10);

    // 测试 limit
    auto first3 = from(numbers)
        .limit(3)
        .execute();
    assert(first3.size() == 3);

    // 组合测试
    auto result = from(numbers)
        .where([](int n) { return n > 3; })
        .order_by([](int n) { return n; })
        .limit(3)
        .execute();
    assert(result.size() == 3);
    assert(result[0] == 4);
    assert(result[1] == 5);
    assert(result[2] == 6);

    std::cout << "结果: ";
    for (int n : result) std::cout << n << " ";
    std::cout << "\n";

    std::cout << "练习 2 通过\n\n";
}

// ============================================
// 练习 3: 实现管道操作符
// ============================================

/**
 * 实现管道操作符让查询更具表现力
 *
 * 要求：
 * - WhereOp 结构和 where 函数
 * - LimitOp 结构和 limit 函数
 * - 对应的 operator| 重载
 */

// TODO: 实现 WhereOp 和 where 函数
template<typename T>
struct WhereOp {
    std::function<bool(const T&)> pred;
};

template<typename T>
WhereOp<T> where_op(std::function<bool(const T&)> pred) {
    return WhereOp<T>{std::move(pred)};
}

// TODO: 实现 operator| for WhereOp
template<typename T>
Query<T> operator|(Query<T> query, WhereOp<T> op) {
    (void)op;
    // 修改这里
    return query;
}

// TODO: 实现 LimitOp 和 limit_op 函数
struct LimitOp {
    size_t count;
};

inline LimitOp limit_op(size_t n) {
    return LimitOp{n};
}

// TODO: 实现 operator| for LimitOp
template<typename T>
Query<T> operator|(Query<T> query, LimitOp op) {
    (void)op;
    // 修改这里
    return query;
}

void test_exercise_3() {
    std::cout << "=== 练习 3: 管道操作符 ===\n";

    std::vector<int> numbers = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    auto result = from(numbers)
        | where_op<int>([](int n) { return n % 2 == 0; })
        | limit_op(3);

    auto executed = result.execute();

    assert(executed.size() == 3);
    assert(executed[0] == 2);
    assert(executed[1] == 4);
    assert(executed[2] == 6);

    std::cout << "管道结果: ";
    for (int n : executed) std::cout << n << " ";
    std::cout << "\n";

    std::cout << "练习 3 通过\n\n";
}

// ============================================
// 练习 4: 实现 group_by 和聚合函数
// ============================================

/**
 * 实现分组和聚合功能
 */

struct Person {
    std::string name;
    std::string city;
    int age;
};

// TODO: 实现 group_by
template<typename T, typename K>
std::map<K, std::vector<T>> group_by(
    const std::vector<T>& data,
    std::function<K(const T&)> key_fn
) {
    (void)key_fn;
    // 修改这里
    std::map<K, std::vector<T>> result;
    return result;
}

// TODO: 实现 sum 聚合
template<typename T, typename F>
auto sum(const std::vector<T>& data, F field) {
    using R = std::invoke_result_t<F, const T&>;
    R total{};
    (void)data;
    (void)field;
    // 修改这里
    return total;
}

// TODO: 实现 avg 聚合
template<typename T, typename F>
double avg(const std::vector<T>& data, F field) {
    (void)data;
    (void)field;
    // 修改这里
    return 0.0;
}

void test_exercise_4() {
    std::cout << "=== 练习 4: 分组和聚合 ===\n";

    std::vector<Person> people = {
        {"Alice", "Beijing", 30},
        {"Bob", "Shanghai", 25},
        {"Charlie", "Beijing", 35},
        {"Diana", "Shanghai", 28},
        {"Eve", "Beijing", 32},
    };

    // 测试 group_by
    auto by_city = group_by<Person, std::string>(
        people,
        [](const Person& p) { return p.city; }
    );

    assert(by_city.size() == 2);
    assert(by_city["Beijing"].size() == 3);
    assert(by_city["Shanghai"].size() == 2);

    std::cout << "按城市分组:\n";
    for (const auto& [city, persons] : by_city) {
        std::cout << "  " << city << ": " << persons.size() << " 人\n";
    }

    // 测试 sum
    int total_age = sum(people, [](const Person& p) { return p.age; });
    assert(total_age == 150);
    std::cout << "年龄总和: " << total_age << "\n";

    // 测试 avg
    double avg_age = avg(people, [](const Person& p) { return p.age; });
    assert(avg_age == 30.0);
    std::cout << "平均年龄: " << avg_age << "\n";

    std::cout << "练习 4 通过\n\n";
}

// ============================================
// 练习 5: 综合应用
// ============================================

/**
 * 使用前面实现的组件完成复杂查询
 */

struct Product {
    int id;
    std::string name;
    std::string category;
    double price;
    int stock;
};

void test_exercise_5() {
    std::cout << "=== 练习 5: 综合应用 ===\n";

    std::vector<Product> products = {
        {1, "Laptop", "Electronics", 999.99, 50},
        {2, "Mouse", "Electronics", 29.99, 200},
        {3, "Keyboard", "Electronics", 79.99, 150},
        {4, "Desk", "Furniture", 299.99, 30},
        {5, "Chair", "Furniture", 199.99, 45},
        {6, "Monitor", "Electronics", 399.99, 75},
        {7, "Lamp", "Furniture", 49.99, 100},
        {8, "Headphones", "Electronics", 149.99, 120},
    };

    // TODO: 完成以下查询
    // 1. 查找价格在 50-200 之间的电子产品
    auto mid_price_electronics = from(products)
        // .where(...)
        // .where(...)
        .execute();

    // 2. 按类别分组并统计每个类别的产品数量
    // auto by_category = group_by<Product, std::string>(...);

    // 3. 计算电子产品的平均价格
    auto electronics = from(products)
        .where([](const Product& p) { return p.category == "Electronics"; })
        .execute();
    // double electronics_avg_price = avg(electronics, ...);

    std::cout << "练习 5 需要你完成代码后运行\n";
    std::cout << "提示：使用 where, group_by, avg 等函数\n\n";
}

// ============================================
// main
// ============================================

int main() {
    std::cout << "========================================\n";
    std::cout << "    第12章练习：查询 DSL 实战\n";
    std::cout << "========================================\n\n";

    test_exercise_1();
    test_exercise_2();
    test_exercise_3();
    test_exercise_4();
    test_exercise_5();

    std::cout << "========================================\n";
    std::cout << "    所有练习完成!\n";
    std::cout << "========================================\n";

    return 0;
}
