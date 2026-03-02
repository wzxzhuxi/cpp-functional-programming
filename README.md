# C++ 函数式编程教程

> "Talk is cheap. Show me the code."

## 概述

这是一个使用 C++ 进行函数式编程的实战教程。不搞虚的，直接上代码。

## 为什么用 C++ 做函数式编程？

- C++ 支持 lambda、模板、constexpr
- 零成本抽象 - 函数式不等于低效
- 强类型系统
- 编译期计算能力强大

## 目录结构

### 基础篇

- **01-functions/** - Lambda 表达式、闭包、std::function、泛型 lambda
- **02-immutability/** - const 正确性、constexpr、不可变类设计
- **03-pure-functions/** - 纯函数、副作用隔离、引用透明性
- **04-higher-order-functions/** - map/filter/reduce、STL 算法、函数作为参数和返回值

### 核心技术

- **05-currying-partial/** - 柯里化模板、std::bind、偏应用、配置工厂
- **06-composition/** - compose/pipe、组合子、point-free 风格
- **07-ranges/** - C++20 Ranges、views 管道语法、惰性求值

### 类型驱动设计

- **08-algebraic-types/** - std::variant、std::optional、模式匹配、状态机
- **09-error-handling/** - Result 类型、std::expected (C++23)、异常 vs 返回值

### 进阶模式

- **10-functors-monads/** - fmap、bind、Maybe/Result monad、monad 定律
- **11-lazy-memoization/** - Lazy\<T\>、memoize、无限序列
- **12-capstone/** - 构建一个内存数据查询 DSL（领域特定语言）

特别说明 : 原书实战项目为 **actor模型设计** ,本教程为 **DSL**

### 项目实战

- [FunServe](https://github.com/wzxzhuxi/FunServe) - 使用本教程所学的函数式编程技术构建的实战项目

## 环境要求

- C++20 或更高（推荐 C++23）
- 编译器：GCC 10+, Clang 12+, MSVC 2019+
- CMake 3.20+

## 编译

```bash
cmake -S . -B build
cmake --build build

# 编译单个目标（格式：<章节目录>_<文件名>）
cmake --build build --target 01-functions_01_lambdas

# 运行
./build/bin/01-functions_01_lambdas
```

## 学习路径

1. 按章节顺序学习
2. 每章先看理论，再跑代码
3. 完成对应练习
4. 不懂的多看几遍代码 - 代码不会撒谎

## 核心原则

- **Immutability** - 默认不可变
- **Pure Functions** - 同样输入 → 同样输出
- **Composition** - 小函数组合成复杂逻辑
- **Declarative** - 说"要什么"，不是"怎么做"
- **Type Safety** - 类型系统是你的朋友

## 反模式警告

避免这些坑：
- 深度嵌套（> 3 层就该重构）
- 上帝函数（一个函数干太多事）
- 共享可变状态（bug 的温床）
- 到处 null 检查（用 Optional）
- 异常做流程控制（用 Result）

## 审核
内容使用claude code以及codex审核

## 贡献

发现问题直接提 issue，有更好的实现就 PR。

## License

MIT
