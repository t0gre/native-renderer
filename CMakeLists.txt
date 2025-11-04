cmake_minimum_required(VERSION 3.24)
project(NativeRenderer)

find_package(OpenGL REQUIRED)

add_subdirectory(lib lib)

# executable
add_executable(native index.cpp)
target_link_libraries(native PRIVATE 
    lib
    SDL3::SDL3
    ${OPENGL_LIBRARIES}
)

target_include_directories(native PUBLIC 
    lib/include
    ${SDL3_INCLUDE_DIRS}
)

# shaders
file(COPY lib/shaders DESTINATION ${CMAKE_BINARY_DIR})
file(COPY assets DESTINATION ${CMAKE_BINARY_DIR})

