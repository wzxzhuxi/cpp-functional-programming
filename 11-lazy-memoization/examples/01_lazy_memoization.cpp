/**
 * @file 01_lazy_memoization.cpp
 * @brief 惰性求值与记忆化示例
 *
 * 本文件演示：
 * - Lazy<T> 实现
 * - 记忆化技术
 * - 惰性列表
 * - 实际应用场景
 */

#include <chrono>
#include <functional>
#include <iostream>
#include <limits>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

// ============================================
// 1. Lazy<T> 基本实现
// ============================================

template<typename T>
class Lazy {
    std::function<T()> thunk_;
    mutable std::optional<T> cached_;
    mutable bool evaluated_ = false;

public:
    explicit Lazy(std::function<T()> thunk)
        : thunk_(std::move(thunk)) {}

    // 获取值（首次调用时计算）
    [[nodiscard]] const T& get() const {
        if (!evaluated_) {
            cached_ = thunk_();
            evaluated_ = true;
        }
        return *cached_;
    }

    // Functor: fmap
    template<typename F>
    [[nodiscard]] auto fmap(F f) const {
        using U = std::invoke_result_t<F, const T&>;
        auto self = *this;
        return Lazy<U>([self, f]() {
            return f(self.get());
        });
    }

    // Monad: bind
    template<typename F>
    [[nodiscard]] auto bind(F f) const {
        using LazyU = std::invoke_result_t<F, const T&>;
        auto self = *this;
        return LazyU([self, f]() {
            return f(self.get()).get();
        });
    }

    // 检查是否已计算
    [[nodiscard]] bool is_evaluated() const { return evaluated_; }
};

// 辅助函数
template<typename F>
auto lazy(F f) {
    using T = std::invoke_result_t<F>;
    return Lazy<T>(std::move(f));
}

void lazy_basic_demo() {
    std::cout << "=== Lazy 基本用法 ===\n";

    int compute_count = 0;

    auto expensive = lazy([&compute_count] {
        std::cout << "  [计算中...]\n";
        compute_count++;
        return 42;
    });

    std::cout << "Lazy 对象已创建\n";
    std::cout << "是否已计算: " << (expensive.is_evaluated() ? "是" : "否") << "\n";

    std::cout << "第一次获取: " << expensive.get() << "\n";
    std::cout << "第二次获取: " << expensive.get() << "\n";
    std::cout << "第三次获取: " << expensive.get() << "\n";

    std::cout << "计算次数: " << compute_count << "\n";
    std::cout << "\n";
}

// ============================================
// 2. Lazy 的 Functor 和 Monad
// ============================================

void lazy_functor_demo() {
    std::cout << "=== Lazy Functor/Monad ===\n";

    int step_count = 0;

    auto base = lazy([&step_count] {
        std::cout << "  Step 1: 计算基础值\n";
        step_count++;
        return 10;
    });

    auto doubled = base.fmap([&step_count](int x) {
        std::cout << "  Step 2: 乘2\n";
        step_count++;
        return x * 2;
    });

    auto stringed = doubled.fmap([&step_count](int x) {
        std::cout << "  Step 3: 转字符串\n";
        step_count++;
        return std::string("Result: ") + std::to_string(x);
    });

    std::cout << "所有转换已定义，但尚未执行\n";
    std::cout << "步骤计数: " << step_count << "\n\n";

    std::cout << "现在获取最终值...\n";
    std::cout << stringed.get() << "\n";
    std::cout << "步骤计数: " << step_count << "\n";

    std::cout << "\n再次获取（使用缓存）...\n";
    std::cout << stringed.get() << "\n";
    std::cout << "步骤计数: " << step_count << " (没有增加)\n";

    std::cout << "\n";
}

// ============================================
// 3. 记忆化实现
// ============================================

// 单参数记忆化
template<typename Arg, typename Result>
auto memoize(std::function<Result(Arg)> f) {
    auto cache = std::make_shared<std::unordered_map<Arg, Result>>();

    return [f, cache](Arg arg) -> Result {
        auto it = cache->find(arg);
        if (it != cache->end()) {
            return it->second;
        }
        Result result = f(arg);
        (*cache)[arg] = result;
        return result;
    };
}

// 双参数记忆化（使用 pair 作为 key）
struct PairHash {
    template<typename T1, typename T2>
    size_t operator()(const std::pair<T1, T2>& p) const {
        return std::hash<T1>{}(p.first) ^ (std::hash<T2>{}(p.second) << 1);
    }
};

