/**
 * @file 01_functors_monads.cpp
 * @brief Functor 与 Monad 示例
 *
 * 本文件演示：
 * - Functor 概念与实现
 * - Monad 概念与实现
 * - Maybe Monad
 * - Result Monad
 * - 链式组合操作
 */

#include <cmath>
#include <functional>
#include <iostream>
#include <optional>
#include <sstream>
#include <string>
#include <variant>
#include <vector>

// ============================================
// 1. Box Functor - 最简单的容器
// ============================================

template<typename T>
class Box {
    T value_;

public:
    explicit Box(T value) : value_(std::move(value)) {}

    [[nodiscard]] const T& get() const { return value_; }

    // Functor: fmap
    template<typename F>
    [[nodiscard]] auto fmap(F f) const {
        using U = std::invoke_result_t<F, const T&>;
        return Box<U>(f(value_));
    }
};

void box_functor_demo() {
    std::cout << "=== Box Functor ===\n";

    Box<int> box(42);
    std::cout << "原始值: " << box.get() << "\n";

    // fmap: int -> int
    auto doubled = box.fmap([](int x) { return x * 2; });
    std::cout << "乘2: " << doubled.get() << "\n";

    // fmap: int -> string (类型转换)
    auto stringed = doubled.fmap([](int x) { return "Value: " + std::to_string(x); });
    std::cout << "转字符串: " << stringed.get() << "\n";

    // 链式调用
    auto result = Box<int>(10)
        .fmap([](int x) { return x + 5; })      // 15
        .fmap([](int x) { return x * 2; })      // 30
        .fmap([](int x) { return std::to_string(x) + "!"; });  // "30!"
    std::cout << "链式: " << result.get() << "\n";

    std::cout << "\n";
}

// ============================================
// 2. 验证 Functor 法则
// ============================================

void functor_laws_demo() {
    std::cout << "=== Functor 法则 ===\n";

    Box<int> box(42);

    // 恒等律: fmap(id) == id
    auto identity = [](auto x) { return x; };
    auto after_id = box.fmap(identity);
    std::cout << "恒等律: fmap(id)(" << box.get() << ") = " << after_id.get()
              << (box.get() == after_id.get() ? " ✓" : " ✗") << "\n";

    // 组合律: fmap(f . g) == fmap(f) . fmap(g)
    auto f = [](int x) { return x * 2; };
    auto g = [](int x) { return x + 10; };

    // f . g (先 g 后 f)
    auto composed = box.fmap([&](int x) { return f(g(x)); });

    // fmap(f) . fmap(g)
    auto chained = box.fmap(g).fmap(f);

    std::cout << "组合律: fmap(f∘g) = " << composed.get()
              << ", fmap(f)(fmap(g)) = " << chained.get()
              << (composed.get() == chained.get() ? " ✓" : " ✗") << "\n";

    std::cout << "\n";
}

// ============================================
// 3. Maybe Monad
// ============================================

template<typename T>
class Maybe {
    std::optional<T> value_;

    Maybe() = default;

public:
    using value_type = T;

    // 工厂函数
    [[nodiscard]] static Maybe just(T value) {
        Maybe m;
        m.value_ = std::move(value);
        return m;
    }

    [[nodiscard]] static Maybe nothing() {
        return Maybe{};
    }

    // 查询
    [[nodiscard]] bool has_value() const { return value_.has_value(); }
    [[nodiscard]] const T& get() const { return *value_; }
    [[nodiscard]] T get_or(T default_value) const {
        return value_.value_or(std::move(default_value));
    }

    // Functor: fmap
    template<typename F>
    [[nodiscard]] auto fmap(F f) const {
        using U = std::invoke_result_t<F, const T&>;
        if (has_value()) {
            return Maybe<U>::just(f(*value_));
        }
        return Maybe<U>::nothing();
    }

    // Monad: bind (也叫 flatMap, and_then)
    template<typename F>
    [[nodiscard]] auto bind(F f) const {
        using MaybeU = std::invoke_result_t<F, const T&>;
        if (has_value()) {
            return f(*value_);
        }
        return MaybeU::nothing();
    }

    // Monad: return (就是 just)
    // 用于将普通值提升到 Maybe 上下文
};

