cmake_minimum_required(VERSION 3.13)
project(evogen)

set(CMAKE_CXX_STANDARD 20)

add_compile_options(-Wall -Werror)

add_subdirectory(cmd)
add_subdirectory(libevogen)
