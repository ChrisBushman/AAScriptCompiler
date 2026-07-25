# Native Makefile for PowerPC / Mac OS X 10.4 "Tiger".
# Run this ON the Tiger box, or via remote-build.sh from a modern dev
# machine over SSH. No external dependencies -- portable ANSI C, no SDL.

REPO_ROOT := $(abspath ../..)
BUILD_DIR := build

CC ?= gcc
ARCH ?= ppc
MACOSX_VERSION_MIN ?= 10.4
SDKROOT ?=

ARCHFLAGS := -arch $(ARCH) -mmacosx-version-min=$(MACOSX_VERSION_MIN)
ifneq ($(strip $(SDKROOT)),)
ARCHFLAGS += -isysroot $(SDKROOT)
endif

OPT_FLAGS := -g -O2
STD_FLAGS := -std=gnu99

TARGET := $(BUILD_DIR)/AAScriptCompiler

.PHONY: all clean

all: $(BUILD_DIR) $(TARGET)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(TARGET): $(REPO_ROOT)/SC.C
	$(CC) $(ARCHFLAGS) $(OPT_FLAGS) $(STD_FLAGS) -x c -o $@ $<

clean:
	rm -rf $(BUILD_DIR)