// 辅助函数
template<typename T>
std::ostream& operator<<(std::ostream& os, const Maybe<T>& m) {
    if (m.has_value()) {
        os << "Just(" << m.get() << ")";
    } else {
        os << "Nothing";
    }
    return os;
}

// 示例函数
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

[[nodiscard]] Maybe<int> safe_divide(int a, int b) {
    if (b == 0) return Maybe<int>::nothing();
    return Maybe<int>::just(a / b);
}

void maybe_monad_demo() {
    std::cout << "=== Maybe Monad ===\n";

    // fmap vs bind 的区别
    auto m = Maybe<int>::just(16);

    // fmap: 普通函数
    auto fmapped = m.fmap([](int x) { return x * 2; });
    std::cout << "fmap (*2): " << fmapped << "\n";

    // bind: 返回 Maybe 的函数
    auto bound = m.bind([](int x) { return safe_sqrt(static_cast<double>(x)); });
    std::cout << "bind (safe_sqrt): " << bound << "\n";

    // 链式调用
    auto result = Maybe<std::string>::just("16")
        .bind([](const std::string& s) { return safe_parse_int(s); })
        .bind([](int x) { return safe_sqrt(static_cast<double>(x)); })
        .fmap([](double x) { return x * 2; });
    std::cout << "链式 parse->sqrt->*2: " << result << "\n";

    // 错误传播
    auto error_result = Maybe<std::string>::just("abc")
        .bind([](const std::string& s) { return safe_parse_int(s); })
        .bind([](int x) { return safe_sqrt(static_cast<double>(x)); });
    std::cout << "解析 'abc': " << error_result << "\n";

    // nothing 传播
    auto nothing_result = Maybe<int>::nothing()
        .bind([](int x) { return safe_sqrt(static_cast<double>(x)); })
        .fmap([](double x) { return x * 100; });
    std::cout << "Nothing 传播: " << nothing_result << "\n";

    std::cout << "\n";
}

// ============================================
// 4. 验证 Monad 法则
// ============================================

void monad_laws_demo() {
    std::cout << "=== Monad 法则 ===\n";

    auto f = [](int x) { return Maybe<int>::just(x * 2); };
    auto g = [](int x) { return Maybe<int>::just(x + 10); };
    auto pure = [](int x) { return Maybe<int>::just(x); };

    // 左单位元: return(a).bind(f) == f(a)
    int a = 5;
    auto left1 = Maybe<int>::just(a).bind(f);
    auto left2 = f(a);
    std::cout << "左单位元: just(5).bind(f) = " << left1
              << ", f(5) = " << left2
              << (left1.get() == left2.get() ? " ✓" : " ✗") << "\n";

    // 右单位元: m.bind(return) == m
    auto m = Maybe<int>::just(42);
    auto right = m.bind(pure);
    std::cout << "右单位元: m.bind(return) = " << right
              << ", m = " << m
              << (right.get() == m.get() ? " ✓" : " ✗") << "\n";

    // 结合律: m.bind(f).bind(g) == m.bind(x => f(x).bind(g))
    auto assoc1 = m.bind(f).bind(g);
    auto assoc2 = m.bind([&](int x) { return f(x).bind(g); });
    std::cout << "结合律: m.bind(f).bind(g) = " << assoc1
              << ", m.bind(x=>f(x).bind(g)) = " << assoc2
              << (assoc1.get() == assoc2.get() ? " ✓" : " ✗") << "\n";

    std::cout << "\n";
}

// ============================================
// 5. Result Monad
// ============================================

template<typename T, typename E>
class Result {
    std::variant<T, E> data_;

    Result() = default;

public:
    using value_type = T;
    using error_type = E;

    [[nodiscard]] static Result ok(T value) {
        Result r;
        r.data_.template emplace<0>(std::move(value));
        return r;
    }

    [[nodiscard]] static Result err(E error) {
        Result r;
        r.data_.template emplace<1>(std::move(error));
        return r;
    }

    [[nodiscard]] bool is_ok() const { return data_.index() == 0; }
    [[nodiscard]] bool is_err() const { return data_.index() == 1; }

