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

- **01-basics/** - 函数式编程基础概念
  - 什么是函数式编程
  - C++ 中的函数式特性
  - Lambda 表达式和闭包

- **02-immutability/** - 不可变性
  - const 的正确用法
  - 不可变数据结构
  - 为什么可变状态是万恶之源

- **03-pure-functions/** - 纯函数
  - 纯函数定义和好处
  - 副作用隔离
  - 引用透明性

### 核心篇

- **04-higher-order-functions/** - 高阶函数
  - 函数作为一等公民
  - map/filter/reduce 实现
  - 函数柯里化和偏应用

- **05-functional-composition/** - 函数组合
  - pipe 和 compose
  - 数据流转换
  - 点自由风格（point-free）

- **06-algebraic-types/** - 代数数据类型
  - Sum types（std::variant）
  - Product types（std::tuple）
  - Pattern matching

### 进阶篇

- **07-monads/** - Monad 模式
  - Optional/Maybe monad
  - Result/Either monad
  - IO monad 概念

- **08-advanced-patterns/** - 高级模式
  - Functor, Applicative
  - 懒求值（lazy evaluation）
  - 递归和尾调用优化

### 实践

- **examples/** - 示例代码
  - `basic/` - 基础示例
  - `intermediate/` - 中级示例
  - `advanced/` - 高级示例

- **exercises/** - 练习题
  - `solutions/` - 参考答案
  - `tests/` - 单元测试

## 环境要求

- C++17 或更高（推荐 C++20）
- 支持的编译器：GCC 10+, Clang 12+, MSVC 2019+
- CMake 3.15+

## 编译

```bash
mkdir build && cd build
cmake ..
make
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
