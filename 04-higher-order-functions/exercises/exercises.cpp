/**
 * @file exercises.cpp
 * @brief 第04章练习题：高阶函数
 *
 * 完成下面的练习，然后运行程序验证结果。
 * 参考答案在 solutions.cpp
 */

#include <algorithm>
#include <cassert>
#include <functional>
#include <iostream>
#include <map>
#include <numeric>
#include <string>
#include <utility>
#include <vector>

// ============================================
// 练习 1: 实现 zip
// ============================================

/**
 * zip: 将两个数组合并为 pair 数组
 *
 * 示例：
 *   zip({1, 2, 3}, {"a", "b", "c"}) -> {(1, "a"), (2, "b"), (3, "c")}
 *
 * 要求：
 * 1. 长度不同时，取较短的长度
 * 2. 返回新容器，不修改原容器
 */
template<typename T, typename U>
[[nodiscard]] std::vector<std::pair<T, U>> zip(
    const std::vector<T>& vec1,
    const std::vector<U>& vec2
) {
    // TODO: 实现 zip
    return {};
}

void test_exercise_1() {
    std::cout << "=== 练习 1: 实现 zip ===\n";

    std::vector<int> nums = {1, 2, 3};
    std::vector<std::string> strs = {"a", "b", "c"};

    auto zipped = zip(nums, strs);

    assert(zipped.size() == 3);
    assert(zipped[0].first == 1 && zipped[0].second == "a");
    assert(zipped[1].first == 2 && zipped[1].second == "b");
    assert(zipped[2].first == 3 && zipped[2].second == "c");

    // 测试不等长
    std::vector<int> short_nums = {10, 20};
    auto zipped2 = zip(short_nums, strs);
    assert(zipped2.size() == 2);

    std::cout << "zip 测试通过\n\n";
}

// ============================================
// 练习 2: 实现 flat_map
// ============================================

/**
 * flat_map: 对每个元素应用函数，然后展平结果
 *
 * 示例：
 *   flat_map({1, 2, 3}, x -> {x, x*2}) -> {1, 2, 2, 4, 3, 6}
 *
 * flat_map = flatten(map(f))
 */
template<typename T, typename F>
[[nodiscard]] auto flat_map(const std::vector<T>& vec, F f) {
    using R = typename std::invoke_result_t<F, const T&>::value_type;
    std::vector<R> result;

    // TODO: 实现 flat_map
    // 提示：对每个元素调用 f，f 返回一个 vector，将所有结果连接起来

    return result;
}

void test_exercise_2() {
    std::cout << "=== 练习 2: 实现 flat_map ===\n";

    std::vector<int> nums = {1, 2, 3};

    // 每个数字 x 变成 {x, x*2}
    auto result = flat_map(nums, [](int x) {
        return std::vector<int>{x, x * 2};
    });

    // 期望: {1, 2, 2, 4, 3, 6}
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

    // 另一个例子：字符串分割
    std::vector<std::string> words = {"hello", "world"};
    auto chars = flat_map(words, [](const std::string& s) {
        return std::vector<char>(s.begin(), s.end());
    });
    assert(chars.size() == 10);  // "hello" + "world" = 10 字符

    std::cout << "flat_map 测试通过\n\n";
}

// ============================================
// 练习 3: 实现 group_by
// ============================================

/**
 * group_by: 按键函数分组
 *
 * 示例：
 *   group_by({1, 2, 3, 4, 5}, x -> x % 2)
 *   -> {0: [2, 4], 1: [1, 3, 5]}
 */
template<typename T, typename KeyFn>
[[nodiscard]] auto group_by(const std::vector<T>& vec, KeyFn key_fn) {
    using K = std::invoke_result_t<KeyFn, const T&>;
    std::map<K, std::vector<T>> result;

    // TODO: 实现 group_by

    return result;
}

void test_exercise_3() {
    std::cout << "=== 练习 3: 实现 group_by ===\n";

    std::vector<int> nums = {1, 2, 3, 4, 5, 6};

    // 按奇偶分组
    auto grouped = group_by(nums, [](int x) { return x % 2; });

    assert(grouped.size() == 2);
    assert(grouped[0].size() == 3);  // 偶数: 2, 4, 6
    assert(grouped[1].size() == 3);  // 奇数: 1, 3, 5

    std::cout << "按奇偶分组:\n";
    std::cout << "  偶数: ";
    for (int x : grouped[0]) std::cout << x << " ";
    std::cout << "\n  奇数: ";
    for (int x : grouped[1]) std::cout << x << " ";
    std::cout << "\n";

    // 按字符串长度分组
    std::vector<std::string> words = {"a", "bb", "ccc", "dd", "e", "fff"};
    auto by_length = group_by(words, [](const std::string& s) {
        return s.length();
    });

    assert(by_length[1].size() == 2);  // "a", "e"
    assert(by_length[2].size() == 2);  // "bb", "dd"
    assert(by_length[3].size() == 2);  // "ccc", "fff"

    std::cout << "group_by 测试通过\n\n";
}

