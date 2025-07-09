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
set(CMAKE_MODULE_PATH $ENV{VK_RENDERER_HOME})
include(system)

file(GLOB RENDERER_SRC ${VK_RENDERER_HOME}/renderer/*.cpp)
file(GLOB EDITOR_SRC ${CMAKE_CURRENT_LIST_DIR}/src/*.cpp)
set(
    INCLUDES 
    ${OS_HOME}
    ${DEPS_HOME}
    ${SHADERS_HOME}
    ${VK_RENDERER_HOME}/renderer/
    ${CMAKE_CURRENT_LIST_DIR}/
    ${CMAKE_CURRENT_LIST_DIR}/../
    ${CMAKE_CURRENT_LIST_DIR}/src/
    ${CMAKE_CURRENT_LIST_DIR}/../backend/
    ${PROTOBUF_HOME}/src
    ${VULKAN_INCLUDES}
    ${STB_HOME}
    ${NUKLEAR_HOME}
)

message("RENDERER_SRC: ${RENDERER_SRC}")
message("RENDERER_INCLUDES: ${RENDERER_INCLUDES}")

add_executable(
    UI_editor MACOSX_BUNDLE
    ${EDITOR_SRC}
    ${OS_HOME}/glad/glad.cpp
    ../IndieGoUI.cpp
    ../IndieGoUI.pb.cc
    editor_data.pb.cc
    helper_drawing.cpp
    ${RENDERER_SRC}
    ${CMAKE_CURRENT_LIST_DIR}/../backend/renderer.cpp
    ${SHADERS_HOME}/Shader.cpp
)

target_include_directories(
    UI_editor
    PUBLIC 
    ${INCLUDES}
)


# link glfw and opencv libraries
target_link_libraries(
    UI_editor
    PUBLIC
        ${VULKAN_LIBS}
        ${PROTOBUF_LIB}
)

target_compile_definitions(
    UI_editor PUBLIC
    WIDTH=1920
    HEIGHT=1080
    UI_FLT_VAL_SCALE=100
    NDEBUG=1
    TEST_IMAGE_PATH="${VK_RENDERER_HOME}/examples/images/textures/texture.jpg"
    SHADERS_PREFIX="${CMAKE_CURRENT_LIST_DIR}/"
)

target_compile_options(
    UI_editor PUBLIC 
        ${COMPILE_FLAGS}
)