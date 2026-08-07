/* Mac OS 9 / Metrowerks CodeWarrior compile unit for the AA Script Compiler.
 *
 * The canonical source is SC.C (uppercase ".C"). CodeWarrior's default file
 * mappings send ".c" to the C compiler but leave an uppercase ".C" unmapped
 * (so it is never compiled), while gcc/clang treat ".C" as C++. This tiny
 * lowercase ".c" wrapper is what the CodeWarrior project actually compiles;
 * it pulls SC.C in as a single translation unit so the program builds as C
 * without requiring a project-level ".C" file mapping. All of the actual
 * code -- and every classic-Mac fix -- lives in SC.C itself.
 *
 * SC.C is found via the project's access path to the repository root.
 */
#include "SC.C"
