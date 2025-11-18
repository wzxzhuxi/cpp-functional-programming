#include <algorithm>
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
// 1. Lazy Evaluation
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

void lazy_evaluation_demo() {
    std::cout << "=== Lazy Evaluation ===\n";

    Lazy<int> expensive_calc([]() {
        std::cout << "Computing expensive calculation...\n";
        int sum = 0;
        for (int i = 0; i < 1000; ++i) {
            sum += i;
        }
        return sum;
    });

    std::cout << "Lazy value created (not computed yet)\n";
    std::cout << "Is evaluated: " << (expensive_calc.is_evaluated() ? "yes" : "no") << "\n";

    std::cout << "\nFirst force:\n";
    int result1 = expensive_calc.force();
    std::cout << "Result: " << result1 << "\n";
    std::cout << "Is evaluated: " << (expensive_calc.is_evaluated() ? "yes" : "no") << "\n";

    std::cout << "\nSecond force (using cache):\n";
    int result2 = expensive_calc.force();
    std::cout << "Result: " << result2 << "\n\n";
}

// ============================================
// 2. Infinite Range
// ============================================

class InfiniteRange {
    int current;

public:
    explicit InfiniteRange(int start = 0) : current(start) {}

    int next() { return current++; }

    template<typename P>
    int next_if(P predicate) {
        while (true) {
            int val = next();
            if (predicate(val)) return val;
        }
    }

    std::vector<int> take(int n) {
        std::vector<int> result;
        for (int i = 0; i < n; ++i) {
            result.push_back(next());
        }
        return result;
    }

    template<typename P>
    std::vector<int> take_while(int max, P predicate) {
        std::vector<int> result;
        for (int i = 0; i < max; ++i) {
            int val = next_if(predicate);
            result.push_back(val);
        }
        return result;
    }
};

void infinite_range_demo() {
    std::cout << "=== Infinite Range ===\n";

    InfiniteRange range1(0);
    auto first_10 = range1.take(10);
    std::cout << "First 10 numbers: ";
    for (int n : first_10) {
        std::cout << n << " ";
    }
    std::cout << "\n";

    InfiniteRange range2(0);
    auto first_10_evens = range2.take_while(10, [](int x) { return x % 2 == 0; });
    std::cout << "First 10 even numbers: ";
    for (int n : first_10_evens) {
        std::cout << n << " ";
    }
    std::cout << "\n";

    InfiniteRange range3(0);
    auto first_10_div3 = range3.take_while(10, [](int x) { return x % 3 == 0; });
    std::cout << "First 10 numbers divisible by 3: ";
    for (int n : first_10_div3) {
        std::cout << n << " ";
    }
    std::cout << "\n\n";
}

// ============================================
// 3. Memoization
// ============================================

void memoization_demo() {
    std::cout << "=== Memoization ===\n";

    std::map<int, long long> fib_cache;
    std::function<long long(int)> fib_cached = [&](int n) -> long long {
        if (n <= 1) return n;
        auto it = fib_cache.find(n);
        if (it != fib_cache.end()) return it->second;
        long long result = fib_cached(n - 1) + fib_cached(n - 2);
        fib_cache[n] = result;
        return result;
    };

    std::cout << "Fibonacci(30) with memoization: " << fib_cached(30) << "\n";
    std::cout << "Fibonacci(40) with memoization: " << fib_cached(40) << "\n";
    std::cout << "Cache size: " << fib_cache.size() << "\n\n";
}

// ============================================
// 4. Tail Recursion
// ============================================

int factorial_normal(int n) {
    if (n <= 1) return 1;
    return n * factorial_normal(n - 1);
}

int factorial_tail(int n, int acc = 1) {
    if (n <= 1) return acc;
    return factorial_tail(n - 1, n * acc);
}

int sum_normal(const std::vector<int>& vec, size_t index = 0) {
    if (index >= vec.size()) return 0;
    return vec[index] + sum_normal(vec, index + 1);
}

int sum_tail(const std::vector<int>& vec, size_t index = 0, int acc = 0) {
    if (index >= vec.size()) return acc;
    return sum_tail(vec, index + 1, acc + vec[index]);
}

