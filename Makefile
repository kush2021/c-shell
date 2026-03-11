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
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

release:
	$(CC) $(RELEASE_FLAGS) -o $(TARGET) $(SRC)

run: $(TARGET)
	./$(TARGET)

test: $(TARGET)
	./test/run_tests.sh

clean:
	rm -rf ./build/*

.PHONY: all release run test clean
