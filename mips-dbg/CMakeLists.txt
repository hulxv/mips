project(MipsDbg)

cmake_minimum_required(VERSION 3.10)

set(MIPS_DBG_SOURCES src/main.c)
add_executable(mips-dbg ${MIPS_DBG_SOURCES})
