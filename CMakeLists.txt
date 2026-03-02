cmake_minimum_required(VERSION 3.20)
project(fp_chapters CXX)

# ============================================
# C++ 标准设置
# ============================================

include(CheckCXXCompilerFlag)
check_cxx_compiler_flag("-std=c++23" COMPILER_SUPPORTS_CXX23)

if(COMPILER_SUPPORTS_CXX23)
    set(CMAKE_CXX_STANDARD 23)
    message(STATUS "Using C++23")
else()
    set(CMAKE_CXX_STANDARD 20)
    message(STATUS "C++23 not available, falling back to C++20")
endif()

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ============================================
# 输出目录
# ============================================

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# ============================================
# 编译器警告
# ============================================

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(
        -Wall
        -Wextra
        -Wpedantic
        -Wconversion
        -Wsign-conversion
        -Wnon-virtual-dtor
        -Wold-style-cast
        -Woverloaded-virtual
        -Wshadow
    )

    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        add_compile_options(-Wmost -Wextra-semi)
    endif()
endif()

if(MSVC)
    add_compile_options(/W4 /permissive-)
endif()

# ============================================
# 辅助函数：添加章节
# ============================================

function(add_chapter CHAPTER_NAME)
    set(CHAPTER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/${CHAPTER_NAME})

    # 添加示例
    if(EXISTS ${CHAPTER_DIR}/examples)
        file(GLOB EXAMPLE_SOURCES ${CHAPTER_DIR}/examples/*.cpp)
        foreach(SOURCE ${EXAMPLE_SOURCES})
            get_filename_component(EXAMPLE_NAME ${SOURCE} NAME_WE)
            set(TARGET_NAME "${CHAPTER_NAME}_${EXAMPLE_NAME}")
            add_executable(${TARGET_NAME} ${SOURCE})
        endforeach()
    endif()

    # 添加练习
    if(EXISTS ${CHAPTER_DIR}/exercises)
        file(GLOB EXERCISE_SOURCES ${CHAPTER_DIR}/exercises/*.cpp)
        foreach(SOURCE ${EXERCISE_SOURCES})
            get_filename_component(EXERCISE_NAME ${SOURCE} NAME_WE)
            set(TARGET_NAME "${CHAPTER_NAME}_${EXERCISE_NAME}")
            add_executable(${TARGET_NAME} ${SOURCE})
        endforeach()
    endif()
endfunction()

# ============================================
# 添加所有章节
# ============================================

# 第一阶段：基础篇
add_chapter(01-functions)
add_chapter(02-immutability)
add_chapter(03-pure-functions)
add_chapter(04-higher-order-functions)

# 第二阶段：核心技术
add_chapter(05-currying-partial)
add_chapter(06-composition)
add_chapter(07-ranges)

# 第三阶段：类型驱动设计
add_chapter(08-algebraic-types)
add_chapter(09-error-handling)

# 第四阶段：进阶模式
add_chapter(10-functors-monads)
add_chapter(11-lazy-memoization)
add_chapter(12-capstone)

# ============================================
# 配置摘要
# ============================================

message(STATUS "")
message(STATUS "=== Configuration Summary ===")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Compiler:     ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "Build Type:   ${CMAKE_BUILD_TYPE}")
message(STATUS "=============================")
message(STATUS "")
