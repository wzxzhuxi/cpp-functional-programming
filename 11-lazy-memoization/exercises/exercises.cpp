/**
 * @file exercises.cpp
 * @brief 第11章练习题：惰性求值与记忆化
 *
 * 完成下面的练习，然后运行程序验证结果。
 * 参考答案在 solutions.cpp
 */

#include <cassert>
#include <functional>
#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

// ============================================
// 练习 1: 实现 Lazy<T>
// ============================================

/**
 * Lazy<T> 表示一个延迟计算的值
 *
 * 要求：
 * - 构造函数接受一个 thunk（无参函数）
 * - get() 首次调用时计算，之后返回缓存
 * - is_evaluated() 检查是否已计算
 */
template<typename T>
class Lazy {
    std::function<T()> thunk_;
    mutable std::optional<T> cached_;
    mutable bool evaluated_ = false;

public:
    explicit Lazy(std::function<T()> thunk)
        : thunk_(std::move(thunk)) {}

    // TODO: 实现 get
    [[nodiscard]] const T& get() const {
        // 修改这里
        throw std::runtime_error("Not implemented");
    }

    // TODO: 实现 is_evaluated
    [[nodiscard]] bool is_evaluated() const {
        return false;  // 修改这行
    }

    // TODO: 实现 fmap
    template<typename F>
    [[nodiscard]] auto fmap(F f) const {
        using U = std::invoke_result_t<F, const T&>;
        (void)f;
        // 修改这里
        return Lazy<U>([]() { return U{}; });
    }
};

// 辅助函数
template<typename F>
auto lazy(F f) {
    using T = std::invoke_result_t<F>;
    return Lazy<T>(std::move(f));
}

void test_exercise_1() {
    std::cout << "=== 练习 1: Lazy<T> ===\n";

    int compute_count = 0;

    auto expensive = lazy([&compute_count] {
        compute_count++;
        return 42;
    });

    assert(!expensive.is_evaluated());
    assert(compute_count == 0);

    int val1 = expensive.get();
    assert(val1 == 42);
    assert(expensive.is_evaluated());
    assert(compute_count == 1);

    int val2 = expensive.get();
    assert(val2 == 42);
    assert(compute_count == 1);  // 没有重新计算

    std::cout << "get() 正确: " << val1 << "\n";
    std::cout << "计算次数: " << compute_count << "\n";

    // fmap 测试
    auto doubled = expensive.fmap([](int x) { return x * 2; });
    assert(doubled.get() == 84);
    std::cout << "fmap (*2): " << doubled.get() << "\n";

    std::cout << "练习 1 通过\n\n";
}

// ============================================
// 练习 2: 实现 memoize
// ============================================

/**
 * memoize 将普通函数转换为带缓存的函数
 *
 * 要求：
 * - 相同参数只计算一次
 * - 之后直接返回缓存结果
 */

// TODO: 实现 memoize (单参数版本)
template<typename Arg, typename Result>
auto memoize(std::function<Result(Arg)> f) {
    // 修改这里
    // 提示：使用 shared_ptr<unordered_map> 存储缓存
    return [f](Arg arg) -> Result {
        return f(arg);  // 这只是直接调用，没有缓存
    };
}

void test_exercise_2() {
    std::cout << "=== 练习 2: memoize ===\n";

    int call_count = 0;

    std::function<int(int)> expensive = [&call_count](int n) {
        call_count++;
        return n * n;
    };

    auto memo = memoize(expensive);

    assert(memo(5) == 25);
    assert(call_count == 1);

    assert(memo(5) == 25);  // 缓存命中
    assert(call_count == 1);  // 没有新调用

    assert(memo(3) == 9);
    assert(call_count == 2);

    assert(memo(3) == 9);  // 缓存命中
    assert(call_count == 2);

    assert(memo(5) == 25);  // 再次缓存命中
    assert(call_count == 2);

    std::cout << "5次调用，实际计算: " << call_count << " 次\n";

    std::cout << "练习 2 通过\n\n";
}

// ============================================
// 练习 3: 记忆化斐波那契
// ============================================

// 朴素版本（已提供）
long long fib_naive(int n) {
    if (n <= 1) return n;
    return fib_naive(n - 1) + fib_naive(n - 2);
}

// TODO: 实现记忆化版本
// 提示：需要使用 std::function 和闭包捕获
std::function<long long(int)> make_fib_memo() {
    // 修改这里
    return [](int n) -> long long {
        return fib_naive(n);  // 这只是调用朴素版本
    };
}

