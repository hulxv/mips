project(MipsAsm)

cmake_minimum_required(VERSION 3.10)

set(MIPS_SOURCES 
	src/main.c
	src/lexer.c
	src/lexer.h
	src/codegen.c
	src/codegen.h
	src/parser.c
	src/parser.h
)
add_executable(mips-asm ${MIPS_SOURCES})
