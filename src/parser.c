#include "parser.h"

int parse_expression(struct vector *expression)
{
    struct vector *ops = vector_create(sizeof(node*));
    struct vector *values = vector_create(sizeof(node*));
    node *n = vector_peek(expression);
    while (n) {
        vector_push(n->value.ttype == TOKEN_TYPE_OPERATOR ? ops : values, &n);
        n++;
    }
    return JAVAC_PARSE_OK;
}

int parse(lexer_process *process)
{
    node_set_vector(process->compiler->ast.expressions, process->compiler->ast.statements);
    // 解析toke列表
    bool exp = false;
    token *elem = (token*)vector_peek(process->tokens);
    // 区分Expression和Statement
    while(elem && elem->type) {
        if (elem->type == TOKEN_TYPE_OPERATOR && SEQ(elem->sval, "=")) {
            exp = true;
            elem ++;
            continue;
        } else if (elem->type == TOKEN_TYPE_SYMBOL && elem->cval == ';' && exp) {
            node_push_delimiter(NODE_TYPE_EXPRESSION);
            exp = false;
            elem ++;
            continue;
        }
        
        if (exp) 
            node_create_expression(process, elem);
        elem ++;
    };

    // parse expression
    if (parse_expression(process->compiler->ast.expressions) != JAVAC_PARSE_OK)
        return JAVAC_PARSE_ERROR;

    return JAVAC_PARSE_OK;
}