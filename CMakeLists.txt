cmake_minimum_required(VERSION 3.24)
project(NativeRenderer)

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -g")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(OpenGL REQUIRED)

if(CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    set(TRACY_ENABLE ON CACHE BOOL "" FORCE)
else()
    set(TRACY_ENABLE OFF CACHE BOOL "" FORCE)
endif()

add_subdirectory(third_party/tracy)
add_subdirectory(scene scene)
add_subdirectory(linalg linalg)
add_subdirectory(gl_render gl_render)

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
    linalg
    gl_render
    scene
    SDL3::SDL3
    ${OPENGL_LIBRARIES}
    Tracy::TracyClient
    ImGui
)

target_include_directories(native PUBLIC include)


# assets copy on configure
file(COPY assets DESTINATION ${CMAKE_BINARY_DIR}) # only runs on configure

# shaders copy on configure
add_custom_command(
         TARGET native POST_BUILD
         COMMAND ${CMAKE_COMMAND} -E copy_directory              
          ${CMAKE_SOURCE_DIR}/gl_render/shaders ${CMAKE_BINARY_DIR}/shaders
         COMMENT "Copying shaders to build directory" VERBATIM
         )


####################### tests ##############


# executable
add_executable(tests 
    tests/tests.cpp 
    tests/test_helpers.cpp
    tests/raycast_scene_tests.cpp
    tests/raycast_triangle_tests.cpp
    tests/raycast_vertices_tests.cpp
    )

target_link_libraries(tests PRIVATE 
    linalg
    scene 
)

target_include_directories(tests PUBLIC tests/include)

