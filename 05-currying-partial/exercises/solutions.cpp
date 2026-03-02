/**
 * @file solutions.cpp
 * @brief 第05章练习题参考答案
 */

#include <algorithm>
#include <cassert>
#include <cmath>
#include <functional>
#include <iostream>
#include <string>
#include <tuple>

// ============================================
// 练习 1: 实现 curry4 - 答案
// ============================================

template<typename F>
[[nodiscard]] auto curry4(F f) {
    return [f](auto a) {
        return [f, a](auto b) {
            return [f, a, b](auto c) {
                return [f, a, b, c](auto d) {
                    return f(a, b, c, d);
                };
            };
        };
    };
}

void test_exercise_1() {
    std::cout << "=== 练习 1: 实现 curry4 - 答案 ===\n";

    auto add4 = [](int a, int b, int c, int d) {
        return a + b + c + d;
    };

    auto curried = curry4(add4);

    assert(curried(1)(2)(3)(4) == 10);
    std::cout << "curry4(add4)(1)(2)(3)(4) = " << curried(1)(2)(3)(4) << "\n";

    // 部分应用
    auto add_1 = curried(1);
    auto add_1_2 = add_1(2);
    auto add_1_2_3 = add_1_2(3);
    assert(add_1_2_3(4) == 10);
    assert(add_1_2_3(10) == 16);

    // 更复杂的例子
    auto format4 = [](std::string a, std::string b, std::string c, std::string d) {
        return a + "-" + b + "-" + c + "-" + d;
    };

    auto curried_format = curry4(format4);
    auto result = curried_format("2024")("01")("15")("Mon");
    std::cout << "format4: " << result << "\n";

    std::cout << "curry4 测试通过\n\n";
}

// ============================================
// 练习 2: 实现 partial_at - 答案
// ============================================

// 辅助：固定第一个位置
template<typename F, typename Arg>
[[nodiscard]] auto partial_first(F f, Arg arg) {
    return [f, arg](auto... rest) {
        return f(arg, rest...);
    };
}

// 辅助：固定最后一个位置
template<typename F, typename Arg>
[[nodiscard]] auto partial_last(F f, Arg arg) {
    return [f, arg](auto... first) {
        return f(first..., arg);
    };
}

// 完整实现 partial_at
// 使用辅助函数在特定位置插入参数
namespace detail {
    template<size_t N, typename F, typename Fixed, typename... Args, size_t... Before, size_t... After>
    auto call_with_fixed_at(
        F f, Fixed fixed,
        std::tuple<Args...> args,
        std::index_sequence<Before...>,
        std::index_sequence<After...>
    ) {
        return f(
            std::get<Before>(args)...,
            fixed,
            std::get<N + After>(args)...
        );
    }
}

template<size_t N, typename F, typename Arg>
[[nodiscard]] auto partial_at(F f, Arg arg) {
    return [f, arg](auto... rest) {
        auto args_tuple = std::make_tuple(rest...);
        constexpr size_t num_rest = sizeof...(rest);

        return detail::call_with_fixed_at<N>(
            f, arg, args_tuple,
            std::make_index_sequence<N>{},
            std::make_index_sequence<num_rest - N>{}
        );
    };
}

void test_exercise_2() {
    std::cout << "=== 练习 2: 实现 partial_at - 答案 ===\n";

    auto subtract3 = [](int a, int b, int c) {
        return a - b - c;
    };

    // 固定第一个位置 (index 0)
    auto fixed_0 = partial_at<0>(subtract3, 100);
    assert(fixed_0(20, 30) == 50);  // 100 - 20 - 30 = 50
    std::cout << "partial_at<0>(100)(20, 30) = " << fixed_0(20, 30) << "\n";

    // 固定第二个位置 (index 1)
    auto fixed_1 = partial_at<1>(subtract3, 100);
    assert(fixed_1(200, 30) == 70);  // 200 - 100 - 30 = 70
    std::cout << "partial_at<1>(100)(200, 30) = " << fixed_1(200, 30) << "\n";

    // 固定第三个位置 (index 2)
    auto fixed_2 = partial_at<2>(subtract3, 100);
    assert(fixed_2(200, 50) == 50);  // 200 - 50 - 100 = 50
    std::cout << "partial_at<2>(100)(200, 50) = " << fixed_2(200, 50) << "\n";

    std::cout << "partial_at 测试通过\n\n";
}

// ============================================
// 练习 3: HTTP 请求工厂 - 答案
// ============================================

