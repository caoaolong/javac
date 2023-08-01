#include "lexer.h"
#include "token.h"
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
    // 处理注释内容
    if (c == '/') {
        tk = handle_comment(lex_process);
        return tk;
    }
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
        CASE_STRING:
            tk = token_make_string_number(lex_process, TOKEN_TYPE_STRING);
            break;
        CASE_QUOTE:
            tk = token_make_character(lex_process);
            break;
        default:
            if (!isalpha(c) && !isdigit(c)) 
                tk = token_make_symbol(lex_process);
            else if (isalpha(c) || c == '_' || c == '$') 
                tk = token_make_identifier_keyword(lex_process);
            else
                compile_error(lex_process->compiler, "语法错误\n");
            break;
    }
    return tk;
}

int lexer(lexer_process *process)
{
    lex_process = process;
    lex_process->pos.filename = lex_process->compiler->ifile.path;
    token *tk;
    // 括号栈
    struct vector *brackets = vector_create(sizeof(token));
    do {
        tk = tokens();
        if (tk && tk->type) {
            brackets_verify(process, brackets, tk);
            vector_push(lex_process->tokens, tk);
        }
    } while(tk);

    if (!vector_empty(brackets))
        compile_error(process->compiler, "括号没有正确结束\n");

    return JAVAC_LEXER_OK;
}

void brackets_verify(lexer_process *process, struct vector *brackets, token *tk)
{
    if (tk->type == TOKEN_TYPE_SYMBOL) {
        if (tk->cval == '(' || tk->cval == '[' || tk->cval == '{')
            vector_push(brackets, tk);
        else if (tk->cval == ')' || tk->cval == ']' || tk->cval == '}') {
            token *elem = (token*)vector_back(brackets);
            if (elem->cval == '(')
                vector_pop(brackets);
            else if (elem->cval == '[')
                vector_pop(brackets);
            else if (elem->cval == '{')
                vector_pop(brackets);
            else
                compile_error(process->compiler, "括号不匹配: %c\n", tk->cval);
        }
    }
}
