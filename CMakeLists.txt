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

# Include directories (platform-independent)
target_include_directories(MyApp PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/include/imgui
    ${CMAKE_SOURCE_DIR}/include/imgui/backends
)

# Platform-specific settings
if(WIN32)
    message(STATUS "Configuring for Windows")

    # Path to your Windows static GLFW
    set(GLFW_LIB "${CMAKE_SOURCE_DIR}/lib/windows/libglfw3.a")

    target_link_libraries(MyApp PRIVATE
        ${GLFW_LIB}
        opengl32
        gdi32
        user32
        kernel32
        winmm
    )

elseif(UNIX)
    message(STATUS "Configuring for Linux")

    set(GLFW_LIB "${CMAKE_SOURCE_DIR}/lib/linux/libglfw3.a")

    target_link_libraries(MyApp PRIVATE
        ${GLFW_LIB}
        pthread
        dl
        GL
    )
endif()

