#include "lexer.h"
#include "stdio.h"
#include <string.h>
#include <ctype.h>

int lines_counter = 1;
bool inc_lines_counter = false;

int isbinary(int c) {
    return c == '1' || c == '0';
}

TokenState* init_token_state(int start, int end, int line) {
    TokenState* state = calloc(1 , sizeof(TokenState));
    state->start_index = start;
    state->end_index = end  ;
    state->line_index = line;

    return state;
}

Token* init_token(char* value, int type, TokenState* state) {
    Token* token = calloc(1, sizeof(Token));
    token->value = value;
    token->type = type;

    if (state != NULL) 
        token->state = state;

    return token;
}

static const char* token_type_to_str(int type) {
    switch (type) {
        case TOKEN_IDENTIFIER: return "TOKEN_ID";
        case TOKEN_LABEL_DEF: return "TOKEN_LABLE";
        case TOKEN_REGISTER: return "TOKEN_REGISTER";
        case TOKEN_IMMEDIATE: return "TOKEN_IMMEDIATE";
        case TOKEN_STRING: return "TOKEN_STRING";
        case TOKEN_DIRECTIVE: return "TOKEN_DIRECTIVE";
        case TOKEN_COMMA: return "TOKEN_COMMA";
        case TOKEN_LPAREN: return "TOKEN_LPAREN";
        case TOKEN_RPAREN: return "TOKEN_RPAREN";
        case TOKEN_EOF: return "TOKEN_EOF";
        case TOKEN_NEW_LINE: return "TOKEN_NEW_LINE";
    }

    return "not_stringable";
}

char* token_to_str(Token* token) {
    const char* type_str = token_type_to_str(token->type);
    const char* template = "<type=`%s`, int_type=`%d`, value=`%s`, start=`%d', end=`%d`, line=`%d`>";

    size_t len = 0;
    len = snprintf (NULL, len, template, type_str, token->type, token->value, token->state->start_index, token->state->end_index, token->state->line_index);

    char* str = calloc(1, sizeof(char) * len + 1);
    sprintf(str, template, type_str, token->type, token->value, token->state->start_index, token->state->end_index, token->state->line_index);

    return str;
}

Lexer* init_lexer(const char* src) {
    Lexer* lexer = calloc(1, sizeof(Lexer));
    lexer->src = src;
    lexer->src_size = strlen(src);
    lexer->current_index = 0;
    lexer->current_char = src[lexer->current_index];

    return lexer;
}

void lexer_next(Lexer* lexer) {
    if (lexer->current_index < lexer->src_size && lexer->current_char != '\0') {
        lexer->current_index += 1;
        lexer->current_char = lexer->src[lexer->current_index];
    }
}

char lexer_peek(Lexer* lexer, int offset) {
    return lexer->src[lexer->current_index + offset];
}

char* lexer_get_substring(Lexer* lexer, int start, int end) {
    size_t size = (end - start) + 1;
    char* value = calloc(size + 1, sizeof(char));

    memcpy(value, lexer->src + start, size);

    value[size] = '\0';

    return value;
}

void lexer_skip_whitespace(Lexer* lexer) {
    while (lexer->current_char == ' ' || lexer->current_char == '\t' || lexer->current_char == '\r') {
        lexer_next(lexer);
    }
}

Token* lexer_parse_id(Lexer* lexer) {
    int start = lexer->current_index;
    lexer_next(lexer);

    while (isalpha(lexer->current_char)) {
        lexer_next(lexer);
    }

    if (lexer->current_char == ':') {
        lexer_next(lexer);

        int end = lexer->current_index - 1;

        char* value = lexer_get_substring(lexer, start, end);

        TokenState* state = init_token_state(start, end, lines_counter);

        return init_token(value, TOKEN_LABEL_DEF, state);
    }
        
    int end = lexer->current_index - 1;

    char* value = lexer_get_substring(lexer, start, end);

    TokenState* state = init_token_state(start, end, lines_counter);

    return init_token(value, TOKEN_IDENTIFIER, state);
}