template<typename Arg1, typename Arg2, typename Result>
auto memoize2(std::function<Result(Arg1, Arg2)> f) {
    using Key = std::pair<Arg1, Arg2>;
    auto cache = std::make_shared<std::unordered_map<Key, Result, PairHash>>();

    return [f, cache](Arg1 a1, Arg2 a2) -> Result {
        Key key{a1, a2};
        auto it = cache->find(key);
        if (it != cache->end()) {
            return it->second;
        }
        Result result = f(a1, a2);
        (*cache)[key] = result;
        return result;
    };
}

void memoization_demo() {
    std::cout << "=== 记忆化 ===\n";

    int call_count = 0;

    // 模拟昂贵计算
    std::function<int(int)> expensive = [&call_count](int n) {
        call_count++;
        std::cout << "  计算 f(" << n << ")\n";
        return n * n;
    };

    auto memoized = memoize(expensive);

    std::cout << "调用 memoized(5): " << memoized(5) << "\n";
    std::cout << "调用 memoized(3): " << memoized(3) << "\n";
    std::cout << "调用 memoized(5): " << memoized(5) << " (缓存命中)\n";
    std::cout << "调用 memoized(3): " << memoized(3) << " (缓存命中)\n";
    std::cout << "调用 memoized(7): " << memoized(7) << "\n";

    std::cout << "总调用次数: " << call_count << " (而不是5)\n";

    std::cout << "\n";
}

// ============================================
// 4. 斐波那契：朴素 vs 记忆化
// ============================================

// 朴素版本
long long fib_naive(int n) {
    if (n <= 1) return n;
    return fib_naive(n - 1) + fib_naive(n - 2);
}

// 记忆化版本
std::function<long long(int)> make_fib_memo() {
    std::function<long long(int)> fib;
    fib = memoize<int, long long>([&fib](int n) -> long long {
        if (n <= 1) return n;
        return fib(n - 1) + fib(n - 2);
    });
    return fib;
}

// 迭代版本（最优）
long long fib_iterative(int n) {
    if (n <= 1) return n;
    long long a = 0, b = 1;
    for (int i = 2; i <= n; ++i) {
        long long c = a + b;
        a = b;
        b = c;
    }
    return b;
}

void fibonacci_demo() {
    std::cout << "=== 斐波那契比较 ===\n";

    // 朴素版本（小数值）
    auto start = std::chrono::high_resolution_clock::now();
    long long naive_result = fib_naive(35);
    auto end = std::chrono::high_resolution_clock::now();
    auto naive_time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "朴素 fib(35) = " << naive_result
              << " (" << naive_time.count() << " ms)\n";

    // 记忆化版本
    auto fib_memo = make_fib_memo();
    start = std::chrono::high_resolution_clock::now();
    long long memo_result = fib_memo(35);
    end = std::chrono::high_resolution_clock::now();
    auto memo_time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "记忆化 fib(35) = " << memo_result
              << " (" << memo_time.count() << " ms)\n";

    // 迭代版本
    start = std::chrono::high_resolution_clock::now();
    long long iter_result = fib_iterative(35);
    end = std::chrono::high_resolution_clock::now();
    auto iter_time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "迭代 fib(35) = " << iter_result
              << " (" << iter_time.count() << " ms)\n";

    // 记忆化可以处理更大的数
    std::cout << "记忆化 fib(50) = " << fib_memo(50) << "\n";

    std::cout << "\n";
}

// ============================================
// 5. 惰性序列（简化版）
// ============================================

// 使用生成器函数而非递归数据结构
template<typename T>
class LazySeq {
    std::function<T(size_t)> generator_;
    size_t size_;

public:
    LazySeq(std::function<T(size_t)> gen, size_t size = std::numeric_limits<size_t>::max())
        : generator_(std::move(gen)), size_(size) {}

    // 取前 n 个元素
    [[nodiscard]] std::vector<T> take(size_t n) const {
        std::vector<T> result;
        size_t count = std::min(n, size_);
        result.reserve(count);
        for (size_t i = 0; i < count; ++i) {
            result.push_back(generator_(i));
        }
        return result;
    }

    // fmap：惰性转换
    template<typename F>
    [[nodiscard]] auto fmap(F f) const {
        using U = std::invoke_result_t<F, T>;
        auto gen = generator_;
        return LazySeq<U>([gen, f](size_t i) {
            return f(gen(i));
        }, size_);
    }

    // filter（返回向量，因为惰性 filter 难以高效实现）
    [[nodiscard]] std::vector<T> filter_take(std::function<bool(T)> pred, size_t n) const {
        std::vector<T> result;
        for (size_t i = 0; result.size() < n && i < size_; ++i) {
            T val = generator_(i);
            if (pred(val)) {
                result.push_back(val);
            }
        }
        return result;
    }

