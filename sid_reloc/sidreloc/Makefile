CC=gcc
PROFILE=
CFLAGS=-Wall -std=c99 -O3 -DNDEBUG ${PROFILE}
LDFLAGS=-lm ${PROFILE}

ifeq ($(OS),Windows_NT)
    RM   = del /Q
else
    RM   = rm -f
endif

all:		sidreloc

.PHONY:		all clean

sidreloc:	sidreloc.o solver.o cpu.o
		gcc -o $@ $^ ${LDFLAGS}

clean:
		$(RM) *.o reloc
