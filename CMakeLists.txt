cmake_minimum_required(VERSION 3.15)
project(MyApp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ImGui sources
file(GLOB IMGUI_SOURCES
    include/imgui/*.cpp
    include/imgui/backends/imgui_impl_glfw.cpp
    include/imgui/backends/imgui_impl_opengl3.cpp
)

# Your source files
file(GLOB_RECURSE SOURCES src/*.cpp)

add_executable(MyApp ${SOURCES} ${IMGUI_SOURCES})

# Include directories
target_include_directories(MyApp PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    include
    include/imgui
    include/imgui/backends
    include/GLFW
)

# Link static GLFW and system libraries (Linux)
target_link_libraries(MyApp PRIVATE
    ${CMAKE_SOURCE_DIR}/lib/libglfw3.a
    pthread
    dl
    GL
)

