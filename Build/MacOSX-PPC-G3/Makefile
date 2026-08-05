# Native Makefile for PowerPC G3 / Mac OS X 10.3 "Panther".
#
# Separate G3 platform (distinct from Build/MacOSX-PPC, which targets a G4
# Tiger box). It pins a Panther-native toolchain -- Apple's own gcc 4.0 against
# the 10.3.9 SDK, generic-G3 codegen (-mcpu=750 -mno-altivec) -- so the binary
# links Panther's system libSystem/libstdc++ (current version 71.x), NOT
# Tigerbrew gcc-14's 10.4 runtime (which pulls $UNIX2003/$LDBL128 conformance
# variants and Tiger-only stdio globals that don't exist on Panther), and
# contains no AltiVec/VRSAVE instructions a G3 (ppc750) can't execute. The
# resulting binary runs on a G3 as well as G4/G5.
#
# Build ON the Tiger box (it carries /Developer/SDKs/MacOSX10.3.9.sdk and Apple
# gcc 4.0), or via remote-build.sh. No external dependencies -- portable ANSI C.

REPO_ROOT := $(abspath ../..)
BUILD_DIR := build

# := not ?=: CC is a GNU-make *built-in* variable (cc), so ?= would never
# override it. := beats the built-in default but still loses to a command-line
# `make CC=...` override.
CC := /usr/bin/gcc-4.0
ARCH ?= ppc
MACOSX_VERSION_MIN ?= 10.3
SDKROOT ?= /Developer/SDKs/MacOSX10.3.9.sdk
G3FLAGS ?= -mcpu=750 -mno-altivec

ARCHFLAGS := -arch $(ARCH) -mmacosx-version-min=$(MACOSX_VERSION_MIN) $(G3FLAGS)
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
