#include "token.h"
#include "stdio.h"
#include "stdlib.h"

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