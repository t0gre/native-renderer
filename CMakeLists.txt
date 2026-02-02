cmake_minimum_required(VERSION 3.24)
project(NativeRenderer)

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -g")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

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
    third_party/SDL/include
)


# shaders copy on configure
file(COPY assets DESTINATION ${CMAKE_BINARY_DIR}) # only runs on configure

add_custom_command(
         TARGET native POST_BUILD
         COMMAND ${CMAKE_COMMAND} -E copy_directory              
          ${CMAKE_SOURCE_DIR}/lib/shaders ${CMAKE_BINARY_DIR}/shaders
         COMMENT "Copying shaders to build directory" VERBATIM
         )
