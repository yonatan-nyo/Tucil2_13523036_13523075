CC = gcc
CFLAGS = -Wall -Werror -std=c17

# Compile all C files in the src, src/* and src/*/* directories
build:
	$(CC) $(CFLAGS) $(wildcard src/*.c src/*/*.c src/*/*/*.c) -o bin/main

# Run the compiled program
run:
	./bin/main

# Compile and run the program
all: build run

# Clean up the compiled program
clean:
	rm bin/main