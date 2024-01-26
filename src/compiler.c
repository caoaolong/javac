#include "process.h"
#include "lexer.h"
#include "parser.h"
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
    // generate abstract syntax tree
    if (parse(lexer_process) != JAVAC_PARSE_OK)
        return JAVAC_PARSE_ERROR;
    // generate classfile
    if (format(lexer_process) != JAVAC_FORMAT_OK)
        return JAVAC_FORMAT_ERROR;
    return JAVAC_OK;
}

bool strendswith(const char *str, const char *suffix)
{
    return strstr(str, suffix) == str + strlen(str) - strlen(suffix);
}

bool strstartswith(const char *str, const char *prefix)
{
    return strstr(str, prefix) == str;
}

int strindextotal(const char *str, const char c)
{
    int count = 0;
    for (int i = 0; i < strlen(str); i++) {
        if (str[i] == c)
            count++;
    }
    return count;
}