cmake_minimum_required(VERSION 3.20)

project(c64aiserver C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_executable(c64aiserver c64aiserver.c)

target_compile_options(c64aiserver PRIVATE
    -O3
    -march=native
    -Wall
    -Wextra
)

target_link_options(c64aiserver PRIVATE
    -flto
    -s
)
