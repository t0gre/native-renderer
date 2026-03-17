include_guard(GLOBAL)
cmake_minimum_required(VERSION 3.24)
project(GlRenderer)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(OpenGL REQUIRED)
add_subdirectory(../lib lib)

add_library(gl_render SHARED 
    basic_color_render_program.cpp
    gl_renderer.cpp
    render_program.cpp 
    texture_render_program.cpp
)

target_include_directories(gl_render PUBLIC include)
target_link_libraries(gl_render PUBLIC lib)

