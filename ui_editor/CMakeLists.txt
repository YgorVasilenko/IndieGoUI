cmake_minimum_required(VERSION 3.10)

project(UI_editor VERSION 1.0)

set(RENDERER_INCLUDES .. ../os)

# protobuf setup
set(PROTOBUF_SRC $ENV{PROTOBUF_HOME}/src)
set(PROTOBUF_LIB $ENV{PROTOBUF_HOME}/build/Debug)

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
    UI_editor
    src/app.cpp
    src/ui.cpp
    src/init.cpp
    src/glad.c
    ../IndieGoUI.cpp
    ../IndieGoUI.pb.cc
    ${RENDERER_SRC}
)

target_include_directories(
    UI_editor
    PUBLIC 
    ${RENDERER_INCLUDES}
    ${PROTOBUF_SRC}
)

# link glfw library
target_link_libraries(
    UI_editor
    PUBLIC "../lib/glfw3"

    # serialization
    PUBLIC ${PROTOBUF_LIB}/libprotobufd.lib
)

target_compile_definitions(
    UI_editor PUBLIC
    WIDTH=1920
    HEIGHT=1080
)

set(CMAKE_CXX_FLAGS_DEBUG " /std:c++17 /DEBUG /Zi")