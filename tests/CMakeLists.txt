cmake_minimum_required(VERSION 3.24)
project(NativeRendererTests)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(OpenGL REQUIRED)

add_subdirectory(../lib lib)
add_subdirectory(../mym mym)

# executable
add_executable(tests 
    tests.cpp 
    test_helpers.cpp
    raycast_scene_tests.cpp
    raycast_triangle_tests.cpp
    raycast_vertices_tests.cpp
    )

target_link_libraries(tests PRIVATE 
    mym
    lib 
)

target_include_directories(tests PUBLIC include)

