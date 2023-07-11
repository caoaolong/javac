#ifndef JAVAC_LEXER_H
#define JAVAC_LEXER_H

#include "process.h"

typedef struct lexer_process_t lexer_process;

struct lexer_process_t {
    position pos;
    compile_process *compiler;
    char (*next)(lexer_process *process);
    char (*peek)(lexer_process *process);
    void (*push)(lexer_process *process, char c);
};

lexer_process *lexer_process_create(compile_process *compiler);
void lexer_process_free(lexer_process *process);

int lexer(lexer_process *process);

#endif