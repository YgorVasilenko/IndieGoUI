#   runtime state-saving Map for Immediate-mode libraries
#     Copyright (C) 2022  Igor Vasilenko

#     This program is free software: you can redistribute it and/or modify
#     it under the terms of the GNU General Public License as published by
#     the Free Software Foundation, either version 3 of the License, or
#     (at your option) any later version.

#     This program is distributed in the hope that it will be useful,
#     but WITHOUT ANY WARRANTY; without even the implied warranty of
#     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#     GNU General Public License for more details.

#     You should have received a copy of the GNU General Public License
#     along with this program.  If not, see <https://www.gnu.org/licenses/>.

cmake_minimum_required(VERSION 3.10)

project(UI_editor VERSION 1.0)

# protobuf setup
set(PROTOBUF_SRC ${CMAKE_CURRENT_LIST_DIR}/../protobuf/src)
set(PROTOBUF_LIB ${CMAKE_CURRENT_LIST_DIR}/../protobuf/build)

if (${CMAKE_HOST_SYSTEM_NAME} MATCHES Windows)
    set(PROTOBUF_LIB ${CMAKE_CURRENT_LIST_DIR}/../protobuf/build/Win)
else()
    message(FATAL_ERROR "Only Windows currently supported!")
endif()

set(RENDERER_INCLUDES .. ../os ${PROTOBUF_SRC})

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
    UI_FLT_VAL_SCALE=100
)

set(CMAKE_CXX_FLAGS_DEBUG " /std:c++17 /DEBUG /Zi")