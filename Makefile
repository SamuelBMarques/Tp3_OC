# Set directories
SRC_DIR = src
BUILD_DIR = build

# Compiler and flags
CC = gcc
CFLAGS = -Wall -g

# Find all source files
SRCS = $(wildcard $(SRC_DIR)/*.c)

# Generate object file names by replacing .c with .o and setting the build directory
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

# Default target
all: my_program

# Link the object files into the executable
my_program: $(OBJS)
	$(CC) $(OBJS) -o my_program

# Rule to compile .c to .o
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up build files
clean:
	rm -rf $(BUILD_DIR)/*.o my_program
