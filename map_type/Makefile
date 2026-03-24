CC = gcc
CFLAGS = -Iinclude -Wall -Wextra -DTESTING

SRC = src/main.c
TESTS = tests/test_map.c
EXE = test_map

all: $(EXE)

$(EXE): $(SRC) $(TESTS)
	$(CC) $(CFLAGS) -o $@ $^

test: $(EXE)
	./$(EXE)

clean:
	rm -f $(EXE)

.PHONY: all test clean
