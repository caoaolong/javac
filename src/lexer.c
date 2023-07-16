#include "lexer.h"
#include "token.h"
#include "fsm.h"
#include <ctype.h>

lexer_process       *lex_process;
char                 prefix;

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
    process->tokens = vector_create(sizeof(token));
    process->next = lexer_next_char;
    process->peek = lexer_peek_char;
    process->push = lexer_push_char;
    return process;
}

void lexer_process_free(lexer_process *process)
{
    free(process);
}

token *tokens()
{
    token *tk = NULL;
    char c = lex_process->peek(lex_process);
    switch (c) {
        case EOF:
            return NULL;
        CASE_SPACE:
            c = lex_process->next(lex_process);
            return tokens();
        CASE_NUMBER:
            tk = token_make_string_number(lex_process, TOKEN_TYPE_NUMBER);
            break;
        CASE_OPERATOR:
            tk = token_make_operator(lex_process);
            break;
        CASE_ADD_SUB:
            if (prefix == 0) {
                prefix = lex_process->next(lex_process);
                return tokens();
            } else {
                tk = token_make_operator(lex_process);
                break;
            }
        case '"':
            tk = token_make_string_number(lex_process, TOKEN_TYPE_STRING);
            break;
        default:
            if (!isalpha(c) && !isdigit(c)) 
                tk = token_make_symbol(lex_process);
            else if (isalpha(c) || c == '_' || c == '$') 
                tk = token_make_identifier_keyword(lex_process);
            break;
    }
    return tk;
}

int lexer(lexer_process *process)
{
    prefix = 0;
    lex_process = process;
    process->pos.filename = process->compiler->ifile.path;
    token *tk;
    do {
        tk = tokens();
        if (tk) vector_push(lex_process->tokens, tk);
    } while(tk);

    size_t size = vector_count(process->tokens);
    token *elem = (token*)vector_data_ptr(lex_process->tokens);
    for (size_t i = 0; i < size; i++)
    {
        if (elem->type == TOKEN_TYPE_IDENTIFIER 
            || elem->type == TOKEN_TYPE_KEYWORD
            || elem->type == TOKEN_TYPE_STRING
            || elem->type == TOKEN_TYPE_NUMBER
            || elem->type == TOKEN_TYPE_OPERATOR) {
            printf("token<value=%s,type=%#x>\n", (char *)elem->sval, elem->type);
        } else if (elem->type == TOKEN_TYPE_SYMBOL) {
            printf("token<value=%c,type=%d>\n", elem->cval, elem->type);
        }
        elem += 1;
    }
    
    return JAVAC_LEXER_OK;
}
