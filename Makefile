# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -g -Iinclude  # Compiler flags
LDFLAGS = -lcjson -lcurl  # Linker flags

# Source files (add more as needed)
SRCS = src/main.c src/parser.c src/display.c
OBJS = $(SRCS:src/%.c=build/%.o)  # Object files in build directory

# Executable name
TARGET = pokefetch

# Default target (builds the shell)
all: $(TARGET)

# Rule to build the executable
$(TARGET): $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

# Rule to compile source files into object files
build/%.o: src/%.c | build
	$(CC) $(CFLAGS) -c $< -o $@

# Ensure the build directory exists
build:
	mkdir -p build

# Rule to copy assets (icons) to the build directory
icon:
	rm -rf assets/icons

	mkdir -p assets/icons
	mkdir -p assets/icons/regular
	mkdir -p assets/icons/shiny
	python3 get_icons.py

# Clean target (removes object files and executable)
clean:
	rm -f $(OBJS) $(TARGET)
	rm -rf build

# Phony targets (always run, even if a file with the same name exists)
.PHONY: all clean build

