# Mac OS 9 / PowerPC platform (Metrowerks CodeWarrior 8)

Builds the **AA Script Compiler** (`SC.C`) as a classic **Mac OS 9** (not OS X)
PowerPC/CFM console tool that runs on a real Power Mac / PowerBook under Mac
OS 9.x. It compiles Amulets & Armor script sources (`*.SRC`) into the engine's
bytecode (`*.SRP`), exactly like the other platform builds.

Unlike every other platform here, this one is **not** a gcc/Makefile build —
classic Mac OS has no Unix toolchain, so it uses **Metrowerks CodeWarrior 8**
(the `AAScriptCompiler.mcp` project in this folder), the MSL runtime, and the
SIOUX console.

## Files in this folder

| File | Purpose |
|------|---------|
| `AAScriptCompiler.mcp` | CodeWarrior 8 project, based on the "Mac OS Classic / Std C Console" stationery (MSL C + SIOUX + InterfaceLib/MathLib). Its access paths are machine-local — retarget the repo root (where `SC.C` lives) and this folder when you open it. Two targets: `Std C Console Debug` and `Std C Console Final`. |
| `sc_os9.c` | The single compile unit the project builds — a one-line wrapper: `#include "SC.C"`. See below. |

## Why the `sc_os9.c` wrapper

The canonical source is `SC.C` (uppercase `.C`). CodeWarrior's default file
mappings send lowercase `.c` to the C compiler but leave an uppercase `.C`
**unmapped**, so it is never compiled (you get a `Sources: 0 code` /
`undefined 'main'` link error); meanwhile gcc/clang treat `.C` as **C++**,
where `SC.C`'s `unsigned char*`/`char*` string mixing is an error.

Rather than add a project-wide `.C`→C file mapping, `sc_os9.c` (a real
lowercase `.c` file, so CW maps it to the C compiler) simply pulls `SC.C` in as
one translation unit. `SC.C` is found via the project's access path to the repo
root. Add **only** `sc_os9.c` to the project; `SC.C` stays on disk and is
`#include`d, not added.

## Classic-Mac source fixes (all in `SC.C`, guarded — no other platform affected)

The output format needed **no** changes: `SC.C` already emits little-endian
bytecode explicitly (`OutputNumber` / `WriteLE16/32`, header written
field-by-field, not a struct dump), so the OS 9 PPC build produces byte-for-byte
the same `.SRP` as the gcc/IRIX/Win9x builds (verified against the reference).
The classic-Mac-specific fixes are:

- **`//` comments → `/* */`.** CW's C compiler in strict C89 mode rejects
  C++-style `//` comments (7 commented-out debug/`#include` lines).
- **`#pragma mpwc_relax on`** (guarded `#if defined(__MWERKS__)`). CW is strict
  about mixing `char*` and `unsigned char*`; `SC` uses `T_byte8` (unsigned char)
  string buffers with the standard `char*` `<string.h>`/`scanf` APIs. This
  restores the lenient MPW-C pointer behavior gcc/IRIX already have.
  `require_prototypes off` silences the forward-declaration warnings.
- **Text-mode reads → `"rb"`** (`EVENTS.LST`, `COMMAND.LST`, the input `.SRC`).
  Classic Mac text mode translates line endings; `"rb"` avoids it (the parser
  already tolerates trailing `\r`/`\n`). Output was already `"wb"`.
- **`ccommand(&argv)`** (guarded `#if defined(macintosh)`, `<console.h>`).
  Classic Mac has no shell to hand `argv` to a SIOUX app, so this pops the
  standard console dialog to type `<script file> <output file>`.

## Running it on OS 9

The compiler is a SIOUX console app. On launch it shows the CodeWarrior
"Argument" dialog — type e.g. `L1.SRC L1.SRP` and click OK. `EVENTS.LST` and
`COMMAND.LST` (and the input `.SRC`) must be in the **same folder as the app**;
the `.SRP` is written there too.
