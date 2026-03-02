/**
 * @file exercises.cpp
 * @brief 第09章练习题：错误处理与 Result 类型
 *
 * 完成下面的练习，然后运行程序验证结果。
 * 参考答案在 solutions.cpp
 */

#include <cassert>
#include <cmath>
#include <iostream>
#include <sstream>
#include <string>
#include <variant>
#include <vector>

// overload 辅助工具
template<class... Ts>
struct overload : Ts... { using Ts::operator()...; };

template<class... Ts>
overload(Ts...) -> overload<Ts...>;

// ============================================
// Result 类型（已提供）
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

    [[nodiscard]] bool is_ok() const noexcept { return data_.index() == 0; }
    [[nodiscard]] bool is_err() const noexcept { return data_.index() == 1; }

    [[nodiscard]] const T& unwrap() const {
        if (is_err()) throw std::runtime_error("unwrap on Err");
        return std::get<0>(data_);
    }

    [[nodiscard]] const E& unwrap_err() const {
        if (is_ok()) throw std::runtime_error("unwrap_err on Ok");
        return std::get<1>(data_);
    }

    [[nodiscard]] T unwrap_or(T default_value) const {
        return is_ok() ? std::get<0>(data_) : std::move(default_value);
    }

    template<typename F>
    [[nodiscard]] auto map(F f) const {
        using NewT = std::invoke_result_t<F, const T&>;
        if (is_ok()) {
            return Result<NewT, E>::ok(f(std::get<0>(data_)));
        }
        return Result<NewT, E>::err(std::get<1>(data_));
    }

    template<typename F>
    [[nodiscard]] auto and_then(F f) const {
        using NewResult = std::invoke_result_t<F, const T&>;
        if (is_ok()) {
            return f(std::get<0>(data_));
        }
        return NewResult::err(std::get<1>(data_));
    }

    // TODO: 在练习1中添加更多方法
};

// ============================================
// 练习 1: 扩展 Result 类型
// ============================================

/**
 * 为 Result 添加以下方法：
 * - map_err(F f): 转换错误值
 * - or_else(F f): 错误时尝试恢复
 * - unwrap_or_else(F f): 错误时调用函数获取默认值
 *
 * 由于模板类成员难以分离实现，请在下方实现包装函数
 */

// TODO: 实现 result_map_err
// 将错误类型从 E 转换为 NewE
template<typename T, typename E, typename F>
[[nodiscard]] auto result_map_err(const Result<T, E>& r, F f) {
    using NewE = std::invoke_result_t<F, const E&>;
    // 修改这里
    (void)f;
    return Result<T, NewE>::err(NewE{});
}

// TODO: 实现 result_or_else
// 错误时调用 f 尝试恢复
template<typename T, typename E, typename F>
[[nodiscard]] auto result_or_else(const Result<T, E>& r, F f) {
    // 修改这里
    (void)f;
    return r;
}

// TODO: 实现 result_unwrap_or_else
// 错误时调用 f 获取默认值
template<typename T, typename E, typename F>
[[nodiscard]] T result_unwrap_or_else(const Result<T, E>& r, F f) {
    // 修改这里
    (void)f;
    return T{};
}

void test_exercise_1() {
    std::cout << "=== 练习 1: 扩展 Result ===\n";

    // map_err 测试
    auto r1 = Result<int, int>::err(42);
    auto r2 = result_map_err(r1, [](int e) { return std::to_string(e); });
    assert(r2.is_err());
    assert(r2.unwrap_err() == "42");
    std::cout << "map_err: " << r2.unwrap_err() << "\n";

    // or_else 测试
    auto r3 = Result<int, std::string>::err("error");
    auto r4 = result_or_else(r3, [](const std::string&) {
        return Result<int, std::string>::ok(100);  // 恢复为成功
    });
    assert(r4.is_ok());
    assert(r4.unwrap() == 100);
    std::cout << "or_else recovery: " << r4.unwrap() << "\n";

    // unwrap_or_else 测试
    auto r5 = Result<int, std::string>::err("error");
    int val = result_unwrap_or_else(r5, []() { return 999; });
    assert(val == 999);
    std::cout << "unwrap_or_else: " << val << "\n";

    std::cout << "练习 1 通过\n\n";
}

// ============================================
// 练习 2: 链式文件操作
// ============================================

enum class FileError {
    NotFound,
    PermissionDenied,
    ParseError,
    EmptyFile
};

