#include <stdio.h>
#include <stdlib.h>
#include "lexer.h"

char* read_file(const char* filename);
void compile_file(const char* filename);

int main(int argc, char* argv[]) {
	if (argc < 2) {
		printf("Please specify the input file.\n");
		return 1;
	}

	compile_file(argv[1]);
	// char* file = read_file(argv[1]);
	// printf("%s", file);

	return 0;
	// printf("Hello! I aM VERY SMART MIPS assembler");
}

char* read_file(const char* filename) {
	FILE* fp;

	fp = fopen(filename, "rb");

	if (fp == NULL) {
		printf("Could not read file `%s`\n", filename);
		exit(1);
	}

	fseek(fp, 0L, SEEK_END);
	long file_size = ftell(fp);
	rewind(fp);

	char* buffer = (char*) malloc(file_size + 1);
	
	size_t bytes_read = fread(buffer, sizeof(char), file_size, fp);
	
	buffer[bytes_read] = '\0';

	fclose(fp);

	return buffer;
}

void compile_file(const char* filename) {
	char* src = read_file(filename);

	Lexer* lexer = init_lexer(src);
	Token* tok = 0;

	while ((tok = lexer_next_token(lexer))->type != TOKEN_EOF) {
		printf("%s\n", token_to_str(tok));
	}

	free(src);
}
