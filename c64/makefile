# =================================================
# Make default C kurallarini kapat
# =================================================
.SUFFIXES:

# =================================================
# Toolchain
# =================================================
CC65 = cc65
CA65 = ca65
LD65 = ld65

ifeq ($(OS),Windows_NT)
    C1541 = c1541.exe
else
    C1541 = c1541
endif

# =================================================
# Flags
# =================================================
CFLAGS  = -O -t c64 -I.
LDFLAGS = -C c64-ai.cfg
LIBS    = c64.lib

# =================================================
# Targets & files
# =================================================
TARGET    = c64chat.prg
D64_IMAGE = c64ai.d64

# =================================================
# Sources
# =================================================
C_SOURCES = \
	c64chat.c \
	c64dir.c \
	c64mouse.c \
	c64sid.c \
	page.c \
	aimem.c \
	textbox.c \
	font.c

ASM_SOURCES = \
	charset.asm \
	sidfunc.asm \
	irq.asm \
	menu.asm \
	setupscreen.asm

# =================================================
# Generated files
# =================================================
S_FILES  = $(C_SOURCES:.c=.s)
OBJ_C   = $(S_FILES:.s=.o)
OBJ_ASM = $(ASM_SOURCES:.asm=.o)

OBJS = $(OBJ_C) $(OBJ_ASM)

# =================================================
# OS-aware remove
# =================================================
ifeq ($(OS),Windows_NT)
    RM = del /Q
    CP = copy
else
    RM = rm -f
    CP = cp
endif

# =================================================
# Rules
# =================================================
all: $(TARGET) d64

# -------------------------------------------------
# C -> ASM
%.s: %.c
	$(CC65) $(CFLAGS) $<

# ASM (.s) -> OBJ
%.o: %.s
	$(CA65) $<

# ASM (.asm) -> OBJ
%.o: %.asm
	$(CA65) $<

# -------------------------------------------------
# Link
$(TARGET): $(OBJS)
	$(LD65) $(OBJS) $(LIBS) $(LDFLAGS) -o $(TARGET)

# -------------------------------------------------
# D64 image
BLANK_D64 = disk/blank.d64

$(D64_IMAGE):
	$(CP) $(BLANK_D64) $(D64_IMAGE)

d64: $(D64_IMAGE) $(TARGET)
	$(C1541) -silent on -attach $(D64_IMAGE) -delete c64chat
	$(C1541) -silent on -attach $(D64_IMAGE) -write $(TARGET) c64chat
	$(C1541) -silent on -attach $(D64_IMAGE) -delete help
	$(C1541) -silent on -attach $(D64_IMAGE) -write $(TARGET) help
# =================================================
# Clean
# =================================================
clean:
	$(RM) *.o *.s $(TARGET)

.PHONY: all clean d64
