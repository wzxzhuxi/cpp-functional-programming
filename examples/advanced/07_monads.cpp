#include <cmath>
#include <functional>
#include <iostream>
#include <optional>
#include <string>
#include <variant>
#include <vector>

// ============================================
// 1. Maybe Monad
// ============================================

template<typename T>
class Maybe {
public:
    std::optional<T> value;

    // Constructors
    Maybe() : value(std::nullopt) {}
    explicit Maybe(T v) : value(v) {}
    explicit Maybe(std::nullopt_t) : value(std::nullopt) {}

    // return / pure
    static Maybe just(T v) {
        return Maybe(v);
    }

    static Maybe nothing() {
        return Maybe(std::nullopt);
    }

    // bind / >>=
    template<typename F>
    auto bind(F f) const {
        using R = decltype(f(std::declval<T>()));
        if (!value) return R::nothing();
        return f(*value);
    }

    // fmap (Functor)
    template<typename F>
    auto fmap(F f) const {
        using NewT = decltype(f(std::declval<T>()));
        if (!value) return Maybe<NewT>::nothing();
        return Maybe<NewT>::just(f(*value));
    }

    // has_value
    bool has_value() const { return value.has_value(); }

    // get
    const T& get() const { return *value; }
};

// Maybe 辅助函数
Maybe<int> safe_divide(int a, int b) {
    if (b == 0) return Maybe<int>::nothing();
    return Maybe<int>::just(a / b);
}

Maybe<double> safe_sqrt(double x) {
    if (x < 0) return Maybe<double>::nothing();
    return Maybe<double>::just(std::sqrt(x));
}

void maybe_monad_demo() {
    std::cout << "=== Maybe Monad ===\n";

    // 成功的链式调用
    auto result1 = Maybe<int>::just(100)
        .bind([](int x) { return safe_divide(x, 5); })
        .bind([](int x) { return Maybe<double>::just(static_cast<double>(x)); })
        .bind(safe_sqrt);

    if (result1.has_value()) {
        std::cout << "100 / 5, then sqrt: " << result1.get() << "\n";
    }

    // 中途失败的链式调用
    auto result2 = Maybe<int>::just(100)
        .bind([](int x) { return safe_divide(x, 0); })  // 失败
        .bind([](int x) { return Maybe<double>::just(static_cast<double>(x)); })
        .bind(safe_sqrt);

    std::cout << "100 / 0 (failed): "
              << (result2.has_value() ? "Success" : "Nothing") << "\n";

    // 使用 fmap
    auto result3 = Maybe<int>::just(42)
        .fmap([](int x) { return x * 2; })
        .fmap([](int x) { return x + 10; });

    std::cout << "42 * 2 + 10 = " << result3.get() << "\n\n";
}

// ============================================
// 2. Result Monad
// ============================================

template<typename T, typename E>
class Result {
public:
    std::variant<T, E> data;

    bool is_ok() const { return data.index() == 0; }
    bool is_err() const { return data.index() == 1; }
    const T& unwrap() const { return std::get<0>(data); }
    const E& unwrap_err() const { return std::get<1>(data); }

    // return / pure
    static Result ok(T value) {
        return Result{std::variant<T, E>(std::in_place_index<0>, value)};
    }

    static Result err(E error) {
        return Result{std::variant<T, E>(std::in_place_index<1>, error)};
    }

    // bind / >>=
    template<typename F>
    auto bind(F f) const {
        using R = decltype(f(std::declval<T>()));
        if (!is_ok()) {
            return R::err(unwrap_err());
        }
        return f(unwrap());
    }

    // fmap
    template<typename F>
    auto fmap(F f) const {
        using NewT = decltype(f(std::declval<T>()));
        if (!is_ok()) {
            return Result<NewT, E>::err(unwrap_err());
        }
        return Result<NewT, E>::ok(f(unwrap()));
    }
};

// Result 辅助函数
Result<int, std::string> parse_int(const std::string& s) {
    try {
        return Result<int, std::string>::ok(std::stoi(s));
    } catch (...) {
        return Result<int, std::string>::err("Invalid integer: " + s);
    }
}

