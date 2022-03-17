cmake_minimum_required(VERSION 3.10)

project(UI_map_OpenGL_Nuklear VERSION 1.0)

add_executable( 
    UI_map_OpenGL_Nuklear 
    src/app.cpp 
    src/glad.c 

    # select renderer
    ../../backends/Nuklear/renderer.cpp
)

target_include_directories(
    UI_map_OpenGL_Nuklear
    PUBLIC "../.." 
)

# link glfw library
target_link_libraries(
    UI_map_OpenGL_Nuklear
    PUBLIC "../lib/glfw3"
)