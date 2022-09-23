cmake_minimum_required(VERSION 3.10)

project(GUI_test VERSION 1.0)

# protobuf setup
set(PROTOBUF_SRC ${CMAKE_CURRENT_LIST_DIR}/../protobuf/src)
set(PROTOBUF_LIB ${CMAKE_CURRENT_LIST_DIR}/../protobuf/build)

if (CMAKE_HOST_SYSTEM_NAME MATCHES Windows)
    set(PROTOBUF_LIB ${CMAKE_CURRENT_LIST_DIR}/../protobuf/build/Win)
else()
    message(FATAL_ERROR "Only Windows currently supported!")
endif()

set(RENDERER_INCLUDES .. ../os ${PROTOBUF_SRC})
set(RENDERER_SRC ../../IndieGoUI.cpp ../../IndieGoUI.pb.cc)

if (DEFINED BACKEND)
    message("BACKEND: ${BACKEND}")
    set(RENDERER_SRC ${RENDERER_SRC} ../../backends/${BACKEND}/renderer.cpp)
    if ("${BACKEND}" MATCHES "imgui")
        file(GLOB IMGUI_SRC_FILES "../../backends/imgui/imgui*.cpp")
        set(RENDERER_SRC ${RENDERER_SRC} ../../backends/imgui/backends/imgui_impl_glfw.cpp ../../backends/imgui/backends/imgui_impl_opengl3.cpp ${IMGUI_SRC_FILES})
        set(RENDERER_INCLUDES ${RENDERER_INCLUDES} ../../backends/imgui)
    endif()
else()
    # Nuklear backend is default
    set(RENDERER_SRC ${RENDERER_SRC} ../../backends/Nuklear/renderer.cpp)
endif()

message("RENDERER_SRC: ${RENDERER_SRC}")
message("RENDERER_INCLUDES: ${RENDERER_INCLUDES}")

add_executable(
    GUI_test
    src/app.cpp
    src/glad.c
    ${RENDERER_SRC}
)

target_include_directories(
    GUI_test
    PUBLIC 
    ${RENDERER_INCLUDES}
)

target_link_libraries(
    GUI_test
    PUBLIC "../lib/glfw3"

    # serialization
    PUBLIC ${PROTOBUF_LIB}/libprotobufd.lib
)

target_compile_definitions(
    GUI_test PUBLIC
    WIDTH=1920
    HEIGHT=1080
)

set(CMAKE_CXX_FLAGS_DEBUG " /std:c++17 /DEBUG /Zi")