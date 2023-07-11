#include "lexer.h"

lexer_process       *lex_process;

char lexer_next_char(lexer_process *process)
{
    compile_process *compiler = process->compiler;

    compiler->pos.col += 1;
    char c = getc(compiler->ifile.ifp);
    if (c == '\n')
    {
        compiler->pos.col = 1;
        compiler->pos.line += 1;
    }

    return c;
}

char lexer_peek_char(lexer_process *process)
{
    compile_process *compiler = process->compiler;
    char c = getc(compiler->ifile.ifp);
    ungetc(c, compiler->ifile.ifp);
    return c;
}

void lexer_push_char(lexer_process *process, char c)
{
    compile_process *compiler = process->compiler;
    ungetc(c, compiler->ifile.ifp);
}

lexer_process *lexer_process_create(compile_process *compiler)
{
    lexer_process *process = calloc(1, sizeof(lexer_process));
    if (!process)
        return NULL;
    
    process->compiler = compiler;
    process->next = lexer_next_char;
    process->peek = lexer_peek_char;
    process->push = lexer_push_char;
    return process;
}

void lexer_process_free(lexer_process *process)
{
    free(process);
}

int lexer(lexer_process *process)
{
    lex_process = process;
    process->pos.filename = process->compiler->ifile.path;
    char c;
    printf("---Code Start---\n");
    do {
        c = process->next(lex_process);
        if (c != EOF) 
            printf("%c", c);
    } while(c != EOF);
    printf("\n---Code End---\n");
    return JAVAC_LEXER_OK;
}
