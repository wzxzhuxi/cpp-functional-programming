/**
 * @file solutions.cpp
 * @brief 第09章练习题参考答案
 */

#include <cassert>
#include <cmath>
#include <iostream>
#include <numeric>
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
// Result 类型
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

    // 内部访问
    [[nodiscard]] const std::variant<T, E>& data() const { return data_; }
};

// ============================================
// 练习 1 答案: 扩展 Result 类型
// ============================================

// map_err: 转换错误值
template<typename T, typename E, typename F>
[[nodiscard]] auto result_map_err(const Result<T, E>& r, F f) {
    using NewE = std::invoke_result_t<F, const E&>;
    if (r.is_err()) {
        return Result<T, NewE>::err(f(r.unwrap_err()));
    }
    return Result<T, NewE>::ok(r.unwrap());
}

// or_else: 错误时尝试恢复
template<typename T, typename E, typename F>
[[nodiscard]] auto result_or_else(const Result<T, E>& r, F f) {
    using NewResult = std::invoke_result_t<F, const E&>;
    if (r.is_err()) {
        return f(r.unwrap_err());
    }
    return NewResult::ok(r.unwrap());
}

// unwrap_or_else: 错误时调用函数获取默认值
template<typename T, typename E, typename F>
[[nodiscard]] T result_unwrap_or_else(const Result<T, E>& r, F f) {
    if (r.is_err()) {
        return f();
    }
    return r.unwrap();
}

void test_exercise_1() {
    std::cout << "=== 练习 1: 扩展 Result ===\n";

    // map_err 测试
    auto r1 = Result<int, int>::err(42);
    auto r2 = result_map_err(r1, [](int e) { return std::to_string(e); });
    assert(r2.is_err());
    assert(r2.unwrap_err() == "42");
    std::cout << "map_err: " << r2.unwrap_err() << "\n";

    // 成功时 map_err 不做任何事
    auto r1_ok = Result<int, int>::ok(100);
    auto r2_ok = result_map_err(r1_ok, [](int e) { return std::to_string(e); });
    assert(r2_ok.is_ok());
    assert(r2_ok.unwrap() == 100);

    // or_else 测试
    auto r3 = Result<int, std::string>::err("error");
    auto r4 = result_or_else(r3, [](const std::string&) {
        return Result<int, std::string>::ok(100);
    });
    assert(r4.is_ok());
    assert(r4.unwrap() == 100);
    std::cout << "or_else recovery: " << r4.unwrap() << "\n";

    // 成功时 or_else 不做任何事
    auto r3_ok = Result<int, std::string>::ok(50);
    auto r4_ok = result_or_else(r3_ok, [](const std::string&) {
        return Result<int, std::string>::ok(100);
    });
    assert(r4_ok.is_ok());
    assert(r4_ok.unwrap() == 50);

    // unwrap_or_else 测试
    auto r5 = Result<int, std::string>::err("error");
    int val = result_unwrap_or_else(r5, []() { return 999; });
    assert(val == 999);
    std::cout << "unwrap_or_else: " << val << "\n";

    std::cout << "练习 1 通过\n\n";
}

// ============================================
// 练习 2 答案: 链式文件操作
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

// 检查内容是否为空
[[nodiscard]] Result<std::string, FileError> check_not_empty(const std::string& content) {
    if (content.empty()) {
        return Result<std::string, FileError>::err(FileError::EmptyFile);
    }
    return Result<std::string, FileError>::ok(content);
}

// 解析空格分隔的数字
[[nodiscard]] Result<std::vector<int>, FileError> parse_numbers(const std::string& content) {
    std::vector<int> numbers;
    std::istringstream iss(content);
    std::string token;

    while (iss >> token) {
        try {
            size_t pos;
            int num = std::stoi(token, &pos);
            if (pos != token.length()) {
                return Result<std::vector<int>, FileError>::err(FileError::ParseError);
            }
            numbers.push_back(num);
        } catch (...) {
            return Result<std::vector<int>, FileError>::err(FileError::ParseError);
        }
    }

    return Result<std::vector<int>, FileError>::ok(std::move(numbers));
}

// 链式操作
[[nodiscard]] Result<int, FileError> process_file(const std::string& path) {
    return read_file(path)
        .and_then([](const std::string& content) {
            return check_not_empty(content);
        })
        .and_then([](const std::string& content) {
            return parse_numbers(content);
        })
        .map([](const std::vector<int>& nums) {
            return std::accumulate(nums.begin(), nums.end(), 0);
        });
}

void test_exercise_2() {
    std::cout << "=== 练习 2: 链式文件操作 ===\n";

    auto r1 = process_file("numbers.txt");
    assert(r1.is_ok());
    assert(r1.unwrap() == 150);
    std::cout << "numbers.txt sum: " << r1.unwrap() << "\n";

    auto r2 = process_file("missing.txt");
    assert(r2.is_err());
    assert(r2.unwrap_err() == FileError::NotFound);
    std::cout << "missing.txt: " << file_error_str(r2.unwrap_err()) << "\n";

    auto r3 = process_file("empty.txt");
    assert(r3.is_err());
    assert(r3.unwrap_err() == FileError::EmptyFile);
    std::cout << "empty.txt: " << file_error_str(r3.unwrap_err()) << "\n";

    auto r4 = process_file("bad.txt");
    assert(r4.is_err());
    assert(r4.unwrap_err() == FileError::ParseError);
    std::cout << "bad.txt: " << file_error_str(r4.unwrap_err()) << "\n";

    std::cout << "练习 2 通过\n\n";
}

