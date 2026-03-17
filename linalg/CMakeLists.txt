include_guard(GLOBAL)
cmake_minimum_required(VERSION 3.24)
project(Linalg)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


add_library(linalg SHARED 
    vec.cpp
    mat4.cpp
    math_utils.cpp 
)

target_include_directories(linalg PUBLIC include)


