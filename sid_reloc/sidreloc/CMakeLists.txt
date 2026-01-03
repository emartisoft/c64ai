cmake_minimum_required(VERSION 3.20)

project(sidreloc C)

# C standard
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# PROFILE değişkeni (istersen: -DPROFILE=...)
set(PROFILE "" CACHE STRING "Extra profiling flags")

# Derleme bayrakları
add_compile_options(
    -Wall
    -O3
    -DNDEBUG
    ${PROFILE}
)

# Executable
add_executable(sidreloc
    sidreloc.c
    solver.c
    cpu.c
)

# Link bayrakları
target_link_libraries(sidreloc
    m
)

target_link_options(sidreloc PRIVATE
    ${PROFILE}
)
