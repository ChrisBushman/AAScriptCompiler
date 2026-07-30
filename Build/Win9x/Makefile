# Cross-compiled Makefile for Windows 95/98/ME (i686). See the
# AmuletsArmor repo's Build/Win9x/Makefile for the full toolchain
# rationale (Ubuntu's gcc-mingw-w64-i686, not Homebrew's UCRT-locked
# one). Run on Linux/macOS with a mingw-w64 cross toolchain; there is no
# way to build (or run) this target on the box it's actually for.
#
# The easiest of the four Win9x ports: SC.C is plain ANSI C (stdio.h/
# stdlib.h/ctype.h/string.h only, a real `int main(argc, argv)`) with no
# SDL dependency and nothing Windows-specific at all, so there's no
# SDL_main rename to work around, no SDL/SDL_net to rebuild, and no
# WINVER-gated API surface to worry about.

REPO_ROOT := $(shell cd ../.. && pwd)

BUILD_DIR := build

CC := i686-w64-mingw32-gcc

TARGET := $(BUILD_DIR)/AAScriptCompiler.exe

.PHONY: all clean package

all: $(BUILD_DIR) $(TARGET)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# -x c: SC.C's uppercase extension defaults to C++ on toolchains that key
# off file extension (this codebase's convention: lowercase .c = C,
# uppercase .C = C++, matching the sibling AmuletsArmor/AAServer/CMake
# builds) -- force it to compile as plain C, same as the top-level
# CMakeLists.txt already does.
#
# __USE_MINGW_ANSI_STDIO=0 / -fno-stack-protector / -march=i486: see
# AmuletsArmor's Build/Win9x/Makefile for the full rationale. The real
# fix, found via a second round of real-hardware testing: every mingw-
# built binary tested crashed with "invalid instruction" at *identical*
# bytes despite sharing no application code -- proof it's mingw's own
# compiler-generated startup code hitting CMOV (0f 44), a Pentium Pro/P6-
# family instruction i686-w64-mingw32-gcc emits by default
# (-march=pentiumpro) but real 486/Pentium/Pentium MMX hardware can't
# execute. This binary wasn't in the crash reports yet, but it's built
# with the identical toolchain default, so it almost certainly has the
# same latent bug -- fixed proactively rather than waiting to see it fail.
$(TARGET): $(REPO_ROOT)/SC.C | $(BUILD_DIR)
	$(CC) -O2 -D_CRT_SECURE_NO_WARNINGS=1 -D__USE_MINGW_ANSI_STDIO=0 -fno-stack-protector -march=i486 -mtune=i486 -x c -o $@ $<

clean:
	rm -rf $(BUILD_DIR)

package: all
	mkdir -p $(BUILD_DIR)/package
	cp $(TARGET) $(BUILD_DIR)/package/
	cp $(REPO_ROOT)/BLANK.SRC $(REPO_ROOT)/BLANK.SRP $(REPO_ROOT)/COMMAND.LST \
	   $(REPO_ROOT)/COMMAND.TXT $(REPO_ROOT)/EVENTS.LST $(REPO_ROOT)/SCRIPT.INC \
	   $(REPO_ROOT)/STANDARD.INC $(REPO_ROOT)/TEMPLATE.SRC $(REPO_ROOT)/TEMPLATE.SRP \
	   $(BUILD_DIR)/package/
