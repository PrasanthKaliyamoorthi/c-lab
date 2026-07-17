CC=gcc

CFLAGS=-Wall -Wextra -Wpedantic -g

SRC=$(wildcard src/*.c)

TARGET=ctask

all:
	$(CC) $(CFLAGS) $(SRC) -lncurses -Iinclude -o build/$(TARGET)

clean:
	rm -f $(TARGET)
