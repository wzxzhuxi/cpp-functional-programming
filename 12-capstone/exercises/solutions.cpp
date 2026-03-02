/**
 * @file solutions.cpp
 * @brief 第12章练习题参考答案
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
// 练习 1 答案: Predicate<T> 组合
// ============================================

template<typename T>
class Predicate {
    std::function<bool(const T&)> pred_;

public:
    explicit Predicate(std::function<bool(const T&)> pred)
        : pred_(std::move(pred)) {}

    bool operator()(const T& value) const {
        return pred_(value);
    }

    [[nodiscard]] Predicate and_(Predicate other) const {
        auto p1 = pred_;
        auto p2 = other.pred_;
        return Predicate([p1, p2](const T& x) {
            return p1(x) && p2(x);
        });
    }

    [[nodiscard]] Predicate or_(Predicate other) const {
        auto p1 = pred_;
        auto p2 = other.pred_;
        return Predicate([p1, p2](const T& x) {
            return p1(x) || p2(x);
        });
    }

    [[nodiscard]] Predicate not_() const {
        auto p = pred_;
        return Predicate([p](const T& x) {
            return !p(x);
        });
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

    assert(is_positive_even(4));
    assert(!is_positive_even(3));
    assert(!is_positive_even(-2));

    assert(is_positive_or_even(4));
    assert(is_positive_or_even(3));
    assert(is_positive_or_even(-2));
    assert(!is_positive_or_even(-3));

    assert(!is_not_positive(5));
    assert(is_not_positive(-5));
    assert(is_not_positive(0));

    std::cout << "练习 1 通过\n\n";
}

// ============================================
// 练习 2 答案: Query<T> 基本方法
// ============================================

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

    [[nodiscard]] Query where(std::function<bool(const T&)> pred) const {
        auto new_preds = predicates_;
        new_preds.push_back(std::move(pred));
        return Query(source_, std::move(new_preds), comparator_, limit_count_);
    }

    template<typename F>
    [[nodiscard]] Query order_by(F projection) const {
        auto comp = [projection](const T& a, const T& b) {
            return projection(a) < projection(b);
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

template<typename T>
Query<T> from(std::vector<T> source) {
    return Query<T>(std::move(source));
}

void test_exercise_2() {
    std::cout << "=== 练习 2: Query 基本方法 ===\n";

    std::vector<int> numbers = {5, 2, 8, 1, 9, 3, 7, 4, 6, 10};

    auto evens = from(numbers)
        .where([](int n) { return n % 2 == 0; })
        .execute();
    assert(evens.size() == 5);

    auto sorted = from(numbers)
        .order_by([](int n) { return n; })
        .execute();
    assert(sorted[0] == 1);
    assert(sorted[9] == 10);

    auto first3 = from(numbers)
        .limit(3)
        .execute();
    assert(first3.size() == 3);

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
// 练习 3 答案: 管道操作符
// ============================================

template<typename T>
struct WhereOp {
    std::function<bool(const T&)> pred;
};

template<typename T>
WhereOp<T> where_op(std::function<bool(const T&)> pred) {
    return WhereOp<T>{std::move(pred)};
}

template<typename T>
Query<T> operator|(Query<T> query, WhereOp<T> op) {
    return query.where(std::move(op.pred));
}

struct LimitOp {
    size_t count;
};

inline LimitOp limit_op(size_t n) {
    return LimitOp{n};
}

template<typename T>
Query<T> operator|(Query<T> query, LimitOp op) {
    return query.limit(op.count);
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
// 练习 4 答案: 分组和聚合
// ============================================

struct Person {
    std::string name;
    std::string city;
    int age;
};

template<typename T, typename K>
std::map<K, std::vector<T>> group_by(
    const std::vector<T>& data,
    std::function<K(const T&)> key_fn
) {
    std::map<K, std::vector<T>> result;
    for (const auto& item : data) {
        result[key_fn(item)].push_back(item);
    }
    return result;
}

template<typename T, typename F>
auto sum(const std::vector<T>& data, F field) {
    using R = std::invoke_result_t<F, const T&>;
    R total{};
    for (const auto& item : data) {
        total += field(item);
    }
    return total;
}

template<typename T, typename F>
double avg(const std::vector<T>& data, F field) {
    if (data.empty()) return 0.0;
    return static_cast<double>(sum(data, field)) / static_cast<double>(data.size());
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

    int total_age = sum(people, [](const Person& p) { return p.age; });
    assert(total_age == 150);
    std::cout << "年龄总和: " << total_age << "\n";

    double avg_age = avg(people, [](const Person& p) { return p.age; });
    assert(avg_age == 30.0);
    std::cout << "平均年龄: " << avg_age << "\n";

    std::cout << "练习 4 通过\n\n";
}

// ============================================
// 练习 5 答案: 综合应用
// ============================================

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

    // 1. 查找价格在 50-200 之间的电子产品
    auto mid_price_electronics = from(products)
        .where([](const Product& p) { return p.category == "Electronics"; })
        .where([](const Product& p) { return p.price >= 50 && p.price <= 200; })
        .execute();

    std::cout << "1. 电子产品(50-200元):\n";
    for (const auto& p : mid_price_electronics) {
        std::cout << "   " << p.name << ": $" << p.price << "\n";
    }
    assert(mid_price_electronics.size() == 2);  // Keyboard, Headphones

    // 2. 按类别分组
    auto by_category = group_by<Product, std::string>(
        products,
        [](const Product& p) { return p.category; }
    );

    std::cout << "2. 按类别分组:\n";
    for (const auto& [category, prods] : by_category) {
        std::cout << "   " << category << ": " << prods.size() << " 个产品\n";
    }
    assert(by_category["Electronics"].size() == 5);
    assert(by_category["Furniture"].size() == 3);

    // 3. 电子产品平均价格
    auto electronics = from(products)
        .where([](const Product& p) { return p.category == "Electronics"; })
        .execute();

    double electronics_avg_price = avg(electronics,
        [](const Product& p) { return p.price; });

    std::cout << "3. 电子产品平均价格: $" << electronics_avg_price << "\n";
    // (999.99 + 29.99 + 79.99 + 399.99 + 149.99) / 5 = 331.99
    assert(electronics_avg_price > 331 && electronics_avg_price < 333);

    // 4. 找库存最多的3个产品
    auto top_stock = from(products)
        .order_by([](const Product& p) { return -p.stock; })  // 负数实现降序
        .limit(3)
        .execute();

    std::cout << "4. 库存最多的3个产品:\n";
    for (const auto& p : top_stock) {
        std::cout << "   " << p.name << ": " << p.stock << " 件\n";
    }

    std::cout << "练习 5 通过\n\n";
}

// ============================================
// main
// ============================================

int main() {
    std::cout << "========================================\n";
    std::cout << "    第12章练习：查询 DSL (参考答案)\n";
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