struct HttpRequest {
    std::string base_url;
    std::string method;
    std::string path;
    std::string body;

    [[nodiscard]] std::string to_string() const {
        return method + " " + base_url + path +
               (body.empty() ? "" : " [" + body + "]");
    }
};

[[nodiscard]] auto make_request_factory(std::string base_url) {
    return [base_url](std::string method) {
        return [base_url, method](std::string path) {
            return [base_url, method, path](std::string body) {
                return HttpRequest{base_url, method, path, body};
            };
        };
    };
}

// 更灵活的版本：支持无 body
[[nodiscard]] auto make_request_factory_v2(std::string base_url) {
    return [base_url](std::string method) {
        return [base_url, method](std::string path) {
            // 返回一个可以带或不带 body 的请求构建器
            return [base_url, method, path](std::string body = "") {
                return HttpRequest{base_url, method, path, body};
            };
        };
    };
}

void test_exercise_3() {
    std::cout << "=== 练习 3: HTTP 请求工厂 - 答案 ===\n";

    auto api = make_request_factory("https://api.example.com");

    // GET 请求
    auto get = api("GET");
    auto get_users = get("/users");
    auto request1 = get_users("");
    std::cout << request1.to_string() << "\n";

    // POST 请求
    auto post = api("POST");
    auto post_users = post("/users");
    auto request2 = post_users("{\"name\": \"Alice\"}");
    std::cout << request2.to_string() << "\n";

    // DELETE 请求
    auto request3 = api("DELETE")("/users/123")("");
    std::cout << request3.to_string() << "\n";

    // 实际应用：API 客户端
    auto github_api = make_request_factory("https://api.github.com");
    auto github_get = github_api("GET");
    auto github_post = github_api("POST");

    std::cout << "\n实际应用示例:\n";
    std::cout << github_get("/repos/user/repo")("").to_string() << "\n";
    std::cout << github_post("/repos/user/repo/issues")("{\"title\": \"Bug\"}").to_string() << "\n";

    std::cout << "HTTP 请求工厂 测试通过\n\n";
}

// ============================================
// 练习 4: 柯里化与函数组合 - 答案
// ============================================

template<typename F, typename G>
[[nodiscard]] auto compose(F f, G g) {
    return [f, g](auto x) { return f(g(x)); };
}

template<typename F, typename G>
[[nodiscard]] auto pipe(F f, G g) {
    return [f, g](auto x) { return g(f(x)); };
}

[[nodiscard]] auto add_curried(int a) {
    return [a](int b) { return a + b; };
}

[[nodiscard]] auto multiply_curried(int a) {
    return [a](int b) { return a * b; };
}

[[nodiscard]] auto clamp_curried(int min_val) {
    return [min_val](int max_val) {
        return [min_val, max_val](int value) {
            if (value < min_val) return min_val;
            if (value > max_val) return max_val;
            return value;
        };
    };
}

// 额外：使用 std::clamp
[[nodiscard]] auto clamp_curried_v2(int min_val) {
    return [min_val](int max_val) {
        return [min_val, max_val](int value) {
            return std::clamp(value, min_val, max_val);
        };
    };
}

void test_exercise_4() {
    std::cout << "=== 练习 4: 柯里化与函数组合 - 答案 ===\n";

    // 测试基本函数
    assert(add_curried(5)(3) == 8);
    assert(multiply_curried(2)(4) == 8);

    // 测试 clamp
    auto clamp_0_100 = clamp_curried(0)(100);
    assert(clamp_0_100(50) == 50);
    assert(clamp_0_100(150) == 100);
    assert(clamp_0_100(-10) == 0);

    // 构建管道
    auto add10 = add_curried(10);
    auto times2 = multiply_curried(2);

    auto pipeline = pipe(times2, pipe(add10, clamp_0_100));

    std::cout << "pipeline(20) = " << pipeline(20) << " (20*2+10=50)\n";
    std::cout << "pipeline(50) = " << pipeline(50) << " (50*2+10=110→100)\n";
    std::cout << "pipeline(-20) = " << pipeline(-20) << " (-20*2+10=-30→0)\n";

    assert(pipeline(20) == 50);
    assert(pipeline(50) == 100);
    assert(pipeline(-20) == 0);

    // 更复杂的管道
    auto subtract = [](int a, int b) { return a - b; };
    auto subtract_curried = [](int a) {
        return [a](int b) { return a - b; };
    };

    auto clamp_percent = clamp_curried(0)(100);
    auto double_it = multiply_curried(2);
    auto add_50 = add_curried(50);

    // 计算百分比：value -> *2 -> +50 -> clamp(0, 100)
    auto to_percent = pipe(double_it, pipe(add_50, clamp_percent));

    std::cout << "\n转换为百分比:\n";
    std::cout << "to_percent(10) = " << to_percent(10) << "\n";   // 10*2+50=70
    std::cout << "to_percent(30) = " << to_percent(30) << "\n";   // 30*2+50=110→100
    std::cout << "to_percent(-30) = " << to_percent(-30) << "\n"; // -30*2+50=-10→0

    std::cout << "柯里化与函数组合 测试通过\n\n";
}