Result<double, std::string> to_celsius(int fahrenheit) {
    return Result<double, std::string>::ok((fahrenheit - 32) * 5.0 / 9.0);
}

Result<std::string, std::string> format_temp(double celsius) {
    return Result<std::string, std::string>::ok(
        std::to_string(static_cast<int>(celsius)) + "°C"
    );
}

void result_monad_demo() {
    std::cout << "=== Result Monad ===\n";

    // 成功的转换链
    auto result1 = parse_int("77")
        .bind(to_celsius)
        .bind(format_temp);

    if (result1.is_ok()) {
        std::cout << "77°F = " << result1.unwrap() << "\n";
    }

    // 失败的转换链
    auto result2 = parse_int("invalid")
        .bind(to_celsius)
        .bind(format_temp);

    if (result2.is_err()) {
        std::cout << "Error: " << result2.unwrap_err() << "\n";
    }

    // 使用 fmap
    auto result3 = Result<int, std::string>::ok(10)
        .fmap([](int x) { return x * 2; })
        .fmap([](int x) { return x + 5; });

    std::cout << "10 * 2 + 5 = " << result3.unwrap() << "\n\n";
}

// ============================================
// 3. List Monad
// ============================================

template<typename T>
class List {
public:
    std::vector<T> items;

    List() = default;
    List(std::vector<T> v) : items(v) {}

    // return / pure
    static List pure(T value) {
        return List{std::vector<T>{value}};
    }

    // bind / >>=
    template<typename F>
    auto bind(F f) const {
        using R = decltype(f(std::declval<T>()));
        using NewT = typename decltype(R::items)::value_type;

        std::vector<NewT> result;
        for (const auto& item : items) {
            auto r = f(item);
            result.insert(result.end(), r.items.begin(), r.items.end());
        }
        return List<NewT>{result};
    }

    // fmap
    template<typename F>
    auto fmap(F f) const {
        using NewT = decltype(f(std::declval<T>()));
        std::vector<NewT> result;
        result.reserve(items.size());
        for (const auto& item : items) {
            result.push_back(f(item));
        }
        return List<NewT>{result};
    }
};

void list_monad_demo() {
    std::cout << "=== List Monad ===\n";

    // 生成所有组合
    List<int> numbers{{1, 2, 3}};
    List<std::string> letters{{"a", "b"}};

    auto combinations = numbers.bind([&letters](int n) {
        return letters.fmap([n](const std::string& s) {
            return std::to_string(n) + s;
        });
    });

    std::cout << "Combinations: ";
    for (const auto& item : combinations.items) {
        std::cout << item << " ";
    }
    std::cout << "\n";

    // List 的 flatMap 效果
    List<int> nums{{1, 2, 3}};
    auto expanded = nums.bind([](int x) {
        return List<int>{{x, x * 10, x * 100}};
    });

    std::cout << "Expanded: ";
    for (int x : expanded.items) {
        std::cout << x << " ";
    }
    std::cout << "\n\n";
}

// ============================================
// 4. IO Monad (简化版)
// ============================================

template<typename T>
class IO {
public:
    std::function<T()> action;

    explicit IO(std::function<T()> f) : action(f) {}

    // return / pure
    static IO pure(T value) {
        return IO([value]() { return value; });
    }

    // bind
    template<typename F>
    auto bind(F f) const -> decltype(f(std::declval<T>())) {
        using R = decltype(f(std::declval<T>()));
        return R([action = this->action, f]() {
            T result = action();
            return f(result).run();
        });
    }

    // fmap
    template<typename F>
    auto fmap(F f) const {
        using NewT = decltype(f(std::declval<T>()));
        return IO<NewT>([action = this->action, f]() {
            return f(action());
        });
    }

    // 执行副作用
    T run() const {
        return action();
    }
};

// IO 辅助函数
IO<std::string> get_line() {
    return IO<std::string>([]() {
        std::string line;
        std::getline(std::cin, line);
        return line;
    });
}

