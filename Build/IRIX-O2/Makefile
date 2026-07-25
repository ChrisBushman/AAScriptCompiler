# Native Makefile for IRIX 6.5 / SGI O2 (IP32, MIPS R12000).
# Run this ON the O2 (TGCware's GCC 4.5.2 is the toolchain), or via
# remote-build.sh from a modern dev machine over SSH. No external
# dependencies -- portable ANSI C, no SDL.

REPO_ROOT := $(shell cd ../.. && pwd)
BUILD_DIR := build

CC := /usr/tgcware/gcc45/bin/gcc

OPT_FLAGS := -g -O2
STD_FLAGS := -std=gnu99

TARGET := $(BUILD_DIR)/AAScriptCompiler

.PHONY: all clean

all: $(BUILD_DIR) $(TARGET)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(TARGET): $(REPO_ROOT)/SC.C
	$(CC) $(OPT_FLAGS) $(STD_FLAGS) -x c -o $@ $<

clean:
	rm -rf $(BUILD_DIR)