// ============================================
// 练习 5: 实现 uncurry - 答案
// ============================================

[[nodiscard]] auto curry_add(int a) {
    return [a](int b) {
        return [a, b](int c) {
            return a + b + c;
        };
    };
}

// 递归应用参数
template<typename F, typename Tuple, size_t I = 0>
auto apply_curried_impl(F f, Tuple t) {
    if constexpr (I == std::tuple_size_v<Tuple>) {
        return f;
    } else {
        return apply_curried_impl<decltype(f(std::get<I>(t))), Tuple, I + 1>(
            f(std::get<I>(t)), t
        );
    }
}

template<typename F, typename... Args>
auto uncurry(F f, std::tuple<Args...> t) {
    return apply_curried_impl(f, t);
}

// 替代实现说明：
// 折叠表达式在这里不太适用，因为每次调用 f(arg) 返回的类型不同
// 递归模板是更好的选择

void test_exercise_5() {
    std::cout << "=== 练习 5: 实现 uncurry - 答案 ===\n";

    auto curried = curry_add;

    // 直接调用柯里化函数
    assert(curried(1)(2)(3) == 6);
    std::cout << "curry_add(1)(2)(3) = " << curried(1)(2)(3) << "\n";

    // 使用 uncurry
    auto args = std::make_tuple(1, 2, 3);
    int result = uncurry(curried, args);
    assert(result == 6);
    std::cout << "uncurry(curry_add, (1, 2, 3)) = " << result << "\n";

    // 测试不同的参数
    assert(uncurry(curried, std::make_tuple(10, 20, 30)) == 60);
    std::cout << "uncurry(curry_add, (10, 20, 30)) = " << 60 << "\n";

    std::cout << "uncurry 测试通过\n\n";
}

// ============================================
// 额外示例：flip 和 on 组合子
// ============================================

// flip: 翻转二元函数的参数
template<typename F>
[[nodiscard]] auto flip(F f) {
    return [f](auto a, auto b) {
        return f(b, a);
    };
}

// on: 在应用函数前先转换两个参数
template<typename F, typename G>
[[nodiscard]] auto on(F f, G g) {
    return [f, g](auto a, auto b) {
        return f(g(a), g(b));
    };
}

void bonus_demo() {
    std::cout << "=== 额外示例：flip 和 on ===\n";

    // flip
    auto subtract = [](int a, int b) { return a - b; };
    auto flipped_sub = flip(subtract);

    std::cout << "subtract(10, 3) = " << subtract(10, 3) << "\n";
    std::cout << "flip(subtract)(10, 3) = " << flipped_sub(10, 3) << "\n";

    // on
    auto less_than = [](size_t a, size_t b) { return a < b; };
    auto length = [](const std::string& s) { return s.length(); };
    auto compare_by_length = on(less_than, length);

    std::cout << "compare_by_length(\"hi\", \"hello\") = "
              << compare_by_length("hi", "hello") << "\n";
    std::cout << "compare_by_length(\"world\", \"hi\") = "
              << compare_by_length("world", "hi") << "\n";

    // 实际应用：按长度排序
    auto max_by_length = [&compare_by_length](const std::string& a, const std::string& b) {
        return compare_by_length(a, b) ? b : a;
    };
    std::cout << "max_by_length(\"cat\", \"elephant\") = "
              << max_by_length("cat", "elephant") << "\n";

    std::cout << "\n";
}

// ============================================
// main
// ============================================

int main() {
    std::cout << "========================================\n";
    std::cout << "    第05章练习参考答案\n";
    std::cout << "========================================\n\n";

    test_exercise_1();
    test_exercise_2();
    test_exercise_3();
    test_exercise_4();
    test_exercise_5();
    bonus_demo();

    std::cout << "========================================\n";
    std::cout << "    所有练习完成!\n";
    std::cout << "========================================\n";

    return 0;
}
