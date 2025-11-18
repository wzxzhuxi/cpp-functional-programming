#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <tuple>
#include <variant>
#include <vector>

// ============================================
// Exercise 1: Lazy Evaluation
// ============================================

template<typename T>
class Lazy {
    // TODO: Implement lazy evaluation
    // - Store a thunk (function that produces T)
    // - Cache the result after first evaluation
    // - Provide force() method to evaluate

public:
    explicit Lazy(std::function<T()> f) {}

    const T& force() const {
        static T dummy{};
        return dummy;
    }

    bool is_evaluated() const { return false; }
};

void exercise1_test() {
    std::cout << "=== Exercise 1: Lazy Evaluation ===\n";

    Lazy<int> lazy_sum([]() {
        std::cout << "Computing sum...\n";
        int sum = 0;
        for (int i = 0; i < 100; ++i) {
            sum += i;
        }
        return sum;
    });

    std::cout << "Created lazy value\n";
    std::cout << "Is evaluated: " << (lazy_sum.is_evaluated() ? "yes" : "no") << "\n";

    std::cout << "Forcing evaluation:\n";
    int result = lazy_sum.force();
    std::cout << "Result: " << result << "\n";

    std::cout << "Second force (should use cache):\n";
    int result2 = lazy_sum.force();
    std::cout << "Result: " << result2 << "\n\n";
}

// ============================================
// Exercise 2: Memoization
// ============================================

// TODO: Implement memoize function
// Should cache function results based on arguments
// Return a new function that uses the cache

template<typename R, typename Arg>
std::function<R(Arg)> memoize(std::function<R(Arg)> func) {
    return [func](Arg arg) -> R {
        return func(arg);
    };
}

void exercise2_test() {
    std::cout << "=== Exercise 2: Memoization ===\n";

    // Expensive function to memoize
    std::function<int(int)> expensive = [](int n) -> int {
        std::cout << "Computing for " << n << "...\n";
        int sum = 0;
        for (int i = 0; i < n; ++i) {
            sum += i;
        }
        return sum;
    };

    auto memoized = memoize<int, int>(expensive);

    std::cout << "First call with 10:\n";
    std::cout << "Result: " << memoized(10) << "\n";

    std::cout << "Second call with 10 (should use cache):\n";
    std::cout << "Result: " << memoized(10) << "\n";

    std::cout << "Call with 20:\n";
    std::cout << "Result: " << memoized(20) << "\n\n";
}

// ============================================
// Exercise 3: Tail Recursion
// ============================================

// TODO: Implement tail-recursive factorial
// Use an accumulator parameter
int factorial_tail(int n, int acc = 1) {
    return 1;
}

// TODO: Implement tail-recursive list reversal
// Use an accumulator parameter
std::vector<int> reverse_tail(const std::vector<int>& vec,
                              std::vector<int> acc = {},
                              size_t index = 0) {
    return {};
}

void exercise3_test() {
    std::cout << "=== Exercise 3: Tail Recursion ===\n";

    std::cout << "Factorial(5): " << factorial_tail(5) << "\n";
    std::cout << "Factorial(10): " << factorial_tail(10) << "\n";

    std::vector<int> nums{1, 2, 3, 4, 5};
    auto reversed = reverse_tail(nums);
    std::cout << "Reversed: ";
    for (int n : reversed) {
        std::cout << n << " ";
    }
    std::cout << "\n\n";
}

// ============================================
// Exercise 4: Trampoline
// ============================================

template<typename T>
struct Bounce {
    std::variant<T, std::function<Bounce<T>()>> data;

    bool is_done() const { return data.index() == 0; }
    T get_value() const { return std::get<0>(data); }
    Bounce<T> next() const { return std::get<1>(data)(); }

    static Bounce done(T value) {
        return Bounce{std::variant<T, std::function<Bounce<T>()>>(
            std::in_place_index<0>, value
        )};
    }

    static Bounce more(std::function<Bounce<T>()> f) {
        return Bounce{std::variant<T, std::function<Bounce<T>()>>(
            std::in_place_index<1>, f
        )};
    }
};

template<typename T>
T trampoline(Bounce<T> bounce) {
    // TODO: Implement trampoline
    // Keep calling next() until done
    return T{};
}

// TODO: Implement fibonacci using trampoline
Bounce<long long> fibonacci_bounce(int n, long long a = 0, long long b = 1) {
    // TODO: Implement using Bounce::done and Bounce::more
    return Bounce<long long>::done(0);
}

