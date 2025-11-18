#include <cmath>
#include <functional>
#include <iostream>
#include <optional>
#include <string>
#include <variant>
#include <vector>

// ============================================
// Exercise 1: Maybe Monad
// ============================================

template<typename T>
class Maybe {
public:
    std::optional<T> value;

    Maybe() : value(std::nullopt) {}
    explicit Maybe(T v) : value(v) {}
    explicit Maybe(std::nullopt_t) : value(std::nullopt) {}

    static Maybe just(T v) {
        return Maybe(v);
    }

    static Maybe nothing() {
        return Maybe(std::nullopt);
    }

    template<typename F>
    auto bind(F f) const {
        using R = decltype(f(std::declval<T>()));
        if (!value) return R::nothing();
        return f(*value);
    }

    template<typename F>
    auto fmap(F f) const {
        using NewT = decltype(f(std::declval<T>()));
        if (!value) return Maybe<NewT>::nothing();
        return Maybe<NewT>::just(f(*value));
    }

    bool has_value() const { return value.has_value(); }
    const T& get() const { return *value; }
};

Maybe<int> safe_divide(int a, int b) {
    if (b == 0) return Maybe<int>::nothing();
    return Maybe<int>::just(a / b);
}

void exercise1_test() {
    std::cout << "=== Exercise 1: Maybe Monad ===\n";

    // Test case 1: Chain operations
    auto result1 = Maybe<int>::just(100)
        .bind([](int x) { return safe_divide(x, 5); })
        .bind([](int x) { return Maybe<int>::just(x * 2); });

    if (result1.has_value()) {
        std::cout << "100 / 5 * 2 = " << result1.get() << "\n";
    }

    // Test case 2: Failure case
    auto result2 = Maybe<int>::just(100)
        .bind([](int x) { return safe_divide(x, 0); })
        .bind([](int x) { return Maybe<int>::just(x * 2); });

    std::cout << "100 / 0 (should fail): "
              << (result2.has_value() ? "Has value" : "Nothing") << "\n";

    // Test case 3: fmap
    auto result3 = Maybe<int>::just(10)
        .fmap([](int x) { return x * 2; })
        .fmap([](int x) { return x + 5; });

    if (result3.has_value()) {
        std::cout << "10 * 2 + 5 = " << result3.get() << "\n";
    }
    std::cout << "\n";
}

// ============================================
// Exercise 2: Result Monad
// ============================================

template<typename T, typename E>
class Result {
public:
    std::variant<T, E> data;

    bool is_ok() const { return data.index() == 0; }
    bool is_err() const { return data.index() == 1; }
    const T& unwrap() const { return std::get<0>(data); }
    const E& unwrap_err() const { return std::get<1>(data); }

    static Result ok(T value) {
        return Result{std::variant<T, E>(std::in_place_index<0>, value)};
    }

    static Result err(E error) {
        return Result{std::variant<T, E>(std::in_place_index<1>, error)};
    }

    template<typename F>
    auto bind(F f) const {
        using R = decltype(f(std::declval<T>()));
        if (!is_ok()) {
            return R::err(unwrap_err());
        }
        return f(unwrap());
    }

    template<typename F>
    auto fmap(F f) const {
        using NewT = decltype(f(std::declval<T>()));
        if (!is_ok()) {
            return Result<NewT, E>::err(unwrap_err());
        }
        return Result<NewT, E>::ok(f(unwrap()));
    }
};

Result<int, std::string> parse_int(const std::string& s) {
    try {
        return Result<int, std::string>::ok(std::stoi(s));
    } catch (...) {
        return Result<int, std::string>::err("Invalid integer: " + s);
    }
}

Result<int, std::string> validate_positive(int x) {
    if (x <= 0) {
        return Result<int, std::string>::err("Number must be positive");
    }
    return Result<int, std::string>::ok(x);
}

void exercise2_test() {
    std::cout << "=== Exercise 2: Result Monad ===\n";

    // Test case 1: Success chain
    auto result1 = parse_int("42")
        .bind(validate_positive)
        .fmap([](int x) { return x * 2; });

    if (result1.is_ok()) {
        std::cout << "Valid positive number * 2: " << result1.unwrap() << "\n";
    } else {
        std::cout << "Error: " << result1.unwrap_err() << "\n";
    }

    // Test case 2: Parse failure
    auto result2 = parse_int("invalid")
        .bind(validate_positive)
        .fmap([](int x) { return x * 2; });

    if (result2.is_err()) {
        std::cout << "Parse error: " << result2.unwrap_err() << "\n";
    }

    // Test case 3: Validation failure
    auto result3 = parse_int("-10")
        .bind(validate_positive)
        .fmap([](int x) { return x * 2; });

    if (result3.is_err()) {
        std::cout << "Validation error: " << result3.unwrap_err() << "\n";
    }
    std::cout << "\n";
}

// ============================================
// Exercise 3: List Monad
// ============================================

template<typename T>
class List {
public:
    std::vector<T> items;

    List() = default;
    List(std::vector<T> v) : items(v) {}

