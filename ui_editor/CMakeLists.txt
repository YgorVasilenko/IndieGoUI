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

message(STATUS "CMAKE_HOST_SYSTEM_NAME: ${CMAKE_HOST_SYSTEM_NAME}")

if (${CMAKE_HOST_SYSTEM_NAME} MATCHES Windows)
    set(PROTOBUF_LIB ${CMAKE_CURRENT_LIST_DIR}/../protobuf/build/Win)
    set(CMAKE_CXX_FLAGS_DEBUG " /std:c++17 /DEBUG /Zi /Od")
else()
    set(PROTOBUF_LIBS ${CMAKE_CURRENT_LIST_DIR}/../protobuf/build/Darwin/libprotobuf.a ${CMAKE_CURRENT_LIST_DIR}/../protobuf/build/Darwin/libprotobuf-lite.a ${CMAKE_CURRENT_LIST_DIR}/../protobuf/build/Darwin/libprotoc.a)

    # Mac OS custom static glfw lib
    set(GLFW_LIB_NAME libglfw3.a)
    set(GLFW_VERSION_MAJOR 3) 
    set(GLFW_VERSION_MINOR 4)
    set(GLFW_VERSION 3.4)
    set(GLFW_SOURCE_DIR $ENV{GLFW_PATH})
    set(GLAD_GL "${GLFW_SOURCE_DIR}/deps")

    # glfw requirements
    find_package(Threads REQUIRED)
    find_package(OpenGL REQUIRED)

    add_subdirectory($ENV{GLFW_PATH} $ENV{GLFW_PATH}/build)
endif()

set(RENDERER_INCLUDES src . .. ../os ${PROTOBUF_SRC})

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
    UI_editor WIN32 MACOSX_BUNDLE
    src/app.cpp
    src/ui.cpp
    src/init.cpp
    src/callbacks.cpp
    src/helper_drawing.cpp
    src/skinning.cpp
    src/Shader.cpp
    src/glad.cpp
    ../IndieGoUI.cpp
    ../IndieGoUI.pb.cc
    editor_data.pb.cc
    ${RENDERER_SRC}
)

target_include_directories(
    UI_editor
    PUBLIC 
    ${RENDERER_INCLUDES}
)

if (${CMAKE_HOST_SYSTEM_NAME} MATCHES Darwin)
    set_source_files_properties(
        src/glfw.icns PROPERTIES
            MACOSX_PACKAGE_LOCATION "Resources"
    )

    set_target_properties(
        UI_editor PROPERTIES
            MACOSX_BUNDLE_BUNDLE_NAME "UI_editor"
            MACOSX_BUNDLE_SHORT_VERSION_STRING ${GLFW_VERSION}
            MACOSX_BUNDLE_LONG_VERSION_STRING ${GLFW_VERSION}
            MACOSX_BUNDLE_ICON_FILE src/glfw.icns
            MACOSX_BUNDLE_INFO_PLIST "${GLFW_SOURCE_DIR}/CMake/Info.plist.in"
    )

    target_compile_options(
        UI_editor PUBLIC 
            -framework OpenGL -std=c++17 -O0 -g
    )

    target_link_libraries(
        UI_editor

        PUBLIC glfw
        PUBLIC OpenGL::GL
        
        # serialization
        PUBLIC ${PROTOBUF_LIBS}
    )
else()
    target_link_libraries(
        UI_editor
        PUBLIC "../lib/glfw3"

        # serialization
        PUBLIC ${PROTOBUF_LIB}/libprotobufd.lib
    )    
endif()

target_compile_definitions(
    UI_editor PUBLIC
    WIDTH=1920
    HEIGHT=1080
    UI_FLT_VAL_SCALE=100
    NDEBUG=1
)