IO<int> put_line(const std::string& s) {
    return IO<int>([s]() {
        std::cout << s << "\n";
        return 0;
    });
}

void io_monad_demo() {
    std::cout << "=== IO Monad ===\n";

    // 纯值转换（不执行）
    auto program = IO<int>::pure(42)
        .fmap([](int x) { return x * 2; })
        .fmap([](int x) { return x + 10; });

    std::cout << "Pure computation (42 * 2 + 10): " << program.run() << "\n";

    // 组合 I/O 操作
    std::cout << "Enter a number: ";
    auto input_program = get_line()
        .fmap([](const std::string& s) {
            try {
                return std::stoi(s);
            } catch (...) {
                return 0;
            }
        })
        .fmap([](int x) { return x * 2; })
        .bind([](int x) {
            return put_line("Result: " + std::to_string(x))
                .fmap([x](int) { return x; });
        });

    input_program.run();
    std::cout << "\n";
}

// ============================================
// 5. Monad Laws 验证
// ============================================

void monad_laws_demo() {
    std::cout << "=== Monad Laws ===\n";

    // Left Identity: return(a).bind(f) == f(a)
    auto f = [](int x) { return Maybe<int>::just(x * 2); };
    int a = 5;

    auto left1 = Maybe<int>::just(a).bind(f);
    auto left2 = f(a);

    std::cout << "Left Identity: "
              << (left1.get() == left2.get() ? "✓" : "✗") << "\n";

    // Right Identity: m.bind(return) == m
    auto m = Maybe<int>::just(10);
    auto right1 = m.bind([](int x) { return Maybe<int>::just(x); });
    auto right2 = m;

    std::cout << "Right Identity: "
              << (right1.get() == right2.get() ? "✓" : "✗") << "\n";

    // Associativity: m.bind(f).bind(g) == m.bind(x => f(x).bind(g))
    auto g = [](int x) { return Maybe<int>::just(x + 10); };

    auto assoc1 = m.bind(f).bind(g);
    auto assoc2 = m.bind([f, g](int x) { return f(x).bind(g); });

    std::cout << "Associativity: "
              << (assoc1.get() == assoc2.get() ? "✓" : "✗") << "\n\n";
}

// ============================================
// 6. 实际应用：配置验证链
// ============================================

struct Config {
    std::string host;
    int port;
    std::string username;
};

Result<Config, std::string> validate_host(const Config& cfg) {
    if (cfg.host.empty()) {
        return Result<Config, std::string>::err("Host cannot be empty");
    }
    return Result<Config, std::string>::ok(cfg);
}

Result<Config, std::string> validate_port(const Config& cfg) {
    if (cfg.port < 1 || cfg.port > 65535) {
        return Result<Config, std::string>::err("Invalid port");
    }
    return Result<Config, std::string>::ok(cfg);
}

Result<Config, std::string> validate_username(const Config& cfg) {
    if (cfg.username.length() < 3) {
        return Result<Config, std::string>::err("Username too short");
    }
    return Result<Config, std::string>::ok(cfg);
}

void validation_pipeline_demo() {
    std::cout << "=== Validation Pipeline ===\n";

    Config valid_cfg{"localhost", 8080, "alice"};
    auto result1 = Result<Config, std::string>::ok(valid_cfg)
        .bind(validate_host)
        .bind(validate_port)
        .bind(validate_username);

    std::cout << "Valid config: "
              << (result1.is_ok() ? "✓" : "✗ " + result1.unwrap_err()) << "\n";

    Config invalid_cfg{"", 8080, "alice"};
    auto result2 = Result<Config, std::string>::ok(invalid_cfg)
        .bind(validate_host)
        .bind(validate_port)
        .bind(validate_username);

    std::cout << "Invalid config: "
              << (result2.is_ok() ? "✓" : "✗ " + result2.unwrap_err()) << "\n\n";
}

// ============================================
// Main
// ============================================

int main() {
    maybe_monad_demo();
    result_monad_demo();
    list_monad_demo();
    io_monad_demo();
    monad_laws_demo();
    validation_pipeline_demo();

    return 0;
}
