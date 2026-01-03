CC65 = cc65
CA65 = ca65
LD65 = ld65

ifeq ($(OS),Windows_NT)
    RM   = del /Q
else
    RM   = rm -f
endif

TARGET = c64aisetup
SRC_C = $(TARGET).c
SRC_S = $(TARGET).s
OBJ = $(TARGET).o
PRG = $(TARGET).prg

all: $(PRG)

$(SRC_S): $(SRC_C)
	$(CC65) -O -t c64 $(SRC_C)

$(OBJ): $(SRC_S)
	$(CA65) $(SRC_S)

$(PRG): $(OBJ)
	$(LD65) -t c64 $(OBJ) c64.lib -o $(PRG)

clean:
	$(RM) $(SRC_S) $(OBJ)