    [[nodiscard]] const T& unwrap() const { return std::get<0>(data_); }
    [[nodiscard]] const E& unwrap_err() const { return std::get<1>(data_); }

    // Functor: map
    template<typename F>
    [[nodiscard]] auto map(F f) const {
        using U = std::invoke_result_t<F, const T&>;
        if (is_ok()) {
            return Result<U, E>::ok(f(std::get<0>(data_)));
        }
        return Result<U, E>::err(std::get<1>(data_));
    }

    // Monad: and_then / bind
    template<typename F>
    [[nodiscard]] auto and_then(F f) const {
        using NewResult = std::invoke_result_t<F, const T&>;
        if (is_ok()) {
            return f(std::get<0>(data_));
        }
        return NewResult::err(std::get<1>(data_));
    }
};

template<typename T, typename E>
std::ostream& operator<<(std::ostream& os, const Result<T, E>& r) {
    if (r.is_ok()) {
        os << "Ok(" << r.unwrap() << ")";
    } else {
        os << "Err(" << r.unwrap_err() << ")";
    }
    return os;
}

// Result 版本的安全函数
[[nodiscard]] Result<int, std::string> parse_int(const std::string& s) {
    try {
        size_t pos;
        int val = std::stoi(s, &pos);
        if (pos != s.length()) {
            return Result<int, std::string>::err("包含非数字字符");
        }
        return Result<int, std::string>::ok(val);
    } catch (...) {
        return Result<int, std::string>::err("无效数字格式");
    }
}

[[nodiscard]] Result<double, std::string> result_sqrt(double x) {
    if (x < 0) {
        return Result<double, std::string>::err("负数不能开方");
    }
    return Result<double, std::string>::ok(std::sqrt(x));
}

[[nodiscard]] Result<double, std::string> result_divide(double a, double b) {
    if (b == 0) {
        return Result<double, std::string>::err("除零错误");
    }
    return Result<double, std::string>::ok(a / b);
}

void result_monad_demo() {
    std::cout << "=== Result Monad ===\n";

    // 成功链
    auto success = parse_int("25")
        .and_then([](int x) { return result_sqrt(static_cast<double>(x)); })
        .map([](double x) { return x * 2; });
    std::cout << "parse(25)->sqrt->*2: " << success << "\n";

    // 错误在解析阶段
    auto parse_error = parse_int("abc")
        .and_then([](int x) { return result_sqrt(static_cast<double>(x)); });
    std::cout << "parse(abc)->sqrt: " << parse_error << "\n";

    // 错误在sqrt阶段
    auto sqrt_error = parse_int("-4")
        .and_then([](int x) { return result_sqrt(static_cast<double>(x)); });
    std::cout << "parse(-4)->sqrt: " << sqrt_error << "\n";

    // 复杂链
    auto complex = parse_int("100")
        .and_then([](int x) {
            return result_divide(static_cast<double>(x), 4.0);
        })
        .and_then([](double x) {
            return result_sqrt(x);
        })
        .map([](double x) { return x + 1; });
    std::cout << "100/4->sqrt->+1: " << complex << "\n";

    std::cout << "\n";
}

// ============================================
// 6. List Monad
// ============================================

template<typename T>
class List {
    std::vector<T> data_;

public:
    List() = default;
    explicit List(std::vector<T> data) : data_(std::move(data)) {}
    List(std::initializer_list<T> init) : data_(init) {}

    using value_type = T;

    [[nodiscard]] const std::vector<T>& data() const { return data_; }
    [[nodiscard]] auto begin() const { return data_.begin(); }
    [[nodiscard]] auto end() const { return data_.end(); }

    // Functor: fmap
    template<typename F>
    [[nodiscard]] auto fmap(F f) const {
        using U = std::invoke_result_t<F, const T&>;
        std::vector<U> result;
        result.reserve(data_.size());
        for (const auto& x : data_) {
            result.push_back(f(x));
        }
        return List<U>(std::move(result));
    }

    // Monad: bind (flatMap)
    template<typename F>
    [[nodiscard]] auto bind(F f) const {
        using ListU = std::invoke_result_t<F, const T&>;
        using U = typename ListU::value_type;
        std::vector<U> result;
        for (const auto& x : data_) {
            auto sub = f(x);
            result.insert(result.end(), sub.begin(), sub.end());
        }
        return List<U>(std::move(result));
    }