// ============================================
// 练习 4: 数据处理管道
// ============================================

struct Product {
    std::string name;
    std::string category;
    double price;
    int stock;
};

/**
 * 使用高阶函数实现以下数据处理需求：
 *
 * 1. 筛选有库存的商品 (stock > 0)
 * 2. 按类别分组
 * 3. 计算每个类别的总库存价值 (price * stock)
 * 4. 返回 map<category, total_value>
 */

// 提供基础的 filter 和 reduce
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
    // TODO: 实现数据处理管道
    // 提示：
    // 1. filter 筛选有库存商品
    // 2. group_by 按类别分组（需要先实现练习3）
    // 3. 对每个分组 reduce 计算总价值

    return {};
}

void test_exercise_4() {
    std::cout << "=== 练习 4: 数据处理管道 ===\n";

    std::vector<Product> products = {
        {"苹果", "水果", 5.0, 100},
        {"香蕉", "水果", 3.0, 50},
        {"牛奶", "乳制品", 8.0, 30},
        {"酸奶", "乳制品", 6.0, 0},      // 无库存
        {"面包", "烘焙", 4.0, 80},
        {"蛋糕", "烘焙", 20.0, 0},        // 无库存
        {"橙汁", "饮料", 10.0, 40},
    };

    auto values = calculate_category_values(products);

    // 水果: 5*100 + 3*50 = 650
    // 乳制品: 8*30 = 240 (酸奶无库存)
    // 烘焙: 4*80 = 320 (蛋糕无库存)
    // 饮料: 10*40 = 400

    std::cout << "各类别库存价值:\n";
    for (const auto& [category, value] : values) {
        std::cout << "  " << category << ": " << value << "\n";
    }

    assert(values["水果"] == 650.0);
    assert(values["乳制品"] == 240.0);
    assert(values["烘焙"] == 320.0);
    assert(values["饮料"] == 400.0);

    std::cout << "数据处理管道 测试通过\n\n";
}

// ============================================
// 练习 5: 实现 compose_n（可变参数组合）
// ============================================

/**
 * compose_n: 组合任意数量的函数
 *
 * 示例：
 *   compose_n(f, g, h)(x) = f(g(h(x)))
 *
 * 提示：使用折叠表达式 (C++17)
 */

// 基础 compose（两个函数）
template<typename F, typename G>
[[nodiscard]] auto compose(F f, G g) {
    return [f, g](auto x) { return f(g(x)); };
}

// TODO: 实现 compose_n
// 提示：递归模板或折叠表达式
template<typename F>
[[nodiscard]] auto compose_n(F f) {
    return f;
}

template<typename F, typename G, typename... Rest>
[[nodiscard]] auto compose_n(F f, G g, Rest... rest) {
    // TODO: 实现可变参数组合
    return f;  // 替换这行
}

void test_exercise_5() {
    std::cout << "=== 练习 5: compose_n ===\n";

    auto add1 = [](int x) { return x + 1; };
    auto times2 = [](int x) { return x * 2; };
    auto square = [](int x) { return x * x; };

    // compose_n(add1, times2, square)(3)
    // = add1(times2(square(3)))
    // = add1(times2(9))
    // = add1(18)
    // = 19

    auto combined = compose_n(add1, times2, square);
    int result = combined(3);

    std::cout << "compose_n(add1, times2, square)(3) = " << result << "\n";
    assert(result == 19);

    // 单函数
    auto single = compose_n(add1);
    assert(single(5) == 6);

    // 两函数
    auto two = compose_n(add1, times2);
    assert(two(5) == 11);  // add1(times2(5)) = add1(10) = 11

    std::cout << "compose_n 测试通过\n\n";
}

// ============================================
// main
// ============================================

int main() {
    std::cout << "========================================\n";
    std::cout << "    第04章练习：高阶函数\n";
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
