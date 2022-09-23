#  runtime state-saving Map for Immediate-mode libraries
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

project(GUI_test VERSION 1.0)

set(RENDERER_INCLUDES .. ../os)
set(RENDERER_SRC ../../IndieGoUI.cpp)
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

# link glfw library
target_link_libraries(
    GUI_test
    PUBLIC "../lib/glfw3"
)

target_compile_definitions(
    GUI_test PUBLIC
    WIDTH=1920
    HEIGHT=1080
    NO_SERIALIZATION=1
)

set(CMAKE_CXX_FLAGS_DEBUG " /std:c++17 /DEBUG /Zi")