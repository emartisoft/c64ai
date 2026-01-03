CC      = gcc
CFLAGS  = -Wall -O2
LDFLAGS = -lgpiod

TARGET  = gpio_shutdown_led
SRC     = gpio_shutdown_led.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS)

clean:
	rm -f $(TARGET)
