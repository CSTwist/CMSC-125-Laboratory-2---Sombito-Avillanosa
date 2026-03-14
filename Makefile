# Compiler and Flags
CC = gcc
CFLAGS = -Wall -Wextra -g -Iinclude

# Directories
SRC_DIR = src
INC_DIR = include
TEST_DIR = tests

# Target Executable
TARGET = schedsim

# Find all .c files in the src/ directory
SRCS = $(wildcard $(SRC_DIR)/*.c)

# Generate corresponding .o object files
OBJS = $(SRCS:.c=.o)

# Default target to compile the simulator
all: $(TARGET)

# Link all object files to create the final executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# Compile each .c source file into a .o object file
$(SRC_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean target to remove binaries and object files
clean:
	rm -f $(OBJS) $(TARGET)

# Test target to run the automated test suite
test: $(TARGET)
	bash $(TEST_DIR)/test_suite.sh

# Declare phony targets to prevent conflicts with files named "all", "clean", or "test"
.PHONY: all clean test