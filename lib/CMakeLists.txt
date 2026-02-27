include_guard(GLOBAL)
cmake_minimum_required(VERSION 3.24)
project(NativeRendererLib)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# find_package(OpenGL REQUIRED)
set(SDL_ALSA OFF)
set(SDL_ALSA_SHARED OFF)
set(SDL_RENDER_GPU OFF)
set(SDL_VULKAN OFF)
add_subdirectory(../third_party/SDL SDL3)

set(ASSIMP_BUILD_TESTS OFF)
set(ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT OFF)
set(ASSIMP_BUILD_ALL_EXPORTERS_BY_DEFAULT OFF)
set(ASSIMP_BUILD_GLTF_IMPORTER ON)
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


