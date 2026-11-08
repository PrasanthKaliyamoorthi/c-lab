CC=gcc
CFLAGS= -Wall 
LDFLAGS=
LIBS=-lpthread

All:

clean:
	rm ./main.o ||true

build: clean
	gcc -Wall -Wextra -Wpedantic src/*.c -o main.o -g

testrun: build
	./main.o
dev:testrun

run:

test:
	gcc tests/test.c tests/$(filter-out $@,$(MAKECMDGOALS))* src/$(filter-out $@,$(MAKECMDGOALS))* -o test.o
	./test.o

%:
	@:
