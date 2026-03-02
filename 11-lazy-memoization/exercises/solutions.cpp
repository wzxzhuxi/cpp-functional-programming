/**
 * @file solutions.cpp
 * @brief 第11章练习题参考答案
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
// 练习 1 答案: Lazy<T>
// ============================================

template<typename T>
class Lazy {
    std::function<T()> thunk_;
    mutable std::optional<T> cached_;
    mutable bool evaluated_ = false;

public:
    explicit Lazy(std::function<T()> thunk)
        : thunk_(std::move(thunk)) {}

    [[nodiscard]] const T& get() const {
        if (!evaluated_) {
            cached_ = thunk_();
            evaluated_ = true;
        }
        return *cached_;
    }

    [[nodiscard]] bool is_evaluated() const {
        return evaluated_;
    }

    template<typename F>
    [[nodiscard]] auto fmap(F f) const {
        using U = std::invoke_result_t<F, const T&>;
        auto self = *this;  // 复制，避免悬空引用
        return Lazy<U>([self, f]() {
            return f(self.get());
        });
    }
};

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
    assert(compute_count == 1);

    std::cout << "get() 正确: " << val1 << "\n";
    std::cout << "计算次数: " << compute_count << "\n";

    auto doubled = expensive.fmap([](int x) { return x * 2; });
    assert(doubled.get() == 84);
    std::cout << "fmap (*2): " << doubled.get() << "\n";

    std::cout << "练习 1 通过\n\n";
}

// ============================================
// 练习 2 答案: memoize
// ============================================

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

    assert(memo(5) == 25);
    assert(call_count == 1);

    assert(memo(3) == 9);
    assert(call_count == 2);

    assert(memo(3) == 9);
    assert(call_count == 2);

    assert(memo(5) == 25);
    assert(call_count == 2);

    std::cout << "5次调用，实际计算: " << call_count << " 次\n";

    std::cout << "练习 2 通过\n\n";
}

// ============================================
// 练习 3 答案: 记忆化斐波那契
// ============================================

long long fib_naive(int n) {
    if (n <= 1) return n;
    return fib_naive(n - 1) + fib_naive(n - 2);
}

std::function<long long(int)> make_fib_memo() {
    auto cache = std::make_shared<std::unordered_map<int, long long>>();

    std::function<long long(int)> fib = [cache, &fib](int n) -> long long {
        if (n <= 1) return n;

        auto it = cache->find(n);
        if (it != cache->end()) {
            return it->second;
        }

        long long result = fib(n - 1) + fib(n - 2);
        (*cache)[n] = result;
        return result;
    };

    return fib;
}

void test_exercise_3() {
    std::cout << "=== 练习 3: 记忆化斐波那契 ===\n";

    auto fib_memo = make_fib_memo();

    assert(fib_memo(0) == 0);
    assert(fib_memo(1) == 1);
    assert(fib_memo(10) == 55);
    assert(fib_memo(20) == 6765);

    std::cout << "fib(10) = " << fib_memo(10) << "\n";
    std::cout << "fib(20) = " << fib_memo(20) << "\n";
    std::cout << "fib(40) = " << fib_memo(40) << "\n";

    std::cout << "练习 3 通过\n\n";
}

// ============================================
// 练习 4 答案: 惰性列表
// ============================================

template<typename T>
class LazyList {
public:
    using Generator = std::function<std::pair<T, LazyList>()>;

private:
    std::optional<Generator> gen_;

public:
    LazyList() : gen_(std::nullopt) {}
    explicit LazyList(Generator gen) : gen_(std::move(gen)) {}

    [[nodiscard]] bool is_empty() const { return !gen_.has_value(); }

    [[nodiscard]] T head() const {
        if (!gen_) throw std::runtime_error("Empty list");
        return (*gen_)().first;
    }

    [[nodiscard]] LazyList tail() const {
        if (!gen_) throw std::runtime_error("Empty list");
        return (*gen_)().second;
    }

    [[nodiscard]] std::vector<T> take(int n) const {
        std::vector<T> result;
        LazyList current = *this;

        for (int i = 0; i < n && !current.is_empty(); ++i) {
            auto [h, t] = (*current.gen_)();
            result.push_back(h);
            current = t;
        }

        return result;
    }

    static LazyList iterate(T start, std::function<T(T)> next) {
        return LazyList([start, next]() {
            return std::make_pair(start, iterate(next(start), next));
        });
    }
};

void test_exercise_4() {
    std::cout << "=== 练习 4: 惰性列表 ===\n";

    auto naturals = LazyList<int>::iterate(0, [](int n) { return n + 1; });

    auto first5 = naturals.take(5);
    assert(first5.size() == 5);
    assert(first5[0] == 0);
    assert(first5[4] == 4);

    std::cout << "自然数前5个: ";
    for (int n : first5) std::cout << n << " ";
    std::cout << "\n";

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
// 练习 5 答案: 配置惰性加载
// ============================================

class Config {
    Lazy<std::string> host_;
    Lazy<int> port_;
    Lazy<bool> debug_;

    mutable int load_count_ = 0;

public:
    Config()
        : host_(lazy([this] {
              load_count_++;
              return std::string("localhost");
          }))
        , port_(lazy([this] {
              load_count_++;
              return 8080;
          }))
        , debug_(lazy([this] {
              load_count_++;
              return false;
          })) {}

    [[nodiscard]] const std::string& host() const {
        return host_.get();
    }

    [[nodiscard]] int port() const {
        return port_.get();
    }

    [[nodiscard]] bool debug() const {
        return debug_.get();
    }

    [[nodiscard]] int load_count() const { return load_count_; }
};

void test_exercise_5() {
    std::cout << "=== 练习 5: 配置惰性加载 ===\n";

    Config config;

    assert(config.load_count() == 0);
    std::cout << "创建后加载次数: " << config.load_count() << "\n";

    std::string h = config.host();
    assert(config.load_count() == 1);
    std::cout << "访问 host 后: " << config.load_count() << "\n";

    h = config.host();
    assert(config.load_count() == 1);
    std::cout << "再次访问 host: " << config.load_count() << "\n";

    int p = config.port();
    assert(config.load_count() == 2);
    std::cout << "访问 port 后: " << config.load_count() << "\n";

    std::cout << "debug 未访问，load_count 仍为: " << config.load_count() << "\n";

    std::cout << "host=" << h << ", port=" << p << "\n";
    std::cout << "练习 5 通过\n\n";
}

// ============================================
// main
// ============================================

int main() {
    std::cout << "========================================\n";
    std::cout << "    第11章练习：惰性求值与记忆化 (参考答案)\n";
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
