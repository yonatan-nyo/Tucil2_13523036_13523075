CC = g++
CFLAGS = -Wall -Werror -std=c++17  

# Compile all C++ files in the src, src/*, and src/*/* directories
build:
	$(CC) $(CFLAGS) $(wildcard src/*.cpp src/*/*.cpp src/*/*/*.cpp) -o bin/main

# Run the compiled program
run:
	./bin/main

# Compile and run the program
all: build run

# Clean up the compiled program
clean:
	rm -f bin/main

# Run the program with Valgrind for memory leak detection
debug: build
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./bin/main
