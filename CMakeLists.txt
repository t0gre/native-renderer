cmake_minimum_required(VERSION 3.24)
project(NativeRenderer)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(SDL2 REQUIRED)
find_package(OpenGL REQUIRED)

# lib
add_library(lib SHARED 
    lib/camera.cpp
    lib/mat4.cpp
    lib/scene.cpp
    lib/events.cpp
    lib/math_utils.cpp
    lib/raycast.cpp    
    lib/vec.cpp
    lib/loaders.cpp
    lib/render_program.cpp
)

target_include_directories(lib PUBLIC lib/include ${SDL2_INCLUDE_DIRS})
target_link_libraries(lib ${SDL2_LIBRARIES})

# executable
add_executable(native index.cpp)
target_link_libraries(native PRIVATE 
    lib 
    ${SDL2_LIBRARIES}
    ${OPENGL_LIBRARIES}
)

target_include_directories(native PUBLIC 
    lib/include 
    ${SDL2_INCLUDE_DIRS}
    ${OPENGL_INCLUDE_DIRS}
)

# shaders
file(COPY lib/shaders DESTINATION ${CMAKE_BINARY_DIR})
file(COPY assets DESTINATION ${CMAKE_BINARY_DIR})

