CC = gcc
CFLAGS = -Wall ##-I./include
SRC_DIR = ./src
BUILD_DIR = ./build

# List all source files (excluding main.c)
SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(filter-out $(BUILD_DIR)/main.o, $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SRCS)))

TARGET = brainfuck

$(BUILD_DIR)/$(TARGET): $(OBJS) $(BUILD_DIR)/main.o
	$(CC) $(CFLAGS) $^ -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean

clean:
	rm -rf $(BUILD_DIR)/*.o $(BUILD_DIR)/$(TARGET)