    // 工厂函数
    static LazySeq iterate(T start, std::function<T(T)> next) {
        // 预计算缓存，避免重复计算
        auto cache = std::make_shared<std::vector<T>>();
        cache->push_back(start);

        return LazySeq([cache, next](size_t i) {
            while (cache->size() <= i) {
                cache->push_back(next(cache->back()));
            }
            return (*cache)[i];
        });
    }

    static LazySeq from_function(std::function<T(size_t)> f) {
        return LazySeq(std::move(f));
    }
};

void lazy_list_demo() {
    std::cout << "=== 惰性序列 ===\n";

    // 自然数
    auto naturals = LazySeq<int>::from_function([](size_t i) {
        return static_cast<int>(i);
    });
    auto first10 = naturals.take(10);
    std::cout << "自然数前10个: ";
    for (int n : first10) std::cout << n << " ";
    std::cout << "\n";

    // 偶数
    auto evens = naturals.filter_take([](int n) { return n % 2 == 0; }, 10);
    std::cout << "偶数前10个: ";
    for (int n : evens) std::cout << n << " ";
    std::cout << "\n";

    // 平方数
    auto squares = naturals.fmap([](int n) { return n * n; });
    auto first10Squares = squares.take(10);
    std::cout << "平方数前10个: ";
    for (int n : first10Squares) std::cout << n << " ";
    std::cout << "\n";

    // 使用 iterate
    auto powers = LazySeq<int>::iterate(1, [](int n) { return n * 2; });
    auto first8Powers = powers.take(8);
    std::cout << "2的幂前8个: ";
    for (int n : first8Powers) std::cout << n << " ";
    std::cout << "\n";

    std::cout << "\n";
}

// ============================================
// 6. 实际应用：配置延迟加载
// ============================================

class Config {
    Lazy<std::string> db_host_;
    Lazy<int> db_port_;
    Lazy<std::string> api_key_;

public:
    Config()
        : db_host_(lazy([] {
              std::cout << "  [加载 DB_HOST]\n";
              return std::string("localhost");
          }))
        , db_port_(lazy([] {
              std::cout << "  [加载 DB_PORT]\n";
              return 5432;
          }))
        , api_key_(lazy([] {
              std::cout << "  [加载 API_KEY]\n";
              return std::string("secret-key-12345");
          })) {}

    [[nodiscard]] const std::string& db_host() const { return db_host_.get(); }
    [[nodiscard]] int db_port() const { return db_port_.get(); }
    [[nodiscard]] const std::string& api_key() const { return api_key_.get(); }
};

void config_demo() {
    std::cout << "=== 配置延迟加载 ===\n";

    Config config;
    std::cout << "Config 对象已创建，但配置尚未加载\n\n";

    std::cout << "访问 db_host:\n";
    std::cout << "  值: " << config.db_host() << "\n\n";

    std::cout << "再次访问 db_host (使用缓存):\n";
    std::cout << "  值: " << config.db_host() << "\n\n";

    std::cout << "访问 db_port:\n";
    std::cout << "  值: " << config.db_port() << "\n\n";

    std::cout << "注意: api_key 从未被访问，所以从未加载\n";

    std::cout << "\n";
}

// ============================================
// 7. 短路求值
// ============================================

class LazyAnd {
    std::vector<Lazy<bool>> conditions_;

public:
    void add(std::function<bool()> cond) {
        conditions_.push_back(lazy(std::move(cond)));
    }

    [[nodiscard]] bool evaluate() const {
        for (const auto& cond : conditions_) {
            if (!cond.get()) {
                return false;  // 短路
            }
        }
        return true;
    }

    [[nodiscard]] size_t conditions_count() const {
        return conditions_.size();
    }
};

void short_circuit_demo() {
    std::cout << "=== 短路求值 ===\n";

    int eval_count = 0;

    LazyAnd validator;
    validator.add([&eval_count] {
        std::cout << "  检查 1: 通过\n";
        eval_count++;
        return true;
    });
    validator.add([&eval_count] {
        std::cout << "  检查 2: 失败\n";
        eval_count++;
        return false;
    });
    validator.add([&eval_count] {
        std::cout << "  检查 3: 永远不会执行\n";
        eval_count++;
        return true;
    });

    std::cout << "共 " << validator.conditions_count() << " 个条件\n";
    std::cout << "开始验证...\n";

    bool result = validator.evaluate();
    std::cout << "结果: " << (result ? "通过" : "失败") << "\n";
    std::cout << "实际执行的检查: " << eval_count << " 个\n";

    std::cout << "\n";
}

// ============================================
// main
// ============================================

int main() {
    lazy_basic_demo();
    lazy_functor_demo();
    memoization_demo();
    fibonacci_demo();
    lazy_list_demo();
    config_demo();
    short_circuit_demo();

    return 0;
}
