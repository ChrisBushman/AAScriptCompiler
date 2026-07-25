cmake_minimum_required(VERSION 3.20)
project(AAScriptCompiler LANGUAGES C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_executable(AAScriptCompiler SC.C)

# SC.C's uppercase extension defaults to C++ on toolchains that key off
# file extension (this codebase's convention: lowercase .c = C, uppercase
# .C = C++, matching the sibling AmuletsArmor/AAServer projects) -- force
# it to compile as plain C.
set_source_files_properties(SC.C PROPERTIES LANGUAGE C)
