#ifndef _TOKEN_H

#define _TOKEN_H
typedef enum {
    TOKEN_EOF = 0,
    TOKEN_ERROR,
    
    // Identifiers & Labels
    TOKEN_IDENTIFIER,  // Generic symbol or user label reference
    TOKEN_LABEL_DEF,   // Identifier followed by a colon (e.g., "main:")
    
    // Registers (MIPS explicitly prefixes these with '$')
    TOKEN_REGISTER,    // e.g., $t0, $v0, $at, $ra, $0
    
    // Literals
    TOKEN_IMMEDIATE,   // Hex (0x10), Decimal (42), Negative integers (-4)
    TOKEN_STRING,      // Used in .asciiz directives (e.g., "Hello\n")
    
    // Directives
    TOKEN_DIRECTIVE,   // e.g., .text, .data, .word, .asciiz
    
    // Delimiters & Operators
    TOKEN_COMMA,       // ,
    TOKEN_LPAREN,      // (  used in memory addressing like 4($sp)
    TOKEN_RPAREN,       // )
    TOKEN_NEW_LINE     // Identifies a new line 
} TokenType;

typedef struct {
    int start_index;
    int end_index;
    int line_index;
} TokenState;

typedef struct {
    TokenType type;
    TokenState* state;
    char* value;
} Token;

Token* init_token(char* value, int type, TokenState* state);

TokenState* init_token_state(int start, int end, int line);

static const char* token_type_to_str(int type);

char* token_to_str(Token* token);

#endif