/**
 * @file solutions.cpp
 * @brief 第04章练习题参考答案
 */

#include <algorithm>
#include <cassert>
#include <functional>
#include <iostream>
#include <map>
#include <numeric>
#include <optional>
#include <string>
#include <utility>
#include <vector>

// ============================================
// 练习 1: 实现 zip - 答案
// ============================================

template<typename T, typename U>
[[nodiscard]] std::vector<std::pair<T, U>> zip(
    const std::vector<T>& vec1,
    const std::vector<U>& vec2
) {
    std::vector<std::pair<T, U>> result;
    size_t min_size = std::min(vec1.size(), vec2.size());
    result.reserve(min_size);

    for (size_t i = 0; i < min_size; ++i) {
        result.emplace_back(vec1[i], vec2[i]);
    }

    return result;
}

// 替代实现：使用 transform
template<typename T, typename U>
[[nodiscard]] std::vector<std::pair<T, U>> zip_alt(
    const std::vector<T>& vec1,
    const std::vector<U>& vec2
) {
    std::vector<std::pair<T, U>> result;
    size_t min_size = std::min(vec1.size(), vec2.size());
    result.reserve(min_size);

    std::transform(
        vec1.begin(), vec1.begin() + static_cast<std::ptrdiff_t>(min_size),
        vec2.begin(),
        std::back_inserter(result),
        [](const T& a, const U& b) { return std::make_pair(a, b); }
    );

    return result;
}

void test_exercise_1() {
    std::cout << "=== 练习 1: 实现 zip - 答案 ===\n";

    std::vector<int> nums = {1, 2, 3};
    std::vector<std::string> strs = {"a", "b", "c"};

    auto zipped = zip(nums, strs);

    assert(zipped.size() == 3);
    assert(zipped[0].first == 1 && zipped[0].second == "a");
    assert(zipped[1].first == 2 && zipped[1].second == "b");
    assert(zipped[2].first == 3 && zipped[2].second == "c");

    std::cout << "zip 结果: ";
    for (const auto& [n, s] : zipped) {
        std::cout << "(" << n << ", " << s << ") ";
    }
    std::cout << "\n";

    // 测试不等长
    std::vector<int> short_nums = {10, 20};
    auto zipped2 = zip(short_nums, strs);
    assert(zipped2.size() == 2);

    std::cout << "zip 测试通过\n\n";
}

// ============================================
// 练习 2: 实现 flat_map - 答案
// ============================================

template<typename T, typename F>
[[nodiscard]] auto flat_map(const std::vector<T>& vec, F f) {
    using R = typename std::invoke_result_t<F, const T&>::value_type;
    std::vector<R> result;

    for (const auto& item : vec) {
        auto sub = f(item);
        result.insert(result.end(), sub.begin(), sub.end());
    }

    return result;
}

// 替代实现：先 map 再 flatten
template<typename T, typename F>
[[nodiscard]] auto flat_map_alt(const std::vector<T>& vec, F f) {
    using Inner = std::invoke_result_t<F, const T&>;
    using R = typename Inner::value_type;

    // Step 1: map
    std::vector<Inner> mapped;
    mapped.reserve(vec.size());
    std::transform(vec.begin(), vec.end(), std::back_inserter(mapped), f);

    // Step 2: flatten
    std::vector<R> result;
    for (const auto& inner : mapped) {
        result.insert(result.end(), inner.begin(), inner.end());
    }

    return result;
}

void test_exercise_2() {
    std::cout << "=== 练习 2: 实现 flat_map - 答案 ===\n";

    std::vector<int> nums = {1, 2, 3};

    auto result = flat_map(nums, [](int x) {
        return std::vector<int>{x, x * 2};
    });

    assert(result.size() == 6);
    assert(result[0] == 1);
    assert(result[1] == 2);
    assert(result[2] == 2);
    assert(result[3] == 4);
    assert(result[4] == 3);
    assert(result[5] == 6);

    std::cout << "flat_map 结果: ";
    for (int x : result) std::cout << x << " ";
    std::cout << "\n";

    // 字符串分割
    std::vector<std::string> words = {"hello", "world"};
    auto chars = flat_map(words, [](const std::string& s) {
        return std::vector<char>(s.begin(), s.end());
    });
    assert(chars.size() == 10);

    std::cout << "flat_map 测试通过\n\n";
}

// ============================================
// 练习 3: 实现 group_by - 答案
// ============================================

template<typename T, typename KeyFn>
[[nodiscard]] auto group_by(const std::vector<T>& vec, KeyFn key_fn) {
    using K = std::invoke_result_t<KeyFn, const T&>;
    std::map<K, std::vector<T>> result;

    for (const auto& item : vec) {
        result[key_fn(item)].push_back(item);
    }

    return result;
}

