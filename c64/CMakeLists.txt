cmake_minimum_required(VERSION 3.20)

# Host compiler yok
project(c64chat NONE)

# -------------------------------------------------
# Toolchain
# -------------------------------------------------
find_program(CC65 cc65 REQUIRED)
find_program(CA65 ca65 REQUIRED)
find_program(LD65 ld65 REQUIRED)
if(WIN32)
    find_program(C1541 c1541.exe REQUIRED)
else()
    find_program(C1541 c1541 REQUIRED)
endif()

# -------------------------------------------------
# Flags & files
# -------------------------------------------------
set(CFLAGS  -O -t c64 -I.)
set(LDFLAGS -C ${CMAKE_SOURCE_DIR}/c64-ai.cfg)
set(LIBS    c64.lib)

set(TARGET    c64chat.prg)
set(D64_IMAGE c64ai.d64)

# -------------------------------------------------
# Sources
# -------------------------------------------------
set(C_SOURCES
    c64chat.c
    c64dir.c
    c64mouse.c
    c64sid.c
    page.c
    aimem.c
    textbox.c
    font.c
)

set(ASM_SOURCES
    charset.asm
    sidfunc.asm
    irq.asm
    menu.asm
    setupscreen.asm
)

set(OBJS)

# -------------------------------------------------
# C -> S -> O
# -------------------------------------------------
foreach(CFILE ${C_SOURCES})
    get_filename_component(BASE ${CFILE} NAME_WE)

    set(SFILE ${CMAKE_BINARY_DIR}/${BASE}.s)
    set(OFILE ${CMAKE_BINARY_DIR}/${BASE}.o)

    add_custom_command(
        OUTPUT ${SFILE}
        COMMAND ${CC65}
                -O -t c64 -I.
                -o ${SFILE}
                ${CMAKE_SOURCE_DIR}/${CFILE}
        DEPENDS ${CMAKE_SOURCE_DIR}/${CFILE}
        COMMENT "cc65 ${CFILE}"
    )

    add_custom_command(
        OUTPUT ${OFILE}
        COMMAND ${CA65}
                ${SFILE}
                -o ${OFILE}
        DEPENDS ${SFILE}
        COMMENT "ca65 ${BASE}.s"
    )

    list(APPEND OBJS ${OFILE})
endforeach()

# -------------------------------------------------
# ASM -> O
# -------------------------------------------------
foreach(AFILE ${ASM_SOURCES})
    get_filename_component(BASE ${AFILE} NAME_WE)
    set(OFILE ${CMAKE_BINARY_DIR}/${BASE}.o)

    add_custom_command(
        OUTPUT ${OFILE}
        COMMAND ${CA65} ${CMAKE_SOURCE_DIR}/${AFILE} -o ${OFILE}
        DEPENDS ${AFILE}
        COMMENT "ca65 ${AFILE}"
    )

    list(APPEND OBJS ${OFILE})
endforeach()

# -------------------------------------------------
# Link
# -------------------------------------------------
set(TARGET_BIN ${CMAKE_BINARY_DIR}/${TARGET})

add_custom_command(
    OUTPUT ${TARGET_BIN}
    COMMAND ${LD65} ${OBJS} ${LIBS} ${LDFLAGS} -o ${TARGET_BIN}
    DEPENDS ${OBJS}
    COMMENT "ld65 linking ${TARGET}"
)

add_custom_target(link ALL
    DEPENDS ${TARGET_BIN}
)

# -------------------------------------------------
# D64 paths
# -------------------------------------------------
set(D64_TEMPLATE ${CMAKE_SOURCE_DIR}/disk/blank.d64)
set(D64_PATH     ${CMAKE_BINARY_DIR}/${D64_IMAGE})
set(PRG_COPY_OUT ${CMAKE_BINARY_DIR}/../${TARGET})

# -------------------------------------------------
# Create D64 if missing
# -------------------------------------------------
add_custom_command(
    OUTPUT ${D64_PATH}
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${D64_TEMPLATE}
            ${D64_PATH}
    DEPENDS ${D64_TEMPLATE}
    COMMENT "copying blank.d64 to build directory"
)

# -------------------------------------------------
# Write PRG to D64
# -------------------------------------------------
add_custom_target(d64 ALL
    DEPENDS ${TARGET_BIN} ${D64_PATH}

    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${TARGET_BIN}
            ${PRG_COPY_OUT}

    COMMAND ${C1541}
            -silent on
            -attach ${D64_PATH}
            -delete c64chat

    COMMAND ${C1541}
            -silent on
            -attach ${D64_PATH}
            -write ${TARGET_BIN} c64chat
			
	COMMAND ${C1541}
            -silent on
            -attach ${D64_PATH}
            -delete help

    COMMAND ${C1541}
            -silent on
            -attach ${D64_PATH}
            -write ${TARGET_BIN} help

    COMMENT "writing ${TARGET} & HELP to ${D64_IMAGE}"
)

# -------------------------------------------------
# Default
# -------------------------------------------------
add_custom_target(all_build ALL
    DEPENDS link d64
)

# -------------------------------------------------
# Clean
# -------------------------------------------------
add_custom_target(clean_all
    COMMAND ${CMAKE_COMMAND} -E rm -f
            ${OBJS}
            ${TARGET_BIN}
    COMMENT "clean"
)
