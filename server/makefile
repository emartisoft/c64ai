CC = gcc
CFLAGS = -O3 -march=native -Wall -Wextra
LDFLAGS = -flto -s

ifeq ($(OS),Windows_NT)
    RM   = del /Q
	TARGET = c64aiserver.exe
else
    RM   = rm -f
	TARGET = c64aiserver
endif

SRC = c64aiserver.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(SRC) -o $(TARGET) $(CFLAGS) $(LDFLAGS)

clean:
	$(RM) *.o
