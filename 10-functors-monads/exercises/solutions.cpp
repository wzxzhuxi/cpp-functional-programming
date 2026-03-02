/**
 * @file solutions.cpp
 * @brief 第10章练习题参考答案
 */

#include <cassert>
#include <cmath>
#include <functional>
#include <iostream>
#include <optional>
#include <string>
#include <variant>
#include <vector>

// ============================================
// 练习 1 答案: Identity Functor
// ============================================

template<typename T>
class Identity {
    T value_;

public:
    explicit Identity(T value) : value_(std::move(value)) {}

    [[nodiscard]] const T& get() const { return value_; }

    // Functor: fmap
    template<typename F>
    [[nodiscard]] auto fmap(F f) const {
        using U = std::invoke_result_t<F, const T&>;
        return Identity<U>(f(value_));
    }
};

void test_exercise_1() {
    std::cout << "=== 练习 1: Identity Functor ===\n";

    Identity<int> id(42);
    assert(id.get() == 42);

    auto doubled = id.fmap([](int x) { return x * 2; });
    assert(doubled.get() == 84);
    std::cout << "fmap (*2): " << doubled.get() << "\n";

    auto stringed = doubled.fmap([](int x) { return std::to_string(x); });
    assert(stringed.get() == "84");
    std::cout << "fmap (to_string): " << stringed.get() << "\n";

    // 验证 Functor 法则
    auto identity_fn = [](auto x) { return x; };
    auto after_id = id.fmap(identity_fn);
    assert(after_id.get() == id.get());
    std::cout << "恒等律验证通过\n";

    std::cout << "练习 1 通过\n\n";
}

// ============================================
// 练习 2 答案: Maybe Monad
// ============================================

template<typename T>
class Maybe {
    std::optional<T> value_;

    Maybe() = default;

public:
    using value_type = T;

    [[nodiscard]] static Maybe just(T value) {
        Maybe m;
        m.value_ = std::move(value);
        return m;
    }

    [[nodiscard]] static Maybe nothing() {
        return Maybe{};
    }

    [[nodiscard]] bool has_value() const { return value_.has_value(); }
    [[nodiscard]] const T& get() const { return *value_; }

    // Functor: fmap
    template<typename F>
    [[nodiscard]] auto fmap(F f) const {
        using U = std::invoke_result_t<F, const T&>;
        if (has_value()) {
            return Maybe<U>::just(f(*value_));
        }
        return Maybe<U>::nothing();
    }

    // Monad: bind
    template<typename F>
    [[nodiscard]] auto bind(F f) const {
        using MaybeU = std::invoke_result_t<F, const T&>;
        if (has_value()) {
            return f(*value_);
        }
        return MaybeU::nothing();
    }
};

[[nodiscard]] Maybe<int> safe_parse_int(const std::string& s) {
    try {
        size_t pos;
        int val = std::stoi(s, &pos);
        if (pos != s.length()) return Maybe<int>::nothing();
        return Maybe<int>::just(val);
    } catch (...) {
        return Maybe<int>::nothing();
    }
}

[[nodiscard]] Maybe<double> safe_sqrt(double x) {
    if (x < 0) return Maybe<double>::nothing();
    return Maybe<double>::just(std::sqrt(x));
}

void test_exercise_2() {
    std::cout << "=== 练习 2: Maybe Monad ===\n";

    // fmap 测试
    auto m1 = Maybe<int>::just(25);
    auto m2 = m1.fmap([](int x) { return x * 2; });
    assert(m2.has_value());
    assert(m2.get() == 50);
    std::cout << "fmap just(25) * 2 = " << m2.get() << "\n";

    auto m3 = Maybe<int>::nothing();
    auto m4 = m3.fmap([](int x) { return x * 2; });
    assert(!m4.has_value());
    std::cout << "fmap nothing = nothing\n";

    // bind 测试
    auto m5 = Maybe<std::string>::just("16")
        .bind([](const std::string& s) { return safe_parse_int(s); })
        .bind([](int x) { return safe_sqrt(static_cast<double>(x)); });
    assert(m5.has_value());
    assert(m5.get() == 4.0);
    std::cout << "parse '16' -> sqrt = " << m5.get() << "\n";

    auto m6 = Maybe<std::string>::just("abc")
        .bind([](const std::string& s) { return safe_parse_int(s); });
    assert(!m6.has_value());
    std::cout << "parse 'abc' = nothing\n";

    std::cout << "练习 2 通过\n\n";
}

// ============================================
// 练习 3 答案: 验证 Monad 法则
// ============================================

void test_exercise_3() {
    std::cout << "=== 练习 3: 验证 Monad 法则 ===\n";

    auto f = [](int x) { return Maybe<int>::just(x * 2); };
    auto g = [](int x) { return Maybe<int>::just(x + 10); };
    auto pure = [](int x) { return Maybe<int>::just(x); };

    // 左单位元: return(a).bind(f) == f(a)
    int a = 5;
    auto left1 = Maybe<int>::just(a).bind(f);
    auto left2 = f(a);
    assert(left1.get() == left2.get());
    std::cout << "左单位元: just(5).bind(f) = " << left1.get()
              << ", f(5) = " << left2.get() << " ✓\n";

    // 右单位元: m.bind(return) == m
    auto m = Maybe<int>::just(42);
    auto right = m.bind(pure);
    assert(right.get() == m.get());
    std::cout << "右单位元: m.bind(return) = " << right.get()
              << ", m = " << m.get() << " ✓\n";

    // 结合律: m.bind(f).bind(g) == m.bind(x => f(x).bind(g))
    auto assoc1 = m.bind(f).bind(g);
    auto assoc2 = m.bind([&](int x) { return f(x).bind(g); });
    assert(assoc1.get() == assoc2.get());
    std::cout << "结合律: m.bind(f).bind(g) = " << assoc1.get()
              << ", m.bind(x=>f(x).bind(g)) = " << assoc2.get() << " ✓\n";

    std::cout << "练习 3 通过\n\n";
}

