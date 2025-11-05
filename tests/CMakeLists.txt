cmake_minimum_required(VERSION 3.24)
project(NativeRendererTests)

#set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(OpenGL REQUIRED)

add_subdirectory(../lib lib)

# executable
add_executable(tests tests.cpp)
target_link_libraries(tests PRIVATE 
    lib 
    ${OPENGL_LIBRARIES}
)

target_include_directories(tests PUBLIC 
    lib/include 
    ${OPENGL_INCLUDE_DIRS}
)

