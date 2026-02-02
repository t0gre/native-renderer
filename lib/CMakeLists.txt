cmake_minimum_required(VERSION 3.24)
project(NativeRendererLib)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(OpenGL REQUIRED)
add_subdirectory(../third_party/SDL SDL3)
add_subdirectory(../third_party/assimp assimp)

# lib (include is for templates)
add_library(lib SHARED 
    camera.cpp
    mat4.cpp
    scene.cpp
    events.cpp
    math_utils.cpp
    raycast.cpp    
    vec.cpp
    loaders.cpp
    gl/render_program.cpp
    gl/gl_renderer.cpp
    gl/basic_color_render_program.cpp
    gl/texture_render_program.cpp
    include/mystl.hpp 
)

target_include_directories(lib PUBLIC include)

target_link_libraries(lib 
    SDL3::SDL3  
    assimp::assimp
    ${OPENGL_LIBRARIES})


