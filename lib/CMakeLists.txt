include_guard(GLOBAL)
cmake_minimum_required(VERSION 3.24)
project(NativeRendererLib)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# find_package(OpenGL REQUIRED)
add_subdirectory(../third_party/SDL SDL3)
add_subdirectory(../third_party/assimp assimp)
add_subdirectory(../mym mym)

# lib (include is for templates)
add_library(lib SHARED 
    camera.cpp
    scene.cpp
    raycast.cpp    
    loaders.cpp
    include/mystl.hpp 
)

target_include_directories(lib PUBLIC include)

target_link_libraries(lib 
    mym
    SDL3::SDL3  
    assimp::assimp
    ${OPENGL_LIBRARIES})