[[nodiscard]] std::string file_error_str(FileError e) {
    switch (e) {
        case FileError::NotFound: return "文件未找到";
        case FileError::PermissionDenied: return "权限不足";
        case FileError::ParseError: return "解析错误";
        case FileError::EmptyFile: return "空文件";
    }
    return "未知错误";
}

// 模拟文件系统
[[nodiscard]] Result<std::string, FileError> read_file(const std::string& path) {
    if (path == "missing.txt") {
        return Result<std::string, FileError>::err(FileError::NotFound);
    }
    if (path == "secret.txt") {
        return Result<std::string, FileError>::err(FileError::PermissionDenied);
    }
    if (path == "empty.txt") {
        return Result<std::string, FileError>::ok("");
    }
    if (path == "numbers.txt") {
        return Result<std::string, FileError>::ok("10 20 30 40 50");
    }
    if (path == "bad.txt") {
        return Result<std::string, FileError>::ok("10 abc 30");
    }
    return Result<std::string, FileError>::ok("default content");
}

// TODO: 实现 check_not_empty
// 检查内容是否为空，空则返回 EmptyFile 错误
[[nodiscard]] Result<std::string, FileError> check_not_empty(const std::string& content) {
    // 修改这里
    (void)content;
    return Result<std::string, FileError>::err(FileError::EmptyFile);
}

// TODO: 实现 parse_numbers
// 解析空格分隔的数字，任何非数字返回 ParseError
[[nodiscard]] Result<std::vector<int>, FileError> parse_numbers(const std::string& content) {
    // 修改这里
    (void)content;
    return Result<std::vector<int>, FileError>::err(FileError::ParseError);
}

// TODO: 实现 process_file
// 链式操作：读取文件 -> 检查非空 -> 解析数字 -> 计算和
[[nodiscard]] Result<int, FileError> process_file(const std::string& path) {
    // 使用 and_then 链式调用 read_file, check_not_empty, parse_numbers
    // 最后用 map 计算数组和
    // 修改这里
    (void)path;
    return Result<int, FileError>::err(FileError::NotFound);
}

void test_exercise_2() {
    std::cout << "=== 练习 2: 链式文件操作 ===\n";

    // 成功案例
    auto r1 = process_file("numbers.txt");
    assert(r1.is_ok());
    assert(r1.unwrap() == 150);  // 10+20+30+40+50
    std::cout << "numbers.txt sum: " << r1.unwrap() << "\n";

    // 文件不存在
    auto r2 = process_file("missing.txt");
    assert(r2.is_err());
    assert(r2.unwrap_err() == FileError::NotFound);
    std::cout << "missing.txt: " << file_error_str(r2.unwrap_err()) << "\n";

    // 空文件
    auto r3 = process_file("empty.txt");
    assert(r3.is_err());
    assert(r3.unwrap_err() == FileError::EmptyFile);
    std::cout << "empty.txt: " << file_error_str(r3.unwrap_err()) << "\n";

    // 解析错误
    auto r4 = process_file("bad.txt");
    assert(r4.is_err());
    assert(r4.unwrap_err() == FileError::ParseError);
    std::cout << "bad.txt: " << file_error_str(r4.unwrap_err()) << "\n";

    std::cout << "练习 2 通过\n\n";
}

// ============================================
// 练习 3: HTTP 响应处理
// ============================================

struct HttpResponse {
    int status_code;
    std::string body;
};

struct HttpError {
    int code;
    std::string message;
};

// TODO: 实现 check_status
// 200-299 成功，返回 body
// 其他返回 HttpError
[[nodiscard]] Result<std::string, HttpError> check_status(const HttpResponse& resp) {
    // 修改这里
    (void)resp;
    return Result<std::string, HttpError>::err({500, "Not implemented"});
}

// TODO: 实现 parse_json_number
// 解析简单的数字 body（如 "42"），失败返回 HttpError
[[nodiscard]] Result<int, HttpError> parse_json_number(const std::string& body) {
    // 修改这里
    (void)body;
    return Result<int, HttpError>::err({0, "Not implemented"});
}

// TODO: 实现 fetch_and_parse
// 检查状态 -> 解析数字
[[nodiscard]] Result<int, HttpError> fetch_and_parse(const HttpResponse& resp) {
    // 使用 and_then 链式调用
    // 修改这里
    (void)resp;
    return Result<int, HttpError>::err({0, "Not implemented"});
}

