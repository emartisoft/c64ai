cmake_minimum_required(VERSION 3.20)

project(c64aisetup NONE)

find_program(CC65 cc65 REQUIRED)
find_program(CA65 ca65 REQUIRED)
find_program(LD65 ld65 REQUIRED)

set(TARGET c64aisetup)

set(SRC_C ${CMAKE_SOURCE_DIR}/${TARGET}.c)
set(ASM_S ${CMAKE_BINARY_DIR}/${TARGET}.s)
set(OBJ_O ${CMAKE_BINARY_DIR}/${TARGET}.o)
set(PRG   ${CMAKE_BINARY_DIR}/${TARGET}.prg)

add_custom_command(
    OUTPUT ${ASM_S}
    COMMAND ${CC65} -O -t c64 ${SRC_C} -o ${ASM_S}
    DEPENDS ${SRC_C}
)

add_custom_command(
    OUTPUT ${OBJ_O}
    COMMAND ${CA65} ${ASM_S} -o ${OBJ_O}
    DEPENDS ${ASM_S}
)

add_custom_command(
    OUTPUT ${PRG}
    COMMAND ${LD65} -t c64 ${OBJ_O} c64.lib -o ${PRG}
    DEPENDS ${OBJ_O}
)

add_custom_target(
    build_prg ALL
    DEPENDS ${PRG}
)

add_custom_target(
    clean_prg
    COMMAND ${CMAKE_COMMAND} -E rm -f
        ${ASM_S}
        ${OBJ_O}
        ${PRG}
)
