# CMake 最低版本要求
cmake_minimum_required (VERSION 3.23.0)

# 项目名
project(Alice)

# 设置可执行程序输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY bin)

# 设置项目使用的 CPP 版本
set(CMAKE_CXX_STANDARD 17)

# 添加头文件包含路径
include_directories(
    include
)

# 设置需要编译的源文件
file(GLOB ALL_SRC_FILES
    src/*.cpp
    src/ECS/*.cpp
)

set(SRC_FILES
    ${ALL_SRC_FILES}    
)

# 输出可执行文件
add_executable(Alice ${SRC_FILES})
