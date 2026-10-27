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

    char* str = calloc(strlen(type_str) + strlen(template) + 24, sizeof(char));
    sprintf(str, template, type_str, token->type, token->value, token->state->start_index, token->state->end_index, token->state->line_index);

    return str;
}

Lexer* init_lexer(const char* src) {
    Lexer* lexer = calloc(1, sizeof(Lexer));
    lexer->src = src;
    lexer->src_size = strlen(src);
    lexer->i = 0;
    lexer->c = src[lexer->i];

    return lexer;
}

void lexer_next(Lexer* lexer) {
    if (lexer->i < lexer->src_size && lexer->c != '\0') {
        lexer->i += 1;
        lexer->c = lexer->src[lexer->i];
    }
}

char lexer_peek(Lexer* lexer, int offset) {
    return lexer->src[lexer->i + offset];
}

void lexer_skip_whitespace(Lexer* lexer) {
    while (lexer->c == ' ' || lexer->c == '\t' || lexer->c == '\r') {
        lexer_next(lexer);
    }
}

Token* lexer_parse_id(Lexer* lexer) {
    char* value = calloc(1, sizeof(char));
    int start = lexer->i;

    while (isalpha(lexer->c)) {
        size_t len = strlen(value);
        value = realloc(value, len + 2);

        value[len] = lexer->c;
        value[len + 1] = '\0';

        lexer_next(lexer);
    }

    if (lexer->c == ':') {
        size_t len = strlen(value);
        value = realloc(value, len + 2);

        value[len] = lexer->c;
        value[len + 1] = '\0';
        
        lexer_next(lexer);

        int end = lexer->i-1;
        TokenState* state = init_token_state(start, end, lines_counter);

        return init_token(value, TOKEN_LABEL_DEF, state);
    } 
        
    int end = lexer->i-1;
    TokenState* state = init_token_state(start, end, lines_counter);

    return init_token(value, TOKEN_IDENTIFIER, state);
}

Token* lexer_parse_number(Lexer* lexer) {
    char* value = calloc(1, sizeof(char));
    int start = lexer->i;
    int (*func)(int) = isdigit;

    if (lexer->c == '0') {
        value[0] = lexer->c;
        lexer_next(lexer);

        if (lexer->c == 'x') {
            size_t len = strlen(value);
            value = realloc(value, len + 2);
        
            value[len] = lexer->c;
            value[len + 1] = '\0';

            func = isxdigit;

            lexer_next(lexer);
        } else if (lexer->c == 'b') {
            size_t len = strlen(value);
            value = realloc(value, len + 2);
        
            value[len] = lexer->c;
            value[len + 1] = '\0';

            func = isbinary;

            lexer_next(lexer);
        }
    } else if (lexer->c == '-') {
        value[0] = lexer->c;
        lexer_next(lexer);

        func = isdigit;
    }


    while (func(lexer->c)) {
        size_t len = strlen(value);
        value = realloc(value, len + 2);
        
        value[len] = lexer->c;
        value[len + 1] = '\0';

        lexer_next(lexer);
    }

    int end = lexer->i-1;
    TokenState* state = init_token_state(start, end, lines_counter);

    return init_token(value, TOKEN_IMMEDIATE, state);
}

Token* lexer_char_token(Lexer* lexer, int type) {
    char* value = calloc(2, sizeof(char));
    int start = lexer->i;
    int end = lexer->i;

    value[0] = lexer->c;
    value[1] = '\0';

    TokenState* state = init_token_state(start, end, lines_counter);

    Token* token = init_token(value, type, state);
    lexer_next(lexer);

    return token;
}

Token* lexer_directive_token(Lexer* lexer, int type) {
    char* value = calloc(1, sizeof(char));
    int start = lexer->i;

    value[0] = lexer->c;
    lexer_next(lexer);

    while (isalpha(lexer->c)) {
        size_t len = strlen(value);
        value = realloc(value, len + 2);

        value[len] = lexer->c;
        value[len + 1] = '\0';

        lexer_next(lexer);
    }

    int end = lexer->i-1;
    TokenState* state = init_token_state(start, end, lines_counter);
    Token* token = init_token(value, type, state);

    return token;
}

Token* lexer_register_token(Lexer* lexer, int type) {
    char* value = calloc(1, sizeof(char));
    int start = lexer->i;

    value[0] = lexer->c;
    lexer_next(lexer);

    while (isalnum(lexer->c)) {
        size_t len = strlen(value);
        value = realloc(value, len + 2);

        value[len] = lexer->c;
        value[len + 1] = '\0';

        lexer_next(lexer);
    }

    int end = lexer->i-1;
    TokenState* state = init_token_state(start, end, lines_counter);
    Token* token = init_token(value, type, state);

    return token;

}

Token* lexer_string_token(Lexer* lexer, int type) {
    char* value = calloc(1, sizeof(char));
    int start = lexer->i;

    lexer_next(lexer);

    while (lexer->c != '"') {
        size_t len = strlen(value);
        value = realloc(value, len + 2);

        value[len] = lexer->c;
        value[len + 1] = '\0';

        lexer_next(lexer);
    }

    int end = lexer->i-1;
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
    tok->state->start_index = lexer->i;
    tok->state->end_index = lexer->i;

    return tok;
}

Token* lexer_next_token(Lexer* lexer) {
    while (lexer->c != '\0') {
        lexer_skip_whitespace(lexer);

        if (isalpha(lexer->c)) {
            return lexer_parse_id(lexer);
        }

        if (isdigit(lexer->c)) {
            return lexer_parse_number(lexer);
        }

        switch (lexer->c) {
            case '(': return lexer_char_token(lexer, TOKEN_LPAREN);
            case ')': return lexer_char_token(lexer, TOKEN_RPAREN);
            case ',': return lexer_char_token(lexer, TOKEN_COMMA);
            case '$': return lexer_register_token(lexer, TOKEN_REGISTER);
            case '.': return lexer_directive_token(lexer, TOKEN_DIRECTIVE);
            case '"': return lexer_string_token(lexer, TOKEN_STRING);
            case '-': return lexer_parse_number(lexer);
            case '\n': return lexer_new_line_token(lexer);
            case '\0': break;
            default: printf("[Lexer]: Unexpected character: `%c`\n", lexer->c); lexer_next(lexer); break;
        }
    }

    return init_token(0, TOKEN_EOF, NULL);
}