// ============================================
// 练习 4 答案: Either Monad
// ============================================

template<typename L, typename R>
class Either {
    std::variant<L, R> data_;

    Either() = default;

public:
    using left_type = L;
    using right_type = R;

    [[nodiscard]] static Either left(L value) {
        Either e;
        e.data_.template emplace<0>(std::move(value));
        return e;
    }

    [[nodiscard]] static Either right(R value) {
        Either e;
        e.data_.template emplace<1>(std::move(value));
        return e;
    }

    [[nodiscard]] bool is_left() const { return data_.index() == 0; }
    [[nodiscard]] bool is_right() const { return data_.index() == 1; }

    [[nodiscard]] const L& get_left() const { return std::get<0>(data_); }
    [[nodiscard]] const R& get_right() const { return std::get<1>(data_); }

    // Functor: fmap (对 Right 值应用函数)
    template<typename F>
    [[nodiscard]] auto fmap(F f) const {
        using U = std::invoke_result_t<F, const R&>;
        if (is_right()) {
            return Either<L, U>::right(f(std::get<1>(data_)));
        }
        return Either<L, U>::left(std::get<0>(data_));
    }

    // Monad: bind (对 Right 值应用返回 Either 的函数)
    template<typename F>
    [[nodiscard]] auto bind(F f) const {
        using EitherU = std::invoke_result_t<F, const R&>;
        if (is_right()) {
            return f(std::get<1>(data_));
        }
        return EitherU::left(std::get<0>(data_));
    }
};

[[nodiscard]] Either<std::string, int> parse_either(const std::string& s) {
    try {
        size_t pos;
        int val = std::stoi(s, &pos);
        if (pos != s.length()) {
            return Either<std::string, int>::left("包含非数字字符");
        }
        return Either<std::string, int>::right(val);
    } catch (...) {
        return Either<std::string, int>::left("解析失败");
    }
}

[[nodiscard]] Either<std::string, double> sqrt_either(double x) {
    if (x < 0) {
        return Either<std::string, double>::left("负数不能开方");
    }
    return Either<std::string, double>::right(std::sqrt(x));
}

void test_exercise_4() {
    std::cout << "=== 练习 4: Either Monad ===\n";

    // fmap 测试
    auto e1 = Either<std::string, int>::right(25);
    auto e2 = e1.fmap([](int x) { return x * 2; });
    assert(e2.is_right());
    assert(e2.get_right() == 50);
    std::cout << "fmap right(25) * 2 = " << e2.get_right() << "\n";

    auto e3 = Either<std::string, int>::left("error");
    auto e4 = e3.fmap([](int x) { return x * 2; });
    assert(e4.is_left());
    assert(e4.get_left() == "error");
    std::cout << "fmap left(error) = left(error)\n";

    // bind 测试
    auto e5 = parse_either("16")
        .bind([](int x) { return sqrt_either(static_cast<double>(x)); });
    assert(e5.is_right());
    assert(e5.get_right() == 4.0);
    std::cout << "parse '16' -> sqrt = " << e5.get_right() << "\n";

    auto e6 = parse_either("-4")
        .bind([](int x) { return sqrt_either(static_cast<double>(x)); });
    assert(e6.is_left());
    std::cout << "parse '-4' -> sqrt = left(" << e6.get_left() << ")\n";

    std::cout << "练习 4 通过\n\n";
}

// ============================================
// 练习 5 答案: 组合操作
// ============================================

using CalcResult = Either<std::string, double>;

[[nodiscard]] CalcResult safe_divide(double a, double b) {
    if (b == 0) {
        return CalcResult::left("除零错误");
    }
    return CalcResult::right(a / b);
}

[[nodiscard]] CalcResult calculate(const std::string& a_str, const std::string& b_str) {
    return parse_either(a_str)
        .bind([&](int a) {
            return parse_either(b_str)
                .bind([a](int b) {
                    return safe_divide(static_cast<double>(a), static_cast<double>(b));
                });
        })
        .bind([](double x) {
            return sqrt_either(x);
        })
        .fmap([](double x) {
            return x * 2;
        });
}

void test_exercise_5() {
    std::cout << "=== 练习 5: 组合操作 ===\n";

    // sqrt(100 / 4) * 2 = sqrt(25) * 2 = 5 * 2 = 10
    auto r1 = calculate("100", "4");
    assert(r1.is_right());
    assert(std::abs(r1.get_right() - 10.0) < 0.0001);
    std::cout << "calculate(100, 4) = " << r1.get_right() << "\n";

    // 除零错误
    auto r2 = calculate("100", "0");
    assert(r2.is_left());
    std::cout << "calculate(100, 0) = " << r2.get_left() << "\n";

    // 负数开方错误
    auto r3 = calculate("100", "-4");
    assert(r3.is_left());
    std::cout << "calculate(100, -4) = " << r3.get_left() << "\n";

    // 解析错误
    auto r4 = calculate("abc", "4");
    assert(r4.is_left());
    std::cout << "calculate(abc, 4) = " << r4.get_left() << "\n";

    std::cout << "练习 5 通过\n\n";
}

// ============================================
// main
// ============================================

int main() {
    std::cout << "========================================\n";
    std::cout << "    第10章练习：Functor 与 Monad (参考答案)\n";
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
