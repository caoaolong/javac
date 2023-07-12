#include "lexer.h"
#include "fsm.h"
#include <ctype.h>

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
        switch (c) {
            case EOF:
                exit(0);
            case ' ':
            case '\n':
                break;
            default: {
                int i;
                if (!isalpha(c) && !isdigit(c)) {
                    // 判断是否为符号
                    i = 0;
                    while (true) {
                        i = fsm_signal_symbol_next(i, c);
                        if (i == -1) {
                            printf("compile error!\n");
                            exit(-1);
                        } else if (i == TOKEN_TYPE_SYMBOL || i == TOKEN_TYPE_NEWLINE || i == TOKEN_TYPE_EXPEND)
                            break;
                        c = process->next(lex_process);
                    }
                    printf("%c<%d>\n", c, i);
                } else if (isalpha(c) || c == '_' || c == '$') {
                    // 判断是否为标识符
                    i = 0;
                    struct buffer *buf = buffer_create();
                    while(true) {
                        i = fsm_identifier_next(i, c);
                        if (i == -1)
                            break;
                        else if (i == TOKEN_TYPE_IDENTIFIER) {
                            process->push(lex_process, c);
                            break;
                        } else 
                            buffer_write(buf, c);
                        c = process->next(lex_process);
                    }
                    buffer_write(buf, 0);
                    printf("%s<%d>\n", (char*)buffer_ptr(buf), i);
                    buffer_free(buf);
                }
            }
                break;
        }
    } while(true);
    printf("\n---Code End---\n");
    return JAVAC_LEXER_OK;
}
