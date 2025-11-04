cmake_minimum_required(VERSION 3.24)
project(NativeRendererLib)

#set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(OpenGL REQUIRED)
add_subdirectory(../SDL SDL3)

# lib
add_library(lib SHARED 
    camera.cpp
    mat4.cpp
    scene.cpp
    events.cpp
    math_utils.cpp
    raycast.cpp    
    vec.cpp
    loaders.cpp
    render_program.cpp
)

target_include_directories(lib PUBLIC include ../SDL/include)
target_link_libraries(lib SDL3::SDL3)