void test_exercise_3() {
    std::cout << "=== 练习 3: 实现 group_by - 答案 ===\n";

    std::vector<int> nums = {1, 2, 3, 4, 5, 6};

    auto grouped = group_by(nums, [](int x) { return x % 2; });

    assert(grouped.size() == 2);
    assert(grouped[0].size() == 3);
    assert(grouped[1].size() == 3);

    std::cout << "按奇偶分组:\n";
    std::cout << "  偶数: ";
    for (int x : grouped[0]) std::cout << x << " ";
    std::cout << "\n  奇数: ";
    for (int x : grouped[1]) std::cout << x << " ";
    std::cout << "\n";

    // 按长度分组
    std::vector<std::string> words = {"a", "bb", "ccc", "dd", "e", "fff"};
    auto by_length = group_by(words, [](const std::string& s) {
        return s.length();
    });

    assert(by_length[1].size() == 2);
    assert(by_length[2].size() == 2);
    assert(by_length[3].size() == 2);

    std::cout << "group_by 测试通过\n\n";
}

// ============================================
// 练习 4: 数据处理管道 - 答案
// ============================================

struct Product {
    std::string name;
    std::string category;
    double price;
    int stock;
};

template<typename T, typename Pred>
[[nodiscard]] std::vector<T> filter(const std::vector<T>& vec, Pred pred) {
    std::vector<T> result;
    std::copy_if(vec.begin(), vec.end(), std::back_inserter(result), pred);
    return result;
}

template<typename T, typename Acc, typename F>
[[nodiscard]] Acc reduce(const std::vector<T>& vec, Acc init, F f) {
    return std::accumulate(vec.begin(), vec.end(), init,
        [&f](Acc acc, const T& item) { return f(acc, item); });
}

[[nodiscard]] std::map<std::string, double> calculate_category_values(
    const std::vector<Product>& products
) {
    // Step 1: 筛选有库存的商品
    auto in_stock = filter(products, [](const Product& p) {
        return p.stock > 0;
    });

    // Step 2: 按类别分组
    auto by_category = group_by(in_stock, [](const Product& p) {
        return p.category;
    });

    // Step 3: 计算每个类别的总价值
    std::map<std::string, double> result;
    for (const auto& [category, items] : by_category) {
        double total = reduce(items, 0.0, [](double acc, const Product& p) {
            return acc + p.price * p.stock;
        });
        result[category] = total;
    }

    return result;
}

// 替代实现：单遍历（更高效）
[[nodiscard]] std::map<std::string, double> calculate_category_values_optimized(
    const std::vector<Product>& products
) {
    std::map<std::string, double> result;

    for (const auto& p : products) {
        if (p.stock > 0) {
            result[p.category] += p.price * p.stock;
        }
    }

    return result;
}

void test_exercise_4() {
    std::cout << "=== 练习 4: 数据处理管道 - 答案 ===\n";

    std::vector<Product> products = {
        {"苹果", "水果", 5.0, 100},
        {"香蕉", "水果", 3.0, 50},
        {"牛奶", "乳制品", 8.0, 30},
        {"酸奶", "乳制品", 6.0, 0},
        {"面包", "烘焙", 4.0, 80},
        {"蛋糕", "烘焙", 20.0, 0},
        {"橙汁", "饮料", 10.0, 40},
    };

    auto values = calculate_category_values(products);

    std::cout << "各类别库存价值:\n";
    for (const auto& [category, value] : values) {
        std::cout << "  " << category << ": " << value << "\n";
    }

    assert(values["水果"] == 650.0);
    assert(values["乳制品"] == 240.0);
    assert(values["烘焙"] == 320.0);
    assert(values["饮料"] == 400.0);

    // 测试优化版本
    auto values2 = calculate_category_values_optimized(products);
    assert(values2 == values);

    std::cout << "数据处理管道 测试通过\n\n";
}

// ============================================
// 练习 5: compose_n - 答案
// ============================================

// 基础 compose
template<typename F, typename G>
[[nodiscard]] auto compose(F f, G g) {
    return [f, g](auto x) { return f(g(x)); };
}

// 单函数：恒等
template<typename F>
[[nodiscard]] auto compose_n(F f) {
    return f;
}

// 多函数：递归组合
template<typename F, typename G, typename... Rest>
[[nodiscard]] auto compose_n(F f, G g, Rest... rest) {
    // compose_n(f, g, h, ...) = compose(f, compose_n(g, h, ...))
    return compose(f, compose_n(g, rest...));
}

