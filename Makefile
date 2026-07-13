# Copyright (c) 2026 Kush Padalia. All Rights Reserved.

CC = clang

TARGET = build/csh

SRC = $(shell find src -name '*.c')

OBJ = $(SRC:src/%.c=build/%.o)
DEP = $(OBJ:.o=.d)

CFLAGS = -D_POSIX_C_SOURCE=200809L \
				 -std=c23 \
         -Wall \
         -Wextra \
         -Wpedantic \
         -Werror \
         -Wshadow \
         -Wunreachable-code \
         -g \
         -Og \
         -fsanitize=address,undefined \
         -MMD \
         -MP

RELEASE_FLAGS = -D_POSIX_C_SOURCE=200809L \
								-std=c23 \
                -Wall \
                -Wextra \
                -O2 \
                -MMD \
                -MP

all: $(TARGET)

$(TARGET): $(OBJ)
	mkdir -p $(dir $(TARGET))
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)

build/%.o: src/%.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

release: CFLAGS = $(RELEASE_FLAGS)
release: clean $(TARGET)

run: $(TARGET)
	./$(TARGET)

test: $(TARGET)
	./test/run_tests.sh

clean:
	rm -rf build

compile_commands:
	bear --output compile_commands.json -- $(MAKE) all

-include $(DEP)

.PHONY: all release run test clean compile_commands

