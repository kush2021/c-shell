# Copyright (c) 2026 Kush Padalia. All Rights Reserved.

CC = clang
SRC = src/main.c
TARGET = build/csh

# Debug Build
CFLAGS = -std=c17 \
				 -Wall \
				 -Wextra \
				 -Wpedantic \
				 -Werror \
				 -Wshadow \
				 -Wunreachable-code \
				 -g \
				 -Og \
				 -fsanitize=address,undefined

RELEASE_FLAGS = -std=c17 -Wall -Wextra -O2

all: $(TARGET)

$(TARGET): $(SRC)
	mkdir -p $(dir $(TARGET))
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

release:
	mkdir -p $(dir $(TARGET))
	$(CC) $(RELEASE_FLAGS) -o $(TARGET) $(SRC)

run: $(TARGET)
	./$(TARGET)

test: $(TARGET)
	./test/run_tests.sh

clean:
	rm -rf ./build/*

compile_commands:
	bear --output compile_commands.json -- $(MAKE) all

.PHONY: all release run test clean compile_commands
