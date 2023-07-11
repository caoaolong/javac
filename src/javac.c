#include "process.h"
#include "lexer.h"
#include <stdio.h>

int compile_file(const char *ifile, const char *ofile, int flags)
{
    compile_process *compile_process = compile_process_create(ifile, ofile, flags);
    if (!compile_process)
        return JAVAC_ERROR;
    // lexical analysis
    lexer_process *lexer_process = lexer_process_create(compile_process);
    if (!lexer_process)
        return JAVAC_ERROR;
    if (lexer(lexer_process) != JAVAC_LEXER_OK)
        return JAVAC_LEXER_ERROR;
    
    return JAVAC_OK;
}