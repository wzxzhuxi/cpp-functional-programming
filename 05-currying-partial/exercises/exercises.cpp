/**
 * @file exercises.cpp
 * @brief 第05章练习题：柯里化与偏应用
 *
 * 完成下面的练习，然后运行程序验证结果。
 * 参考答案在 solutions.cpp
 */

#include <cassert>
#include <cmath>
#include <functional>
#include <iostream>
#include <string>

// ============================================
// 练习 1: 实现 curry4
// ============================================

/**
 * 实现四参数柯里化
 *
 * curry4(f)(a)(b)(c)(d) = f(a, b, c, d)
 */
template<typename F>
[[nodiscard]] auto curry4(F f) {
    // TODO: 实现四参数柯里化
    return [f](auto a) {
        return f;  // 替换这行
    };
}

void test_exercise_1() {
    std::cout << "=== 练习 1: 实现 curry4 ===\n";

    auto add4 = [](int a, int b, int c, int d) {
        return a + b + c + d;
    };

    // TODO: 实现 curry4 后取消注释以下测试
    // auto curried = curry4(add4);
    // assert(curried(1)(2)(3)(4) == 10);
    // std::cout << "curry4(add4)(1)(2)(3)(4) = " << curried(1)(2)(3)(4) << "\n";
    
    // // 部分应用
    // auto add_1 = curried(1);
    // auto add_1_2 = add_1(2);
    // auto add_1_2_3 = add_1_2(3);
    // assert(add_1_2_3(4) == 10);
    // assert(add_1_2_3(10) == 16);

    std::cout << "curry4 测试通过\n\n";
}

// ============================================
// 练习 2: 实现 partial_at
// ============================================

/**
 * partial_at<N>: 在第 N 个位置（0-indexed）固定参数
 *
 * 示例：
 *   auto f = [](int a, int b, int c) { return a - b - c; };
 *   partial_at<1>(f, 10)(1, 2) = f(1, 10, 2) = 1 - 10 - 2 = -11
 *
 * 提示：使用 index_sequence 和 tuple
 */

// 辅助：固定第一个位置（简单情况）
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

// TODO: 实现 partial_at<N>
// 这是一个高级练习，如果太难可以跳过
template<size_t N, typename F, typename Arg>
[[nodiscard]] auto partial_at(F f, Arg arg) {
    // 简化版：只实现 N=0, N=1, N=2
    if constexpr (N == 0) {
        return partial_first(f, arg);
    } else {
        // TODO: 实现其他位置
        return f;  // 替换这行
    }
}

void test_exercise_2() {
    std::cout << "=== 练习 2: 实现 partial_at ===\n";

    auto subtract3 = [](int a, int b, int c) {
        return a - b - c;
    };

    // 固定第一个位置
    auto fixed_0 = partial_at<0>(subtract3, 100);
    assert(fixed_0(20, 30) == 50);  // 100 - 20 - 30
    std::cout << "partial_at<0>(100)(20, 30) = " << fixed_0(20, 30) << "\n";

    // 固定第二个位置（高级）
    // auto fixed_1 = partial_at<1>(subtract3, 100);
    // assert(fixed_1(200, 30) == 70);  // 200 - 100 - 30
    // std::cout << "partial_at<1>(100)(200, 30) = " << fixed_1(200, 30) << "\n";

    std::cout << "partial_at 测试通过\n\n";
}

// ============================================
// 练习 3: HTTP 请求工厂
// ============================================

/**
 * 使用柯里化构建 HTTP 请求工厂
 *
 * 层次结构：
 * 1. 基础 URL
 * 2. HTTP 方法
 * 3. 路径
 * 4. 请求体（可选）
 */

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

// TODO: 实现 make_request_factory
// 返回一个柯里化的函数链
[[nodiscard]] auto make_request_factory(std::string base_url) {
    // TODO: 返回接受 method 的函数
    // 该函数返回接受 path 的函数
    // 该函数返回接受 body 的函数
    // 最终返回 HttpRequest

    return [base_url](std::string method) {
        return [base_url, method](std::string path) {
            return [base_url, method, path](std::string body) {
                return HttpRequest{base_url, method, path, body};
            };
        };
    };
}

void test_exercise_3() {
    std::cout << "=== 练习 3: HTTP 请求工厂 ===\n";

    auto api = make_request_factory("https://api.example.com");

    // 创建 GET 请求工厂
    auto get = api("GET");
    auto get_users = get("/users");

    auto request1 = get_users("");
    std::cout << request1.to_string() << "\n";

    // 创建 POST 请求工厂
    auto post = api("POST");
    auto post_users = post("/users");

    auto request2 = post_users("{\"name\": \"Alice\"}");
    std::cout << request2.to_string() << "\n";

    // 直接调用
    auto request3 = api("DELETE")("/users/123")("");
    std::cout << request3.to_string() << "\n";

    std::cout << "HTTP 请求工厂 测试通过\n\n";
}

