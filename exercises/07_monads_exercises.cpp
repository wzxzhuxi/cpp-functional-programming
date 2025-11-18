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
        // TODO: Return a Maybe containing value v
        return Maybe();
    }

    static Maybe nothing() {
        // TODO: Return an empty Maybe
        return Maybe();
    }

    template<typename F>
    auto bind(F f) const {
        // TODO: Implement bind
        // If value is empty, return nothing
        // Otherwise, apply f to the value
        using R = decltype(f(std::declval<T>()));
        return R::nothing();
    }

    template<typename F>
    auto fmap(F f) const {
        // TODO: Implement fmap
        // If value is empty, return nothing
        // Otherwise, apply f to the value and wrap in Maybe
        using NewT = decltype(f(std::declval<T>()));
        return Maybe<NewT>::nothing();
    }

    bool has_value() const { return value.has_value(); }
    const T& get() const { return *value; }
};

// TODO: Implement safe_divide that returns Maybe
// Should return nothing if b == 0
Maybe<int> safe_divide(int a, int b) {
    return Maybe<int>::nothing();
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
        // TODO: Return a successful Result
        return Result{std::variant<T, E>(std::in_place_index<1>, E{})};
    }

    static Result err(E error) {
        // TODO: Return a failed Result
        return Result{std::variant<T, E>(std::in_place_index<1>, error)};
    }

    template<typename F>
    auto bind(F f) const {
        // TODO: Implement bind
        // If this is an error, propagate the error
        // Otherwise, apply f to the value
        using R = decltype(f(std::declval<T>()));
        return R::err(E{});
    }

    template<typename F>
    auto fmap(F f) const {
        // TODO: Implement fmap
        // If this is an error, propagate the error
        // Otherwise, apply f to the value and wrap in Result
        using NewT = decltype(f(std::declval<T>()));
        return Result<NewT, E>::err(E{});
    }
};

// TODO: Implement parse_int that returns Result<int, std::string>
// Should return error if string is not a valid integer
Result<int, std::string> parse_int(const std::string& s) {
    return Result<int, std::string>::err("Not implemented");
}

// TODO: Implement validate_positive that returns Result<int, std::string>
// Should return error if value is not positive
Result<int, std::string> validate_positive(int x) {
    return Result<int, std::string>::err("Not implemented");
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
        // TODO: Return a List containing a single value
        return List{};
    }

    template<typename F>
    auto bind(F f) const {
        // TODO: Implement bind (flatMap)
        // Apply f to each item and flatten the results
        using R = decltype(f(std::declval<T>()));
        using NewT = typename decltype(R::items)::value_type;
        return List<NewT>{};
    }

    template<typename F>
    auto fmap(F f) const {
        // TODO: Implement fmap
        // Apply f to each item
        using NewT = decltype(f(std::declval<T>()));
        return List<NewT>{};
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

// TODO: Implement validate_username
// Should return error if username is shorter than 3 characters
Result<UserData, std::string> validate_username(const UserData& user) {
    return Result<UserData, std::string>::err("Not implemented");
}

// TODO: Implement validate_email
// Should return error if email doesn't contain '@'
Result<UserData, std::string> validate_email(const UserData& user) {
    return Result<UserData, std::string>::err("Not implemented");
}

// TODO: Implement validate_age
// Should return error if age is not between 13 and 120
Result<UserData, std::string> validate_age(const UserData& user) {
    return Result<UserData, std::string>::err("Not implemented");
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

    // TODO: Verify left identity for Maybe monad
    // Law: return(a).bind(f) == f(a)
    auto f = [](int x) { return Maybe<int>::just(x * 2); };
    int a = 5;

    auto left1 = Maybe<int>::just(a).bind(f);
    auto left2 = f(a);

    bool left_identity = (left1.has_value() && left2.has_value() &&
                         left1.get() == left2.get());
    std::cout << "Left Identity: " << (left_identity ? "✓" : "✗") << "\n";

    // TODO: Verify right identity for Maybe monad
    // Law: m.bind(return) == m
    auto m = Maybe<int>::just(10);
    auto right1 = m.bind([](int x) { return Maybe<int>::just(x); });
    auto right2 = m;

    bool right_identity = (right1.has_value() && right2.has_value() &&
                          right1.get() == right2.get());
    std::cout << "Right Identity: " << (right_identity ? "✓" : "✗") << "\n";

    // TODO: Verify associativity for Maybe monad
    // Law: m.bind(f).bind(g) == m.bind(x => f(x).bind(g))
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
