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
	src/token.c
	src/token.h
	src/helper.c
	src/helper.h
)
add_executable(mips-asm ${MIPS_SOURCES})
