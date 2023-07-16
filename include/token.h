#ifndef JAVAC_TOKEN_H
#define JAVAC_TOKEN_H

#include "process.h"

#define TOKEN_TYPE_SYMBOL       0x101
#define TOKEN_TYPE_NEWLINE      0x102
#define TOKEN_TYPE_EXPEND       0x103
#define TOKEN_TYPE_IDENTIFIER   0x104
#define TOKEN_TYPE_KEYWORD      0x105
#define TOKEN_TYPE_STRING       0x106
#define TOKEN_TYPE_NUMBER       0x107
#define TOKEN_TYPE_OPERATOR     0x108
#define TOKEN_TYPE_CHAR         0x109
#define TOKEN_TYPE_COMMENT      0x10A

typedef struct lexer_process_t lexer_process;

typedef struct {
    int type;
    int flags;
    position pos;
    union {
        char cval;
        const char *sval;
        unsigned int inum;
        unsigned long lnum;
        unsigned long long llnum;
        void *any;
    };
} token;

token *token_make_identifier_keyword(lexer_process *process);
token *token_make_symbol(lexer_process *process);
token *token_make_string_number(lexer_process *process, int type);
token *token_make_operator(lexer_process *process);
token *token_make_character(lexer_process *process);

token *handle_comment(lexer_process *process);

#endif