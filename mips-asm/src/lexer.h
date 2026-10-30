#ifndef _LEXER_H

#define _LEXER_H

#include "token.h"
#include <stdlib.h>
typedef struct {
    const char* src;
    size_t src_size;
    char current_char;
    unsigned int current_index; 
} Lexer;

Lexer* init_lexer(const char* src);

void lexer_next(Lexer* lexer);

char lexer_peek(Lexer* lexer, int offset);

Token* lexer_next_token(Lexer* lexer);

Token* lexer_char_token(Lexer* lexer, int type);

Token* lexer_directive_token(Lexer* lexer, int type);

Token* lexer_register_token(Lexer* lexer, int type);

Token* lexer_string_token(Lexer* lexer, int type);

void lexer_skip_whitespace(Lexer* lexer);

Token* lexer_parse_id(Lexer* lexer);

Token* lexer_parse_number(Lexer* lexer);

Token* lexer_new_line_token(Lexer* lexer);

char* lexer_get_substring(Lexer* lexer, int start, int end);

#endif // _LEXER_H