void tail_recursion_demo() {
    std::cout << "=== Tail Recursion ===\n";

    std::cout << "Factorial(10) normal: " << factorial_normal(10) << "\n";
    std::cout << "Factorial(10) tail: " << factorial_tail(10) << "\n";

    std::vector<int> nums{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    std::cout << "Sum normal: " << sum_normal(nums) << "\n";
    std::cout << "Sum tail: " << sum_tail(nums) << "\n\n";
}

// ============================================
// 5. Trampoline
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

Bounce<long long> factorial_bounce(int n, long long acc = 1) {
    if (n <= 1) {
        return Bounce<long long>::done(acc);
    }
    return Bounce<long long>::more([=]() {
        return factorial_bounce(n - 1, n * acc);
    });
}

Bounce<int> sum_bounce(const std::vector<int>& vec, size_t index = 0, int acc = 0) {
    if (index >= vec.size()) {
        return Bounce<int>::done(acc);
    }
    return Bounce<int>::more([&vec, index, acc]() {
        return sum_bounce(vec, index + 1, acc + vec[index]);
    });
}

void trampoline_demo() {
    std::cout << "=== Trampoline ===\n";

    long long result1 = trampoline(factorial_bounce(20));
    std::cout << "Factorial(20) with trampoline: " << result1 << "\n";

    std::vector<int> nums{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    int result2 = trampoline(sum_bounce(nums));
    std::cout << "Sum with trampoline: " << result2 << "\n\n";
}

// ============================================
// 6. Continuation Passing Style
// ============================================

int add_normal(int a, int b) {
    return a + b;
}

void add_cps(int a, int b, std::function<void(int)> cont) {
    cont(a + b);
}

void calculate_cps(int x, std::function<void(int)> cont) {
    add_cps(x, 10, [cont](int r1) {
        add_cps(r1, 20, [cont](int r2) {
            add_cps(r2, 30, cont);
        });
    });
}

void continuation_demo() {
    std::cout << "=== Continuation Passing Style ===\n";

    std::cout << "Normal add: " << add_normal(3, 4) << "\n";

    std::cout << "CPS add: ";
    add_cps(3, 4, [](int result) {
        std::cout << result << "\n";
    });

    std::cout << "CPS chain (5 + 10 + 20 + 30): ";
    calculate_cps(5, [](int result) {
        std::cout << result << "\n";
    });
    std::cout << "\n";
}

// ============================================
// 7. Lens
// ============================================

template<typename S, typename A>
struct Lens {
    std::function<A(const S&)> getter;
    std::function<S(const S&, const A&)> setter;

    A get(const S& s) const { return getter(s); }

    S set(const S& s, const A& a) const { return setter(s, a); }

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

struct Address {
    std::string street;
    std::string city;
    int zipcode;
};

struct Person {
    std::string name;
    int age;
    Address address;
};

void lens_demo() {
    std::cout << "=== Lens ===\n";

    Lens<Person, std::string> name_lens{
        [](const Person& p) { return p.name; },
        [](const Person& p, const std::string& n) {
            Person new_p = p;
            new_p.name = n;
            return new_p;
        }
    };

    Lens<Person, Address> address_lens{
        [](const Person& p) { return p.address; },
        [](const Person& p, const Address& a) {
            Person new_p = p;
            new_p.address = a;
            return new_p;
        }
    };

    Lens<Address, std::string> city_lens{
        [](const Address& a) { return a.city; },
        [](const Address& a, const std::string& c) {
            Address new_a = a;
            new_a.city = c;
            return new_a;
        }
    };

    Lens<Address, int> zipcode_lens{
        [](const Address& a) { return a.zipcode; },
        [](const Address& a, int z) {
            Address new_a = a;
            new_a.zipcode = z;
            return new_a;
        }
    };

    Person alice{"Alice", 30, {"123 Main St", "Boston", 02101}};

    std::cout << "Original: " << alice.name << " lives in "
              << alice.address.city << " " << alice.address.zipcode << "\n";

    Person alice2 = name_lens.set(alice, "Alice Smith");
    std::cout << "After name change: " << alice2.name << "\n";

    auto person_city_lens = address_lens.compose(city_lens);
    Person alice3 = person_city_lens.set(alice, "New York");
    std::cout << "After city change: " << alice3.address.city << "\n";

    auto person_zipcode_lens = address_lens.compose(zipcode_lens);
    Person alice4 = person_zipcode_lens.modify(alice, [](int z) { return z + 1000; });
    std::cout << "After zipcode modify: " << alice4.address.zipcode << "\n\n";
}

// ============================================
// 8. Y Combinator
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

void y_combinator_demo() {
    std::cout << "=== Y Combinator ===\n";

    auto my_factorial = make_y([](auto self, int n) -> long long {
        if (n <= 1) return 1;
        return n * self(n - 1);
    });

    std::cout << "Factorial(10) with Y: " << my_factorial(10) << "\n";

    auto my_fib = make_y([](auto self, int n) -> long long {
        if (n <= 1) return n;
        return self(n - 1) + self(n - 2);
    });

    std::cout << "Fibonacci(15) with Y: " << my_fib(15) << "\n";

    auto my_sum = make_y([](auto self, const std::vector<int>& vec, size_t index) -> int {
        if (index >= vec.size()) return 0;
        return vec[index] + self(vec, index + 1);
    });

    std::vector<int> nums{1, 2, 3, 4, 5};
    std::cout << "Sum with Y: " << my_sum(nums, 0) << "\n\n";
}

// ============================================
// Main
// ============================================

int main() {
    lazy_evaluation_demo();
    infinite_range_demo();
    memoization_demo();
    tail_recursion_demo();
    trampoline_demo();
    continuation_demo();
    lens_demo();
    y_combinator_demo();

    return 0;
}
