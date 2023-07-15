#ifndef JAVAC_TOKEN_H
#define JAVAC_TOKEN_H

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

#endif