void test_exercise_3() {
    std::cout << "=== 练习 3: HTTP 响应处理 ===\n";

    // 成功
    HttpResponse resp1{200, "42"};
    auto r1 = fetch_and_parse(resp1);
    assert(r1.is_ok());
    assert(r1.unwrap() == 42);
    std::cout << "200 OK: " << r1.unwrap() << "\n";

    // 404
    HttpResponse resp2{404, "Not found"};
    auto r2 = fetch_and_parse(resp2);
    assert(r2.is_err());
    assert(r2.unwrap_err().code == 404);
    std::cout << "404: " << r2.unwrap_err().message << "\n";

    // 解析失败
    HttpResponse resp3{200, "not a number"};
    auto r3 = fetch_and_parse(resp3);
    assert(r3.is_err());
    std::cout << "Parse error: " << r3.unwrap_err().message << "\n";

    std::cout << "练习 3 通过\n\n";
}

// ============================================
// 练习 4: 多错误收集
// ============================================

struct FormData {
    std::string username;
    std::string email;
    std::string password;
    int age;
};

struct ValidatedForm {
    std::string username;
    std::string email;
    std::string password;
    int age;
};

struct FormErrors {
    std::vector<std::string> messages;

    void add(const std::string& msg) {
        messages.push_back(msg);
    }

    [[nodiscard]] bool has_errors() const {
        return !messages.empty();
    }
};

// TODO: 实现 validate_form
// 验证规则：
// - username: 非空，3-20字符
// - email: 包含 '@' 和 '.'
// - password: 至少6字符
// - age: 13-120
// 收集所有错误，不要提前返回
[[nodiscard]] Result<ValidatedForm, FormErrors> validate_form(const FormData& form) {
    // 修改这里
    (void)form;
    FormErrors errors;
    errors.add("Not implemented");
    return Result<ValidatedForm, FormErrors>::err(std::move(errors));
}

void test_exercise_4() {
    std::cout << "=== 练习 4: 多错误收集 ===\n";

    // 完全无效的表单
    FormData bad{"", "invalid", "123", 5};
    auto r1 = validate_form(bad);
    assert(r1.is_err());
    assert(r1.unwrap_err().messages.size() >= 4);
    std::cout << "错误数量: " << r1.unwrap_err().messages.size() << "\n";
    for (const auto& msg : r1.unwrap_err().messages) {
        std::cout << "  - " << msg << "\n";
    }

    // 有效表单
    FormData good{"alice", "alice@example.com", "secret123", 25};
    auto r2 = validate_form(good);
    assert(r2.is_ok());
    std::cout << "有效用户: " << r2.unwrap().username << "\n";

    std::cout << "练习 4 通过\n\n";
}

// ============================================
// 练习 5: 批量操作与错误汇总
// ============================================

// TODO: 实现 collect_all_errors
// 收集所有操作的错误，成功则返回所有值
// 与 collect_results（遇到第一个错误就停止）不同
template<typename T, typename E>
[[nodiscard]] Result<std::vector<T>, std::vector<E>> collect_all_errors(
    const std::vector<Result<T, E>>& results
) {
    // 修改这里
    (void)results;
    return Result<std::vector<T>, std::vector<E>>::err({});
}

void test_exercise_5() {
    std::cout << "=== 练习 5: 批量错误收集 ===\n";

    // 全部成功
    std::vector<Result<int, std::string>> all_ok = {
        Result<int, std::string>::ok(1),
        Result<int, std::string>::ok(2),
        Result<int, std::string>::ok(3)
    };
    auto r1 = collect_all_errors(all_ok);
    assert(r1.is_ok());
    assert(r1.unwrap().size() == 3);
    std::cout << "全部成功: ";
    for (int v : r1.unwrap()) std::cout << v << " ";
    std::cout << "\n";

    // 部分失败
    std::vector<Result<int, std::string>> some_err = {
        Result<int, std::string>::ok(1),
        Result<int, std::string>::err("error A"),
        Result<int, std::string>::ok(3),
        Result<int, std::string>::err("error B")
    };
    auto r2 = collect_all_errors(some_err);
    assert(r2.is_err());
    assert(r2.unwrap_err().size() == 2);
    std::cout << "收集到错误:\n";
    for (const auto& e : r2.unwrap_err()) {
        std::cout << "  - " << e << "\n";
    }

    std::cout << "练习 5 通过\n\n";
}

// ============================================
// main
// ============================================

int main() {
    std::cout << "========================================\n";
    std::cout << "    第09章练习：错误处理\n";
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
