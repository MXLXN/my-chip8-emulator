CC = gcc
CFLAGS = -g -Wall -Wextra -pedantic -std=c99
TARGET = main

$(TARGET): main.c chip8.h chip8.c screen.h screen.c
	$(CC) $(CFLAGS) main.c chip8.c screen.c `pkg-config --cflags --libs sdl2` -o $(TARGET)

clean:
	rm -f $(TARGET)

.PHONY: clean
