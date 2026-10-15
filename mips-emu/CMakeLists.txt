project(MipsEmu)

cmake_minimum_required(VERSION 3.10)

set(MIPS_EMU_SOURCES src/main.c)
add_executable(mips-emu ${MIPS_EMU_SOURCES})