void exercise4_test() {
    std::cout << "=== Exercise 4: Trampoline ===\n";

    std::cout << "Fibonacci(10): " << trampoline(fibonacci_bounce(10)) << "\n";
    std::cout << "Fibonacci(20): " << trampoline(fibonacci_bounce(20)) << "\n";
    std::cout << "Fibonacci(30): " << trampoline(fibonacci_bounce(30)) << "\n\n";
}

// ============================================
// Exercise 5: Lens
// ============================================

template<typename S, typename A>
struct Lens {
    std::function<A(const S&)> getter;
    std::function<S(const S&, const A&)> setter;

    // TODO: Implement get
    A get(const S& s) const {
        return A{};
    }

    // TODO: Implement set
    S set(const S& s, const A& a) const {
        return S{};
    }

    // TODO: Implement modify
    // Should use get and set
    S modify(const S& s, std::function<A(const A&)> f) const {
        return S{};
    }

    // TODO: Implement compose
    // Combine two lenses into one
    template<typename B>
    Lens<S, B> compose(const Lens<A, B>& other) const {
        return Lens<S, B>{
            [](const S& s) { return B{}; },
            [](const S& s, const B& b) { return S{}; }
        };
    }
};

struct Company {
    std::string name;
    int employees;
};

struct Employee {
    std::string name;
    Company company;
    int salary;
};

void exercise5_test() {
    std::cout << "=== Exercise 5: Lens ===\n";

    Employee john{"John", {"TechCorp", 1000}, 50000};

    // TODO: Create lenses for Employee fields
    Lens<Employee, std::string> name_lens{
        [](const Employee& e) { return std::string{}; },
        [](const Employee& e, const std::string& n) { return Employee{}; }
    };

    Lens<Employee, Company> company_lens{
        [](const Employee& e) { return Company{}; },
        [](const Employee& e, const Company& c) { return Employee{}; }
    };

    Lens<Company, std::string> company_name_lens{
        [](const Company& c) { return std::string{}; },
        [](const Company& c, const std::string& n) { return Company{}; }
    };

    Lens<Employee, int> salary_lens{
        [](const Employee& e) { return 0; },
        [](const Employee& e, int s) { return Employee{}; }
    };

    // Test basic get/set
    std::cout << "Name: " << name_lens.get(john) << "\n";
    Employee john2 = name_lens.set(john, "John Smith");
    std::cout << "New name: " << name_lens.get(john2) << "\n";

    // Test modify
    Employee john3 = salary_lens.modify(john, [](int s) { return s + 5000; });
    std::cout << "New salary: " << salary_lens.get(john3) << "\n";

    // Test compose
    auto employee_company_name = company_lens.compose(company_name_lens);
    std::cout << "Company name: " << employee_company_name.get(john) << "\n";
    Employee john4 = employee_company_name.set(john, "NewCorp");
    std::cout << "New company name: " << employee_company_name.get(john4) << "\n\n";
}

// ============================================
// Exercise 6: Y Combinator
// ============================================

template<typename F>
struct Y {
    F f;

    // TODO: Implement operator() to enable recursion
    template<typename... Args>
    auto operator()(Args&&... args) const {
        // Hint: Call f with *this and forward args
        return typename std::result_of<F(Y, Args...)>::type{};
    }
};

template<typename F>
Y<std::decay_t<F>> make_y(F&& f) {
    return {std::forward<F>(f)};
}

void exercise6_test() {
    std::cout << "=== Exercise 6: Y Combinator ===\n";

    // TODO: Use make_y to create anonymous recursive factorial
    auto factorial = make_y([](auto self, int n) -> int {
        // TODO: Implement factorial using self for recursion
        return 1;
    });

    std::cout << "Factorial(5): " << factorial(5) << "\n";
    std::cout << "Factorial(10): " << factorial(10) << "\n";

    // TODO: Use make_y to create anonymous recursive sum
    auto sum = make_y([](auto self, const std::vector<int>& vec, size_t idx) -> int {
        // TODO: Implement sum using self for recursion
        return 0;
    });

    std::vector<int> nums{1, 2, 3, 4, 5};
    std::cout << "Sum: " << sum(nums, 0) << "\n\n";
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
    exercise6_test();

    return 0;
}
