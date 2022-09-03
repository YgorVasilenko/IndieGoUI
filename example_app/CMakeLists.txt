cmake_minimum_required(VERSION 3.10)

project(UI_map_OpenGL VERSION 1.0)

set(RENDERER_INCLUDES .. ../os)

# set(CMAKE_MODULE_PATH $ENV{CMAKE_MODULE_PATH})
# include(common)

if (DEFINED BACKEND)
    message("BACKEND: ${BACKEND}")
    set(RENDERER_SRC ../../backends/${BACKEND}/renderer.cpp)
    if ("${BACKEND}" MATCHES "imgui")
        file(GLOB IMGUI_SRC_FILES "../../backends/imgui/imgui*.cpp")
        set(RENDERER_SRC ${RENDERER_SRC} ../../backends/imgui/backends/imgui_impl_glfw.cpp ../../backends/imgui/backends/imgui_impl_opengl3.cpp ${IMGUI_SRC_FILES})
        set(RENDERER_INCLUDES ${RENDERER_INCLUDES} ../../backends/imgui)
    endif()
else()
    # Nuklear backend is default
    set(RENDERER_SRC ../../backends/Nuklear/renderer.cpp)
endif()

message("RENDERER_SRC: ${RENDERER_SRC}")
message("RENDERER_INCLUDES: ${RENDERER_INCLUDES}")

add_executable(
    UI_map_OpenGL
    src/app.cpp
    src/glad.c
    ${RENDERER_SRC}
)

target_include_directories(
    UI_map_OpenGL
    PUBLIC 
    ${RENDERER_INCLUDES}
)

# link glfw library
target_link_libraries(
    UI_map_OpenGL
    PUBLIC "../lib/glfw3"
)

set(CMAKE_CXX_FLAGS_DEBUG " /std:c++17 /DEBUG /Zi")

if(DEFINED CI_BUILD)
    message(STATUS "Building for ci...")
    target_compile_definitions(
        UI_map_OpenGL PRIVATE CI_BUILD=1
    )
endif()