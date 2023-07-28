#include "ast.h"

struct vector *expression_vector = NULL;
struct vector *statement_vector = NULL;

static struct node_t delimiter = {
    .type = NODE_TYPE_DELIMITER
};

void node_set_vector(struct vector *expression_vec, struct vector *statement_vec)
{
    expression_vector = expression_vec;
    statement_vector = statement_vec;
}

node *node_create(struct node_t *node)
{
    struct node_t *nn = malloc(sizeof(struct node_t));
    memcpy(nn, node, sizeof(struct node_t));
    node_push(nn);
    return nn;
}

void node_push(struct node_t *node)
{
    if (!node)
        return;

    if (node->type == NODE_TYPE_EXPRESSION)
        vector_push(expression_vector, node);
    else if (node->type == NODE_TYPE_STATEMENT)
        vector_push(statement_vector, node);
}

void node_push_delimiter(int type)
{
    if (type == NODE_TYPE_EXPRESSION)
        vector_push(expression_vector, &delimiter);
    else if (type == NODE_TYPE_STATEMENT)
        vector_push(expression_vector, &delimiter);
}

node *node_pop(int type)
{
    struct vector *node_vector = NULL;
    if (type == NODE_TYPE_EXPRESSION)
        node_vector = expression_vector;
    else if (type == NODE_TYPE_STATEMENT)
        node_vector = statement_vector;

    if (vector_empty(node_vector))
        return NULL;
    struct node_t *node = vector_back(node_vector);
    vector_pop(node_vector);
    return node;
}

node *node_peek(int type)
{
    if (type == NODE_TYPE_EXPRESSION)
        return (node *)vector_back(expression_vector);
    else if (type == NODE_TYPE_STATEMENT)
        return (node *)vector_back(statement_vector);

    return NULL;
}

void node_create_expression(lexer_process *process, token *tk)
{
    node *n = NULL;
    switch (tk->type) {
        case TOKEN_TYPE_NUMBER:
            n = node_create(&(struct node_t){
                .value = { .val = tk->sval, .ttype = tk->type }, .type = NODE_TYPE_EXPRESSION});
            break;
        case TOKEN_TYPE_STRING:
        case TOKEN_TYPE_CHAR:
            n = node_create(&(node){
                .value = { .val = tk->sval, .ttype = tk->type }, .type = NODE_TYPE_EXPRESSION});
            break;
        case TOKEN_TYPE_IDENTIFIER:
            n = node_create(&(node){
                .value = { .val = tk->sval, .ttype = tk->type }, .type = NODE_TYPE_EXPRESSION});
            break;
        case TOKEN_TYPE_OPERATOR:
            n = node_create(&(node){
                .exp.op = tk->sval, .value.ttype = tk->type, .type = NODE_TYPE_EXPRESSION});
            break;
    }
}