// 替代实现：使用折叠表达式（C++17）
// 注意：需要从右向左折叠
template<typename... Fs>
[[nodiscard]] auto compose_fold(Fs... fs) {
    // 右折叠：f1 ∘ (f2 ∘ (f3 ∘ ...))
    return (... , [](auto f, auto g) {
        return [f, g](auto x) { return f(g(x)); };
    }(fs));

    // 上面的写法有问题，正确的折叠方式：
}

// 正确的折叠版本
struct ComposeOp {
    template<typename F, typename G>
    auto operator()(F f, G g) const {
        return [f, g](auto x) { return f(g(x)); };
    }
};

template<typename F, typename... Rest>
[[nodiscard]] auto compose_v2(F f, Rest... rest) {
    if constexpr (sizeof...(rest) == 0) {
        return f;
    } else {
        return compose(f, compose_v2(rest...));
    }
}

void test_exercise_5() {
    std::cout << "=== 练习 5: compose_n - 答案 ===\n";

    auto add1 = [](int x) { return x + 1; };
    auto times2 = [](int x) { return x * 2; };
    auto square = [](int x) { return x * x; };

    auto combined = compose_n(add1, times2, square);
    int result = combined(3);

    std::cout << "compose_n(add1, times2, square)(3) = " << result << "\n";
    std::cout << "  = add1(times2(square(3)))\n";
    std::cout << "  = add1(times2(9))\n";
    std::cout << "  = add1(18)\n";
    std::cout << "  = 19\n";
    assert(result == 19);

    // 单函数
    auto single = compose_n(add1);
    assert(single(5) == 6);

    // 两函数
    auto two = compose_n(add1, times2);
    assert(two(5) == 11);

    // 四函数
    auto negate = [](int x) { return -x; };
    auto four = compose_n(add1, times2, square, negate);
    // add1(times2(square(negate(2))))
    // = add1(times2(square(-2)))
    // = add1(times2(4))
    // = add1(8)
    // = 9
    assert(four(2) == 9);

    std::cout << "compose_n 测试通过\n\n";
}

// ============================================
// 额外示例：更多高阶函数
// ============================================

// partition: 分成满足条件和不满足条件两组
template<typename T, typename Pred>
[[nodiscard]] std::pair<std::vector<T>, std::vector<T>>
partition(const std::vector<T>& vec, Pred pred) {
    std::vector<T> yes, no;
    for (const auto& item : vec) {
        (pred(item) ? yes : no).push_back(item);
    }
    return {yes, no};
}

// scan: 类似 reduce，但返回所有中间结果
template<typename T, typename Acc, typename F>
[[nodiscard]] std::vector<Acc> scan(const std::vector<T>& vec, Acc init, F f) {
    std::vector<Acc> result;
    result.reserve(vec.size() + 1);
    result.push_back(init);

    Acc acc = init;
    for (const auto& item : vec) {
        acc = f(acc, item);
        result.push_back(acc);
    }
    return result;
}

// find_first: 返回第一个满足条件的元素
template<typename T, typename Pred>
[[nodiscard]] std::optional<T> find_first(const std::vector<T>& vec, Pred pred) {
    auto it = std::find_if(vec.begin(), vec.end(), pred);
    if (it != vec.end()) {
        return *it;
    }
    return std::nullopt;
}

void bonus_demo() {
    std::cout << "=== 额外示例：更多高阶函数 ===\n";

    std::vector<int> nums = {1, 2, 3, 4, 5, 6};

    // partition
    auto [evens, odds] = partition(nums, [](int x) { return x % 2 == 0; });
    std::cout << "partition(偶数): 偶数=";
    for (int x : evens) std::cout << x << " ";
    std::cout << ", 奇数=";
    for (int x : odds) std::cout << x << " ";
    std::cout << "\n";

    // scan（累积和）
    auto sums = scan(nums, 0, [](int acc, int x) { return acc + x; });
    std::cout << "scan(+): ";
    for (int x : sums) std::cout << x << " ";
    std::cout << "\n";  // 0 1 3 6 10 15 21

    // find_first
    auto first_even = find_first(nums, [](int x) { return x % 2 == 0; });
    std::cout << "find_first(偶数): " << first_even.value_or(-1) << "\n";

    std::cout << "\n";
}

// ============================================
// main
// ============================================

int main() {
    std::cout << "========================================\n";
    std::cout << "    第04章练习参考答案\n";
    std::cout << "========================================\n\n";

    test_exercise_1();
    test_exercise_2();
    test_exercise_3();
    test_exercise_4();
    test_exercise_5();
    bonus_demo();

    std::cout << "========================================\n";
    std::cout << "    所有练习完成!\n";
    std::cout << "========================================\n";

    return 0;
}
