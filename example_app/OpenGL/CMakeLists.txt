cmake_minimum_required(VERSION 3.10)

project(UI_map_OpenGL_Nuklear VERSION 1.0)

if (DEFINED BACKEND)
    message("BACKEND: ${BACKEND}")
    set(RENDERER_SRC ../../backends/${BACKEND}/renderer.cpp)
    set(RENDERER_INCLUDES ../.. ../../os)
    if ("${BACKEND}" MATCHES "imgui")
        file(GLOB IMGUI_SRC_FILES "../../backends/imgui/imgui*.cpp")
        set(RENDERER_SRC ${RENDERER_SRC} ../../backends/imgui/backends/imgui_impl_glfw.cpp ../../backends/imgui/backends/imgui_impl_opengl3.cpp ${IMGUI_SRC_FILES})
        set(RENDERER_INCLUDES ${RENDERER_INCLUDES} ../../backends/imgui)
    endif()
else()
    # Nuklear backend is default
    set(RENDERER_SRC ../../backends/Nuklear/renderer.cpp)
    set(RENDERER_INCLUDES ../.. ../../os)
endif()

message("RENDERER_SRC: ${RENDERER_SRC}")
message("RENDERER_INCLUDES: ${RENDERER_INCLUDES}")

add_executable(
    UI_map_OpenGL_Nuklear
    src/app.cpp
    src/glad.c
    ${RENDERER_SRC}
)

target_include_directories(
    UI_map_OpenGL_Nuklear
    PUBLIC ${RENDERER_INCLUDES}
)

# link glfw library
target_link_libraries(
    UI_map_OpenGL_Nuklear
    PUBLIC "../lib/glfw3"
)