Token* lexer_parse_number(Lexer* lexer) {
    int start = lexer->current_index;
    lexer_next(lexer);

    int (*func)(int) = isdigit;

    if (lexer->current_char == '0') {
        lexer_next(lexer);

        if (lexer->current_char == 'x') {
            func = isxdigit;

            lexer_next(lexer);
        } else if (lexer->current_char == 'b') {
            lexer_next(lexer);
        }
    } else if (lexer->current_char == '-') {
        lexer_next(lexer);

        func = isdigit;
    }


    while (func(lexer->current_char)) {
        lexer_next(lexer);
    }

    int end = lexer->current_index - 1;

    char* value = lexer_get_substring(lexer, start, end);

    TokenState* state = init_token_state(start, end, lines_counter);

    return init_token(value, TOKEN_IMMEDIATE, state);
}

Token* lexer_char_token(Lexer* lexer, int type) {
    char* value = calloc(2, sizeof(char));
    int start = lexer->current_index;
    int end = lexer->current_index;

    value[0] = lexer->current_char;
    value[1] = '\0';

    TokenState* state = init_token_state(start, end, lines_counter);

    Token* token = init_token(value, type, state);
    lexer_next(lexer);

    return token;
}

Token* lexer_directive_token(Lexer* lexer, int type) {
    int start = lexer->current_index;
    lexer_next(lexer);

    while (isalpha(lexer->current_char)) {
        lexer_next(lexer);
    }

    int end = lexer->current_index - 1;

    char* value = lexer_get_substring(lexer, start, end);

    TokenState* state = init_token_state(start, end, lines_counter);
    Token* token = init_token(value, type, state);

    return token;
}

Token* lexer_register_token(Lexer* lexer, int type) {
    int start = lexer->current_index;
    lexer_next(lexer);

    while (isalnum(lexer->current_char)) {
        lexer_next(lexer);
    }

    int end = lexer->current_index - 1;

    char* value = lexer_get_substring(lexer, start, end);

    TokenState* state = init_token_state(start, end, lines_counter);
    Token* token = init_token(value, type, state);

    return token;

}

Token* lexer_string_token(Lexer* lexer, int type) {
    int start = lexer->current_index;
    lexer_next(lexer);

    while (lexer->current_char != '"') {
        lexer_next(lexer);
    }

    int end = lexer->current_index - 1;

    char* value = lexer_get_substring(lexer, start, end);

    TokenState* state = init_token_state(start, end, lines_counter);
    Token* token = init_token(value, type, state);

    lexer_next(lexer);

    return token;
}

Token* lexer_new_line_token(Lexer* lexer) {
    char* value = "\\n";
    lines_counter++;

    Token* tok = lexer_char_token(lexer, TOKEN_NEW_LINE);

    tok->value = value;
    tok->state->start_index = lexer->current_index;
    tok->state->end_index = lexer->current_index;

    return tok;
}

Token* lexer_next_token(Lexer* lexer) {
    while (lexer->current_char != '\0') {
        lexer_skip_whitespace(lexer);

        if (isalpha(lexer->current_char)) {
            return lexer_parse_id(lexer);
        }

        if (isdigit(lexer->current_char)) {
            return lexer_parse_number(lexer);
        }

        switch (lexer->current_char) {
            case '(': return lexer_char_token(lexer, TOKEN_LPAREN);
            case ')': return lexer_char_token(lexer, TOKEN_RPAREN);
            case ',': return lexer_char_token(lexer, TOKEN_COMMA);
            case '$': return lexer_register_token(lexer, TOKEN_REGISTER);
            case '.': return lexer_directive_token(lexer, TOKEN_DIRECTIVE);
            case '"': return lexer_string_token(lexer, TOKEN_STRING);
            case '-': return lexer_parse_number(lexer);
            case '\n': return lexer_new_line_token(lexer);
            case '\0': break;
            default: printf("[Lexer]: Unexpected character: `%c`\n", lexer->current_char); lexer_next(lexer); break;
        }
    }

    return init_token(0, TOKEN_EOF, NULL);
}