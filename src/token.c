#include "javac.h"
#include "lexer.h"
#include "token.h"
#include "fsm.h"
#include <string.h>
#include <stdbool.h>

static token        tmp_token;

static bool is_keyword(const char *value)
{
    return SEQ(value, "public") || SEQ(value, "private") || SEQ(value, "protected")
        || SEQ(value, "abstract") || SEQ(value, "extends") || SEQ(value, "implements")
        || SEQ(value, "static") || SEQ(value, "final") || SEQ(value, "volitale")
        || SEQ(value, "class") || SEQ(value, "interface") || SEQ(value, "enum")
        || SEQ(value, "byte") || SEQ(value, "char") || SEQ(value, "short") || SEQ(value, "void")
        || SEQ(value, "int") || SEQ(value, "long") || SEQ(value, "double") || SEQ(value, "float")
        || SEQ(value, "boolean") || SEQ(value, "assert") || SEQ(value, "import")
        || SEQ(value, "do") || SEQ(value, "while") || SEQ(value, "for") || SEQ(value, "if") || SEQ(value, "else")
        || SEQ(value, "switch") || SEQ(value, "case") || SEQ(value, "break") || SEQ(value, "continue")
        || SEQ(value, "this") || SEQ(value, "super") || SEQ(value, "throws") || SEQ(value, "synchronized")
        || SEQ(value, "try") || SEQ(value, "catch") || SEQ(value, "finally") || SEQ(value, "throw")
        || SEQ(value, "instanceof") || SEQ(value, "default") || SEQ(value, "package") 
        || SEQ(value, "const") || SEQ(value, "native") || SEQ(value, "goto") || SEQ(value, "strictfp")
        || SEQ(value, "return")|| SEQ(value, "transient");
}

token *token_create(lexer_process *process, token *_token)
{
    memcpy(&tmp_token, _token, sizeof(token));
    tmp_token.pos = process->pos;
    return &tmp_token;
}

token *token_make_identifier_keyword(lexer_process *process)
{
    // 判断是否为标识符
    int i = 0;
    char c = process->next(process);
    struct buffer *buf = buffer_create();
    while(true) {
        i = fsm_identifier_next(i, c);
        if (i == -1) {
            buffer_free(buf);
            return NULL;
        } else if (i == TOKEN_TYPE_IDENTIFIER) {
            process->push(process, c);
            break;
        } else 
            buffer_write(buf, c);
        c = process->next(process);
    }
    buffer_write(buf, 0);
    if (is_keyword((char*)buffer_ptr(buf)))
        return token_create(process, &(token){
        .type = TOKEN_TYPE_KEYWORD, .sval = buffer_ptr(buf)});
    else
        return token_create(process, &(token){
        .type = TOKEN_TYPE_IDENTIFIER, .sval = buffer_ptr(buf)});
}

token *token_make_symbol(lexer_process *process)
{
    int i = 0;
    char c = process->next(process);
    while (true) {
        i = fsm_signal_symbol_next(i, c);
        if (i == -1) {
            printf("compile error!\n");
            exit(-1);
        } else if (i == TOKEN_TYPE_SYMBOL || i == TOKEN_TYPE_NEWLINE || i == TOKEN_TYPE_EXPEND)
            break;
        c = process->next(process);
    }
    return token_create(process, &(token){
        .type = TOKEN_TYPE_SYMBOL, .cval = c});
}

token *token_make_string_number(lexer_process *process, int type)
{
    int i = 0;
    char c = process->next(process);
    struct buffer *buf = buffer_create();
    while (true) {
        switch(type) {
            case TOKEN_TYPE_STRING:
                i = fsm_string_next(i, c);
                break;
            case TOKEN_TYPE_NUMBER:
                i = fsm_number_next(i, c);
                break;
            case TOKEN_TYPE_COMMENT:
                i = fsm_comment_next(i, c);
                break;
            default:
                return NULL;
        }
        if (i == -1) {
            buffer_free(buf);
            return NULL;
        } else if (i == TOKEN_TYPE_STRING || i == TOKEN_TYPE_NUMBER || i == TOKEN_TYPE_COMMENT) {
            process->push(process, c);
            break;
        } else
            buffer_write(buf, c); 
        c = process->next(process);
    }
    buffer_write(buf, 0);
    return token_create(process, &(token){
        .type = type, .sval = buffer_ptr(buf)});
}

token *token_make_operator(lexer_process *process)
{
    int i = 0;
    char c = process->next(process);
    struct buffer *buf = buffer_create();
    while (true) {
        i = fsm_operator_next(i, c);
        if (i == -1) {
            buffer_free(buf);
            return NULL;
        } else if (i == TOKEN_TYPE_OPERATOR) {
            process->push(process, c);
            break;
        } else
            buffer_write(buf, c); 
        c = process->next(process);
    }
    buffer_write(buf, 0);
    return token_create(process, &(token){
        .type = TOKEN_TYPE_OPERATOR, .sval = buffer_ptr(buf)});
}

token *token_make_character(lexer_process *process)
{
    int i = 0;
    unsigned char c = process->next(process);
    struct buffer *buf = buffer_create();
    while (true) {
        i = fsm_quote_next(i, c);
        if (i == -1) {
            buffer_free(buf);
            return NULL;
        } else if (i == TOKEN_TYPE_CHAR) {
            buffer_write(buf, c);
            break;
        } else
            buffer_write(buf, c); 
        c = process->next(process);
    }
    buffer_write(buf, 0);
    return token_create(process, &(token){
        .type = TOKEN_TYPE_CHAR, .sval = buffer_ptr(buf)});
}

token *handle_comment(lexer_process *process)
{
    return token_make_string_number(process, TOKEN_TYPE_COMMENT);
}