// ============================================
// 练习 4: 柯里化与函数组合
// ============================================

/**
 * 结合柯里化和函数组合，构建数据处理管道
 */

// 提供的工具函数
template<typename F, typename G>
[[nodiscard]] auto compose(F f, G g) {
    return [f, g](auto x) { return f(g(x)); };
}

template<typename F, typename G>
[[nodiscard]] auto pipe(F f, G g) {
    return [f, g](auto x) { return g(f(x)); };
}

template<typename F>
[[nodiscard]] auto curry2(F f) {
    return [f](auto a) {
        return [f, a](auto b) {
            return f(a, b);
        };
    };
}

// TODO: 创建以下柯里化的处理函数

// 1. add: 加法 (柯里化)
// add(5)(3) = 8

// 2. multiply: 乘法 (柯里化)
// multiply(2)(4) = 8

// 3. clamp: 限制范围 (柯里化)
// clamp(0)(100)(150) = 100
// clamp(0)(100)(50) = 50
// clamp(0)(100)(-10) = 0

[[nodiscard]] auto add_curried(int a) {
    // TODO: 实现
    return [a](int b) { return a + b; };
}

[[nodiscard]] auto multiply_curried(int a) {
    // TODO: 实现
    return [a](int b) { return a * b; };
}

[[nodiscard]] auto clamp_curried(int min_val) {
    // TODO: 实现
    return [min_val](int max_val) {
        return [min_val, max_val](int value) {
            return value;  // 替换这行
        };
    };
}

void test_exercise_4() {
    std::cout << "=== 练习 4: 柯里化与函数组合 ===\n";

    // 测试基本函数
    assert(add_curried(5)(3) == 8);
    assert(multiply_curried(2)(4) == 8);

    // 测试 clamp
    auto clamp_0_100 = clamp_curried(0)(100);
    assert(clamp_0_100(50) == 50);
    assert(clamp_0_100(150) == 100);
    assert(clamp_0_100(-10) == 0);

    // 构建管道：value -> *2 -> +10 -> clamp(0, 100)
    auto add10 = add_curried(10);
    auto times2 = multiply_curried(2);

    auto pipeline = pipe(times2, pipe(add10, clamp_0_100));

    std::cout << "pipeline(20) = " << pipeline(20) << " (20*2+10=50, clamped)\n";
    std::cout << "pipeline(50) = " << pipeline(50) << " (50*2+10=110, clamped to 100)\n";
    std::cout << "pipeline(-20) = " << pipeline(-20) << " (-20*2+10=-30, clamped to 0)\n";

    assert(pipeline(20) == 50);
    assert(pipeline(50) == 100);
    assert(pipeline(-20) == 0);

    std::cout << "柯里化与函数组合 测试通过\n\n";
}

// ============================================
// 练习 5: 实现 uncurry
// ============================================

/**
 * uncurry: 将柯里化函数转换为接受元组的函数
 *
 * uncurry(curry_add3)(std::make_tuple(1, 2, 3)) = 6
 *
 * 这是 curry 的逆操作
 */

// 辅助：柯里化函数
[[nodiscard]] auto curry_add(int a) {
    return [a](int b) {
        return [a, b](int c) {
            return a + b + c;
        };
    };
}

// TODO: 实现 uncurry
// 提示：使用 std::apply 和递归调用
template<typename F, typename Tuple, size_t... Is>
auto uncurry_impl(F f, Tuple t, std::index_sequence<Is...>) {
    // TODO: 实现
    // 对于每个索引，依次调用 f
    return 0;  // 替换这行
}

template<typename F, typename... Args>
auto uncurry(F f, std::tuple<Args...> t) {
    return uncurry_impl(f, t, std::index_sequence_for<Args...>{});
}

void test_exercise_5() {
    std::cout << "=== 练习 5: 实现 uncurry ===\n";

    auto curried = curry_add;

    // 直接调用柯里化函数
    assert(curried(1)(2)(3) == 6);

    // 使用 uncurry
    auto args = std::make_tuple(1, 2, 3);
    // int result = uncurry(curried, args);
    // assert(result == 6);
    // std::cout << "uncurry(curry_add, (1, 2, 3)) = " << result << "\n";

    std::cout << "uncurry 是高级练习，可以跳过\n";
    std::cout << "uncurry 测试通过\n\n";
}

// ============================================
// main
// ============================================

int main() {
    std::cout << "========================================\n";
    std::cout << "    第05章练习：柯里化与偏应用\n";
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
