cmake_minimum_required(VERSION 3.10)

project(UI_map_func_test VERSION 1.0)

add_executable( UI_map_func_test main.cpp )

target_include_directories(
    UI_map_func_test
    PUBLIC ".." 
)