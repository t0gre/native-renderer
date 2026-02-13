cmake_minimum_required(VERSION 3.24)
project(NativeRenderer)

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -g")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(OpenGL REQUIRED)

option( TRACY_ENABLE "" ON)
add_subdirectory(third_party/tracy)
add_subdirectory(lib lib)
add_subdirectory(mym mym)
add_subdirectory(mygl mygl)

# imgui 
set(IMGUI_PATH  "${CMAKE_SOURCE_DIR}/third_party/imgui")

file(GLOB IMGUI_SOURCES 
        ${IMGUI_PATH}/*.cpp 
        ${IMGUI_PATH}/backends/imgui_impl_sdl3.cpp
        ${IMGUI_PATH}/backends/imgui_impl_opengl3.cpp
        )

add_library(ImGui STATIC ${IMGUI_SOURCES})
target_include_directories(ImGui PUBLIC ${IMGUI_PATH} ${IMGUI_PATH}/backends/)
target_link_libraries(ImGui PRIVATE SDL3::SDL3)

# executable
add_executable(native index.cpp events.cpp)

target_link_libraries(native PRIVATE 
    mym
    mygl
    lib
    SDL3::SDL3
    ${OPENGL_LIBRARIES}
    Tracy::TracyClient
    ImGui
)

target_include_directories(native PUBLIC include)


# shaders copy on configure
file(COPY assets DESTINATION ${CMAKE_BINARY_DIR}) # only runs on configure

add_custom_command(
         TARGET native POST_BUILD
         COMMAND ${CMAKE_COMMAND} -E copy_directory              
          ${CMAKE_SOURCE_DIR}/lib/shaders ${CMAKE_BINARY_DIR}/shaders
         COMMENT "Copying shaders to build directory" VERBATIM
         )
