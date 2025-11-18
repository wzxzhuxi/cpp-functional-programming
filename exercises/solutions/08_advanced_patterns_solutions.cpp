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
    mutable std::optional<T> cached;
    std::function<T()> thunk;

public:
    explicit Lazy(std::function<T()> f) : thunk(f) {}

    const T& force() const {
        if (!cached) {
            cached = thunk();
        }
        return *cached;
    }

    bool is_evaluated() const { return cached.has_value(); }
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

template<typename R, typename Arg>
std::function<R(Arg)> memoize(std::function<R(Arg)> func) {
    auto cache = std::make_shared<std::map<Arg, R>>();

    return [func, cache](Arg arg) -> R {
        auto it = cache->find(arg);
        if (it != cache->end()) {
            return it->second;
        }
        R result = func(arg);
        (*cache)[arg] = result;
        return result;
    };
}

void exercise2_test() {
    std::cout << "=== Exercise 2: Memoization ===\n";

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

int factorial_tail(int n, int acc = 1) {
    if (n <= 1) return acc;
    return factorial_tail(n - 1, n * acc);
}

std::vector<int> reverse_tail(const std::vector<int>& vec,
                              std::vector<int> acc = {},
                              size_t index = 0) {
    if (index >= vec.size()) return acc;
    acc.insert(acc.begin(), vec[index]);
    return reverse_tail(vec, acc, index + 1);
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
    while (!bounce.is_done()) {
        bounce = bounce.next();
    }
    return bounce.get_value();
}

Bounce<long long> fibonacci_bounce(int n, long long a = 0, long long b = 1) {
    if (n == 0) {
        return Bounce<long long>::done(a);
    }
    return Bounce<long long>::more([=]() {
        return fibonacci_bounce(n - 1, b, a + b);
    });
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

    A get(const S& s) const {
        return getter(s);
    }

    S set(const S& s, const A& a) const {
        return setter(s, a);
    }

    S modify(const S& s, std::function<A(const A&)> f) const {
        return set(s, f(get(s)));
    }

    template<typename B>
    Lens<S, B> compose(const Lens<A, B>& other) const {
        return Lens<S, B>{
            [this, other](const S& s) { return other.get(this->get(s)); },
            [this, other](const S& s, const B& b) {
                return this->set(s, other.set(this->get(s), b));
            }
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

    Lens<Employee, std::string> name_lens{
        [](const Employee& e) { return e.name; },
        [](const Employee& e, const std::string& n) {
            Employee new_e = e;
            new_e.name = n;
            return new_e;
        }
    };

    Lens<Employee, Company> company_lens{
        [](const Employee& e) { return e.company; },
        [](const Employee& e, const Company& c) {
            Employee new_e = e;
            new_e.company = c;
            return new_e;
        }
    };

    Lens<Company, std::string> company_name_lens{
        [](const Company& c) { return c.name; },
        [](const Company& c, const std::string& n) {
            Company new_c = c;
            new_c.name = n;
            return new_c;
        }
    };

    Lens<Employee, int> salary_lens{
        [](const Employee& e) { return e.salary; },
        [](const Employee& e, int s) {
            Employee new_e = e;
            new_e.salary = s;
            return new_e;
        }
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

    template<typename... Args>
    auto operator()(Args&&... args) const {
        return f(*this, std::forward<Args>(args)...);
    }
};

template<typename F>
Y<std::decay_t<F>> make_y(F&& f) {
    return {std::forward<F>(f)};
}

void exercise6_test() {
    std::cout << "=== Exercise 6: Y Combinator ===\n";

    auto factorial = make_y([](auto self, int n) -> int {
        if (n <= 1) return 1;
        return n * self(n - 1);
    });

    std::cout << "Factorial(5): " << factorial(5) << "\n";
    std::cout << "Factorial(10): " << factorial(10) << "\n";

    auto sum = make_y([](auto self, const std::vector<int>& vec, size_t idx) -> int {
        if (idx >= vec.size()) return 0;
        return vec[idx] + self(vec, idx + 1);
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