void test_exercise_3() {
    std::cout << "=== 练习 3: 记忆化斐波那契 ===\n";

    auto fib_memo = make_fib_memo();

    // 测试正确性
    assert(fib_memo(0) == 0);
    assert(fib_memo(1) == 1);
    assert(fib_memo(10) == 55);
    assert(fib_memo(20) == 6765);

    std::cout << "fib(10) = " << fib_memo(10) << "\n";
    std::cout << "fib(20) = " << fib_memo(20) << "\n";

    // 记忆化版本应该能快速计算更大的数
    // （朴素版本 fib(40) 需要很长时间）
    std::cout << "fib(40) = " << fib_memo(40) << "\n";

    std::cout << "练习 3 通过\n\n";
}

// ============================================
// 练习 4: 惰性列表
// ============================================

/**
 * 实现简化版惰性列表
 *
 * 要求：
 * - take(n): 取前 n 个元素
 * - 支持无限列表
 */
template<typename T>
class LazyList {
public:
    using Generator = std::function<std::pair<T, LazyList>()>;

private:
    std::optional<Generator> gen_;

public:
    // 空列表
    LazyList() : gen_(std::nullopt) {}

    // 从生成器构造
    explicit LazyList(Generator gen) : gen_(std::move(gen)) {}

    [[nodiscard]] bool is_empty() const { return !gen_.has_value(); }

    // TODO: 实现 head
    [[nodiscard]] T head() const {
        // 修改这里
        return T{};
    }

    // TODO: 实现 tail
    [[nodiscard]] LazyList tail() const {
        // 修改这里
        return LazyList();
    }

    // TODO: 实现 take
    [[nodiscard]] std::vector<T> take(int n) const {
        // 修改这里
        (void)n;
        return {};
    }

    // 从初始值和后继函数创建无限列表
    static LazyList iterate(T start, std::function<T(T)> next) {
        return LazyList([start, next]() {
            return std::make_pair(start, iterate(next(start), next));
        });
    }
};

void test_exercise_4() {
    std::cout << "=== 练习 4: 惰性列表 ===\n";

    // 自然数
    auto naturals = LazyList<int>::iterate(0, [](int n) { return n + 1; });

    auto first5 = naturals.take(5);
    assert(first5.size() == 5);
    assert(first5[0] == 0);
    assert(first5[4] == 4);

    std::cout << "自然数前5个: ";
    for (int n : first5) std::cout << n << " ";
    std::cout << "\n";

    // 2的幂
    auto powers = LazyList<int>::iterate(1, [](int n) { return n * 2; });
    auto first8Powers = powers.take(8);

    std::cout << "2的幂前8个: ";
    for (int n : first8Powers) std::cout << n << " ";
    std::cout << "\n";

    assert(first8Powers[0] == 1);
    assert(first8Powers[7] == 128);

    std::cout << "练习 4 通过\n\n";
}

// ============================================
// 练习 5: 配置惰性加载
// ============================================

/**
 * 使用 Lazy 实现配置系统
 *
 * 要求：
 * - 配置值只在首次访问时加载
 * - 加载后缓存结果
 */
class Config {
    // TODO: 使用 Lazy 包装配置值
    std::string host_ = "localhost";  // 改为 Lazy
    int port_ = 8080;                 // 改为 Lazy
    bool debug_ = false;              // 改为 Lazy

    // 加载计数（用于测试）
    mutable int load_count_ = 0;

public:
    // TODO: 修改构造函数使用 lazy
    Config() = default;

    // TODO: 修改为惰性获取
    [[nodiscard]] const std::string& host() const {
        load_count_++;  // 模拟加载
        return host_;
    }

    [[nodiscard]] int port() const {
        load_count_++;
        return port_;
    }

    [[nodiscard]] bool debug() const {
        load_count_++;
        return debug_;
    }

    [[nodiscard]] int load_count() const { return load_count_; }
};

void test_exercise_5() {
    std::cout << "=== 练习 5: 配置惰性加载 ===\n";

    Config config;

    // 创建后不应该有任何加载
    assert(config.load_count() == 0);
    std::cout << "创建后加载次数: " << config.load_count() << "\n";

    // 第一次访问应该加载
    std::string h = config.host();
    assert(config.load_count() == 1);
    std::cout << "访问 host 后: " << config.load_count() << "\n";

    // 再次访问不应该重新加载
    h = config.host();
    assert(config.load_count() == 1);  // 仍然是1
    std::cout << "再次访问 host: " << config.load_count() << "\n";

    // 访问 port
    int p = config.port();
    assert(config.load_count() == 2);
    std::cout << "访问 port 后: " << config.load_count() << "\n";

    // debug 从未访问，不应加载
    std::cout << "debug 未访问，load_count 仍为: " << config.load_count() << "\n";

    std::cout << "host=" << h << ", port=" << p << "\n";
    std::cout << "练习 5 通过\n\n";
}

// ============================================
// main
// ============================================

int main() {
    std::cout << "========================================\n";
    std::cout << "    第11章练习：惰性求值与记忆化\n";
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
