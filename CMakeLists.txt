cmake_minimum_required(VERSION 3.15)
project(MonteCarlo LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
    foreach(flag_var
            CMAKE_C_FLAGS_RELEASE
            CMAKE_C_FLAGS_DEBUG
            CMAKE_CXX_FLAGS_RELEASE
            CMAKE_CXX_FLAGS_DEBUG)
        if(${flag_var} MATCHES "/MD")
            string(REGEX REPLACE "/MD" "/MT" ${flag_var} "${${flag_var}}")
        endif()
    endforeach()
endif()

# GLFW
if(WIN32)
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
    set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE) # static lib
    add_subdirectory(external/GLFW)
endif()

# ImGui sources
file(GLOB IMGUI_SOURCES
    include/imgui/*.cpp
    include/imgui/backends/imgui_impl_glfw.cpp
    include/imgui/backends/imgui_impl_opengl3.cpp
)

add_library(imgui STATIC ${IMGUI_SOURCES})
target_include_directories(imgui PRIVATE
    include/imgui
    include/imgui/backends
    external/GLFW/include
)

file(GLOB_RECURSE SOURCES
    src/main.cpp
    src/gui/gui.cpp
    src/math/random.cpp
    src/math/result.cpp
)

if(WIN32)
    add_executable(MonteCarlo WIN32 ${SOURCES})  
else()
    add_executable(MonteCarlo ${SOURCES})
endif()

# Include directories
target_include_directories(MonteCarlo PRIVATE
    src
    include
    include/imgui
    include/imgui/backends
)

# Link libraries
if(WIN32)
    target_link_libraries(MonteCarlo PRIVATE imgui glfw opengl32 gdi32 user32 kernel32 winmm)
elseif(UNIX)
    set(GLFW_LIB "${CMAKE_SOURCE_DIR}/lib/linux/libglfw3.a")
    target_link_libraries(MonteCarlo PRIVATE imgui ${GLFW_LIB} pthread dl GL)
endif()
