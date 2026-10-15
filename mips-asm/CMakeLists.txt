project(MipsAsm)

cmake_minimum_required(VERSION 3.10)

set(MIPS_SOURCES src/main.c)
add_executable(mips-asm ${MIPS_SOURCES})
