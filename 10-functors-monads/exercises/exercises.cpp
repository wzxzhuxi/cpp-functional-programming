/**
 * @file exercises.cpp
 * @brief 第10章练习题：Functor 与 Monad
 *
 * 完成下面的练习，然后运行程序验证结果。
 * 参考答案在 solutions.cpp
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
// 练习 1: 实现 Identity Functor
// ============================================

/**
 * Identity 是最简单的 Functor，只是包装一个值
 *
 * 要求：
 * - 构造函数接受一个值
 * - get() 返回内部值
 * - fmap(f) 应用函数 f 到内部值，返回新的 Identity
 */
template<typename T>
class Identity {
    T value_;

public:
    explicit Identity(T value) : value_(std::move(value)) {}

    [[nodiscard]] const T& get() const { return value_; }

    // TODO: 实现 fmap
    template<typename F>
    [[nodiscard]] auto fmap(F f) const {
        using U = std::invoke_result_t<F, const T&>;
        (void)f;
        // 修改这行
        return Identity<U>(U{});
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
// 练习 2: 实现 Maybe Monad
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

    // TODO: 实现 fmap
    template<typename F>
    [[nodiscard]] auto fmap(F f) const {
        using U = std::invoke_result_t<F, const T&>;
        (void)f;
        // 修改这里
        return Maybe<U>::nothing();
    }

    // TODO: 实现 bind (flatMap / and_then)
    template<typename F>
    [[nodiscard]] auto bind(F f) const {
        using MaybeU = std::invoke_result_t<F, const T&>;
        (void)f;
        // 修改这里
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
// 练习 3: 验证 Monad 法则
// ============================================

void test_exercise_3() {
    std::cout << "=== 练习 3: 验证 Monad 法则 ===\n";

    auto f = [](int x) { return Maybe<int>::just(x * 2); };
    auto g = [](int x) { return Maybe<int>::just(x + 10); };
    auto pure = [](int x) { return Maybe<int>::just(x); };

    // TODO: 验证左单位元
    // return(a).bind(f) == f(a)
    int a = 5;
    auto left1 = Maybe<int>::just(a).bind(f);
    auto left2 = f(a);
    // 取消注释并验证
    // assert(left1.get() == left2.get());
    std::cout << "左单位元: just(5).bind(f) = " << left1.get()
              << ", f(5) = " << left2.get() << "\n";

    // TODO: 验证右单位元
    // m.bind(return) == m
    auto m = Maybe<int>::just(42);
    auto right = m.bind(pure);
    // 取消注释并验证
    // assert(right.get() == m.get());
    std::cout << "右单位元: m.bind(return) = " << right.get()
              << ", m = " << m.get() << "\n";

    // TODO: 验证结合律
    // m.bind(f).bind(g) == m.bind(x => f(x).bind(g))
    auto assoc1 = m.bind(f).bind(g);
    auto assoc2 = m.bind([&](int x) { return f(x).bind(g); });
    // 取消注释并验证
    // assert(assoc1.get() == assoc2.get());
    std::cout << "结合律: m.bind(f).bind(g) = " << assoc1.get()
              << ", m.bind(x=>f(x).bind(g)) = " << assoc2.get() << "\n";

    std::cout << "练习 3 通过\n\n";
}

// ============================================
// 练习 4: 实现 Either Monad
// ============================================

/**
 * Either<L, R> 类似 Result，但更通用
 * Left 通常表示错误，Right 表示成功
 */
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

    // TODO: 实现 fmap (对 Right 值应用函数)
    template<typename F>
    [[nodiscard]] auto fmap(F f) const {
        using U = std::invoke_result_t<F, const R&>;
        (void)f;
        // 修改这里
        return Either<L, U>::left(L{});
    }

    // TODO: 实现 bind (对 Right 值应用返回 Either 的函数)
    template<typename F>
    [[nodiscard]] auto bind(F f) const {
        using EitherU = std::invoke_result_t<F, const R&>;
        (void)f;
        // 修改这里
        return EitherU::left(L{});
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
// 练习 5: 使用 Monad 组合操作
// ============================================

/**
 * 实现一个计算器，链式处理用户输入
 *
 * 操作链：
 * 1. 解析两个数字
 * 2. 对第一个数除以第二个
 * 3. 对结果开平方
 * 4. 乘以2
 */

// 使用 Either 作为 Result
using CalcResult = Either<std::string, double>;

// TODO: 实现 safe_divide
[[nodiscard]] CalcResult safe_divide(double a, double b) {
    // 修改这里
    (void)a; (void)b;
    return CalcResult::left("Not implemented");
}

// TODO: 实现 calculate
// 1. 解析 a_str 和 b_str
// 2. 用 a 除以 b
// 3. 开平方
// 4. 乘以 2
[[nodiscard]] CalcResult calculate(const std::string& a_str, const std::string& b_str) {
    // 使用 bind 链式调用
    // 修改这里
    (void)a_str; (void)b_str;
    return CalcResult::left("Not implemented");
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
    std::cout << "    第10章练习：Functor 与 Monad\n";
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