    // pure / return
    [[nodiscard]] static List pure(T value) {
        return List({std::move(value)});
    }
};

template<typename T>
std::ostream& operator<<(std::ostream& os, const List<T>& list) {
    os << "[";
    bool first = true;
    for (const auto& x : list) {
        if (!first) os << ", ";
        os << x;
        first = false;
    }
    os << "]";
    return os;
}

void list_monad_demo() {
    std::cout << "=== List Monad ===\n";

    List<int> nums{1, 2, 3};

    // fmap: 每个元素乘2
    auto doubled = nums.fmap([](int x) { return x * 2; });
    std::cout << "fmap (*2): " << doubled << "\n";

    // bind: 每个元素产生多个结果
    auto expanded = nums.bind([](int x) {
        return List<int>{x, x * 10};
    });
    std::cout << "bind (x -> [x, x*10]): " << expanded << "\n";

    // 笛卡尔积示例
    List<int> xs{1, 2};
    List<char> ys{'a', 'b'};

    auto pairs = xs.bind([&](int x) {
        return ys.fmap([x](char y) {
            return std::make_pair(x, y);
        });
    });

    std::cout << "笛卡尔积: [";
    bool first = true;
    for (const auto& [x, y] : pairs) {
        if (!first) std::cout << ", ";
        std::cout << "(" << x << "," << y << ")";
        first = false;
    }
    std::cout << "]\n";

    std::cout << "\n";
}

// ============================================
// 7. 实战：用户注册
// ============================================

struct User {
    std::string name;
    std::string email;
    int age;
};

std::ostream& operator<<(std::ostream& os, const User& u) {
    return os << "User{" << u.name << ", " << u.email << ", " << u.age << "}";
}

[[nodiscard]] Result<std::string, std::string> validate_name(const std::string& name) {
    if (name.empty()) {
        return Result<std::string, std::string>::err("名字不能为空");
    }
    if (name.length() < 2) {
        return Result<std::string, std::string>::err("名字至少2个字符");
    }
    return Result<std::string, std::string>::ok(name);
}

[[nodiscard]] Result<std::string, std::string> validate_email(const std::string& email) {
    if (email.find('@') == std::string::npos) {
        return Result<std::string, std::string>::err("邮箱必须包含@");
    }
    return Result<std::string, std::string>::ok(email);
}

[[nodiscard]] Result<int, std::string> validate_age(const std::string& age_str) {
    return parse_int(age_str)
        .and_then([](int age) -> Result<int, std::string> {
            if (age < 0 || age > 150) {
                return Result<int, std::string>::err("年龄无效");
            }
            return Result<int, std::string>::ok(age);
        });
}

[[nodiscard]] Result<User, std::string> register_user(
    const std::string& name,
    const std::string& email,
    const std::string& age_str
) {
    return validate_name(name)
        .and_then([&](const std::string& valid_name) {
            return validate_email(email)
                .and_then([&, valid_name](const std::string& valid_email) {
                    return validate_age(age_str)
                        .map([&, valid_name, valid_email](int age) {
                            return User{valid_name, valid_email, age};
                        });
                });
        });
}

void practical_demo() {
    std::cout << "=== 实战：用户注册 ===\n";

    // 成功注册
    auto r1 = register_user("Alice", "alice@example.com", "25");
    std::cout << "注册 Alice: " << r1 << "\n";

    // 名字错误
    auto r2 = register_user("", "bob@test.com", "30");
    std::cout << "注册空名: " << r2 << "\n";

    // 邮箱错误
    auto r3 = register_user("Bob", "invalid-email", "30");
    std::cout << "注册无效邮箱: " << r3 << "\n";

    // 年龄错误
    auto r4 = register_user("Charlie", "charlie@test.com", "abc");
    std::cout << "注册无效年龄: " << r4 << "\n";

    std::cout << "\n";
}

// ============================================
// main
// ============================================

int main() {
    box_functor_demo();
    functor_laws_demo();
    maybe_monad_demo();
    monad_laws_demo();
    result_monad_demo();
    list_monad_demo();
    practical_demo();

    return 0;
}
