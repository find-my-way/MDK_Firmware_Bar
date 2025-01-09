# 设置 CMake 最低版本要求
cmake_minimum_required(VERSION 3.10)

# 指定编译器
set(CMAKE_C_COMPILER "D:/mingw64/bin/gcc.exe")
set(CMAKE_CXX_COMPILER "D:/mingw64/bin/g++.exe")

# 项目名称和版本
project(firmware_bar)

# 设置 C++ 标准（需要 C++11 或更高版本）
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 启用生成compile_command.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 添加源文件
set(SOURCE_FILES main.cpp tinyxml2.cpp)
add_executable(firmware_bar ${SOURCE_FILES})