// ============================================
// 练习 3 答案: HTTP 响应处理
// ============================================

struct HttpResponse {
    int status_code;
    std::string body;
};

struct HttpError {
    int code;
    std::string message;
};

// 检查状态码
[[nodiscard]] Result<std::string, HttpError> check_status(const HttpResponse& resp) {
    if (resp.status_code >= 200 && resp.status_code < 300) {
        return Result<std::string, HttpError>::ok(resp.body);
    }

    std::string message;
    switch (resp.status_code) {
        case 400: message = "Bad Request"; break;
        case 401: message = "Unauthorized"; break;
        case 403: message = "Forbidden"; break;
        case 404: message = "Not Found"; break;
        case 500: message = "Internal Server Error"; break;
        default: message = "HTTP Error"; break;
    }

    return Result<std::string, HttpError>::err({resp.status_code, message});
}

// 解析数字
[[nodiscard]] Result<int, HttpError> parse_json_number(const std::string& body) {
    try {
        size_t pos;
        int value = std::stoi(body, &pos);
        // 检查是否完全解析
        size_t i = pos;
        while (i < body.length() && std::isspace(body[i])) ++i;
        if (i != body.length()) {
            return Result<int, HttpError>::err({0, "Invalid JSON number"});
        }
        return Result<int, HttpError>::ok(value);
    } catch (...) {
        return Result<int, HttpError>::err({0, "Parse error"});
    }
}

// 链式处理
[[nodiscard]] Result<int, HttpError> fetch_and_parse(const HttpResponse& resp) {
    return check_status(resp)
        .and_then([](const std::string& body) {
            return parse_json_number(body);
        });
}

void test_exercise_3() {
    std::cout << "=== 练习 3: HTTP 响应处理 ===\n";

    HttpResponse resp1{200, "42"};
    auto r1 = fetch_and_parse(resp1);
    assert(r1.is_ok());
    assert(r1.unwrap() == 42);
    std::cout << "200 OK: " << r1.unwrap() << "\n";

    HttpResponse resp2{404, "Not found"};
    auto r2 = fetch_and_parse(resp2);
    assert(r2.is_err());
    assert(r2.unwrap_err().code == 404);
    std::cout << "404: " << r2.unwrap_err().message << "\n";

    HttpResponse resp3{200, "not a number"};
    auto r3 = fetch_and_parse(resp3);
    assert(r3.is_err());
    std::cout << "Parse error: " << r3.unwrap_err().message << "\n";

    std::cout << "练习 3 通过\n\n";
}

// ============================================
// 练习 4 答案: 多错误收集
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

[[nodiscard]] Result<ValidatedForm, FormErrors> validate_form(const FormData& form) {
    FormErrors errors;

    // 验证 username
    if (form.username.empty()) {
        errors.add("用户名不能为空");
    } else if (form.username.length() < 3) {
        errors.add("用户名至少3个字符");
    } else if (form.username.length() > 20) {
        errors.add("用户名最多20个字符");
    }

    // 验证 email
    if (form.email.find('@') == std::string::npos ||
        form.email.find('.') == std::string::npos) {
        errors.add("邮箱格式无效");
    }

    // 验证 password
    if (form.password.length() < 6) {
        errors.add("密码至少6个字符");
    }

    // 验证 age
    if (form.age < 13 || form.age > 120) {
        errors.add("年龄必须在13-120之间");
    }

    if (errors.has_errors()) {
        return Result<ValidatedForm, FormErrors>::err(std::move(errors));
    }

    return Result<ValidatedForm, FormErrors>::ok(
        ValidatedForm{form.username, form.email, form.password, form.age}
    );
}

void test_exercise_4() {
    std::cout << "=== 练习 4: 多错误收集 ===\n";

    FormData bad{"", "invalid", "123", 5};
    auto r1 = validate_form(bad);
    assert(r1.is_err());
    assert(r1.unwrap_err().messages.size() >= 4);
    std::cout << "错误数量: " << r1.unwrap_err().messages.size() << "\n";
    for (const auto& msg : r1.unwrap_err().messages) {
        std::cout << "  - " << msg << "\n";
    }

    FormData good{"alice", "alice@example.com", "secret123", 25};
    auto r2 = validate_form(good);
    assert(r2.is_ok());
    std::cout << "有效用户: " << r2.unwrap().username << "\n";

    std::cout << "练习 4 通过\n\n";
}

// ============================================
// 练习 5 答案: 批量错误收集
// ============================================

template<typename T, typename E>
[[nodiscard]] Result<std::vector<T>, std::vector<E>> collect_all_errors(
    const std::vector<Result<T, E>>& results
) {
    std::vector<T> values;
    std::vector<E> errors;

    for (const auto& r : results) {
        if (r.is_ok()) {
            values.push_back(r.unwrap());
        } else {
            errors.push_back(r.unwrap_err());
        }
    }

    if (!errors.empty()) {
        return Result<std::vector<T>, std::vector<E>>::err(std::move(errors));
    }

    return Result<std::vector<T>, std::vector<E>>::ok(std::move(values));
}

void test_exercise_5() {
    std::cout << "=== 练习 5: 批量错误收集 ===\n";

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
    std::cout << "    第09章练习：错误处理 (参考答案)\n";
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