    static List pure(T value) {
        return List{std::vector<T>{value}};
    }

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

void exercise3_test() {
    std::cout << "=== Exercise 3: List Monad ===\n";

    // Test case 1: Generate pairs
    List<int> numbers{{1, 2, 3}};
    List<char> letters{{'a', 'b'}};

    auto pairs = numbers.bind([&letters](int n) {
        return letters.fmap([n](char c) {
            return std::make_pair(n, c);
        });
    });

    std::cout << "Pairs: ";
    for (const auto& [n, c] : pairs.items) {
        std::cout << "(" << n << "," << c << ") ";
    }
    std::cout << "\n";

    // Test case 2: Expand numbers
    List<int> nums{{1, 2, 3}};
    auto expanded = nums.bind([](int x) {
        return List<int>{{x, x * 10}};
    });

    std::cout << "Expanded: ";
    for (int x : expanded.items) {
        std::cout << x << " ";
    }
    std::cout << "\n";

    // Test case 3: Filter and transform
    auto filtered = nums
        .fmap([](int x) { return x * 2; })
        .bind([](int x) {
            if (x > 3) {
                return List<int>{{x}};
            }
            return List<int>{};
        });

    std::cout << "Filtered (doubled > 3): ";
    for (int x : filtered.items) {
        std::cout << x << " ";
    }
    std::cout << "\n\n";
}

// ============================================
// Exercise 4: Validation Pipeline
// ============================================

struct UserData {
    std::string username;
    std::string email;
    int age;
};

Result<UserData, std::string> validate_username(const UserData& user) {
    if (user.username.length() < 3) {
        return Result<UserData, std::string>::err(
            "Username must be at least 3 characters"
        );
    }
    return Result<UserData, std::string>::ok(user);
}

Result<UserData, std::string> validate_email(const UserData& user) {
    if (user.email.find('@') == std::string::npos) {
        return Result<UserData, std::string>::err(
            "Email must contain @"
        );
    }
    return Result<UserData, std::string>::ok(user);
}

Result<UserData, std::string> validate_age(const UserData& user) {
    if (user.age < 13 || user.age > 120) {
        return Result<UserData, std::string>::err(
            "Age must be between 13 and 120"
        );
    }
    return Result<UserData, std::string>::ok(user);
}

void exercise4_test() {
    std::cout << "=== Exercise 4: Validation Pipeline ===\n";

    // Test case 1: Valid user
    UserData valid_user{"alice", "alice@example.com", 25};
    auto result1 = Result<UserData, std::string>::ok(valid_user)
        .bind(validate_username)
        .bind(validate_email)
        .bind(validate_age);

    if (result1.is_ok()) {
        std::cout << "Valid user: " << result1.unwrap().username << "\n";
    } else {
        std::cout << "Error: " << result1.unwrap_err() << "\n";
    }

    // Test case 2: Invalid username
    UserData invalid_user1{"ab", "test@example.com", 25};
    auto result2 = Result<UserData, std::string>::ok(invalid_user1)
        .bind(validate_username)
        .bind(validate_email)
        .bind(validate_age);

    if (result2.is_err()) {
        std::cout << "Invalid username error: " << result2.unwrap_err() << "\n";
    }

    // Test case 3: Invalid email
    UserData invalid_user2{"alice", "invalidemail", 25};
    auto result3 = Result<UserData, std::string>::ok(invalid_user2)
        .bind(validate_username)
        .bind(validate_email)
        .bind(validate_age);

    if (result3.is_err()) {
        std::cout << "Invalid email error: " << result3.unwrap_err() << "\n";
    }

    // Test case 4: Invalid age
    UserData invalid_user3{"alice", "alice@example.com", 150};
    auto result4 = Result<UserData, std::string>::ok(invalid_user3)
        .bind(validate_username)
        .bind(validate_email)
        .bind(validate_age);

    if (result4.is_err()) {
        std::cout << "Invalid age error: " << result4.unwrap_err() << "\n";
    }
    std::cout << "\n";
}

// ============================================
// Exercise 5: Monad Laws Verification
// ============================================

void exercise5_test() {
    std::cout << "=== Exercise 5: Monad Laws ===\n";

    // Left identity: return(a).bind(f) == f(a)
    auto f = [](int x) { return Maybe<int>::just(x * 2); };
    int a = 5;

    auto left1 = Maybe<int>::just(a).bind(f);
    auto left2 = f(a);

    bool left_identity = (left1.has_value() && left2.has_value() &&
                         left1.get() == left2.get());
    std::cout << "Left Identity: " << (left_identity ? "✓" : "✗") << "\n";

    // Right identity: m.bind(return) == m
    auto m = Maybe<int>::just(10);
    auto right1 = m.bind([](int x) { return Maybe<int>::just(x); });
    auto right2 = m;

    bool right_identity = (right1.has_value() && right2.has_value() &&
                          right1.get() == right2.get());
    std::cout << "Right Identity: " << (right_identity ? "✓" : "✗") << "\n";

    // Associativity: m.bind(f).bind(g) == m.bind(x => f(x).bind(g))
    auto g = [](int x) { return Maybe<int>::just(x + 10); };

    auto assoc1 = m.bind(f).bind(g);
    auto assoc2 = m.bind([f, g](int x) { return f(x).bind(g); });

    bool associativity = (assoc1.has_value() && assoc2.has_value() &&
                         assoc1.get() == assoc2.get());
    std::cout << "Associativity: " << (associativity ? "✓" : "✗") << "\n\n";
}

// ============================================
// Main
// ============================================

int main() {
    exercise1_test();
    exercise2_test();
    exercise3_test();
    exercise4_test();
    exercise5_test();

    return 0;
}
