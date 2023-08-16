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
    struct node_t *nn = calloc(1, sizeof(struct node_t));
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
    else if (node->type & NODE_TYPE_STATEMENT)
        vector_push(statement_vector, node);
}

void node_push_delimiter(int type)
{
    if (type == NODE_TYPE_EXPRESSION)
        vector_push(expression_vector, &delimiter);
    else if (type == NODE_TYPE_STATEMENT)
        vector_push(statement_vector, &delimiter);
}

node *node_pop(int type)
{
    struct vector *node_vector = NULL;
    if (type == NODE_TYPE_EXPRESSION)
        node_vector = expression_vector;
    else if (type & NODE_TYPE_STATEMENT)
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
    else if (type & NODE_TYPE_STATEMENT)
        return (node *)vector_back(statement_vector);

    return NULL;
}

void node_create_expression(lexer_process *process, token *tk)
{
    node *n = NULL;
    switch (tk->type) {
        case TOKEN_TYPE_NUMBER:
            n = node_create(&(node){
                .value = { .sval.val = tk->sval, .sval.ttype = tk->type }, .type = NODE_TYPE_EXPRESSION});
            break;
        case TOKEN_TYPE_STRING:
        case TOKEN_TYPE_CHAR:
            n = node_create(&(node){
                .value = { .sval.val = tk->sval, .sval.ttype = tk->type }, .type = NODE_TYPE_EXPRESSION});
            break;
        case TOKEN_TYPE_IDENTIFIER:
            n = node_create(&(node){
                .value = { .sval.val = tk->sval, .sval.ttype = tk->type }, .type = NODE_TYPE_EXPRESSION});
            break;
        case TOKEN_TYPE_OPERATOR:
            n = node_create(&(node){
                .exp.op = tk->sval, .value.sval.ttype = tk->type, .type = NODE_TYPE_EXPRESSION});
            break;
        case TOKEN_TYPE_SYMBOL:
            n = node_create(&(node){
                .exp.symbol = tk->cval, .value.sval.ttype = tk->type, .type = NODE_TYPE_EXPRESSION});
            break;
    }
}

node ec3node;

node *node_create_expression_tree(struct node_t *n1, struct node_t *n2, struct node_t *np)
{
    if (np->exp.symbol == '?') {
        ec3node.type = NODE_TYPE_EXPRESSION;
        ec3node.exp.left = n2->exp.left;
        ec3node.exp.right = n2->exp.right;
        struct node_t *nn1 = calloc(1, sizeof(struct node_t));
        memcpy(nn1, n1, sizeof(struct node_t));
        ec3node.exp.test.consequent = nn1;
        return np;
    }

    if (np->exp.symbol == ':') {
        struct node_t *nn1 = calloc(1, sizeof(struct node_t));
        memcpy(nn1, n1, sizeof(struct node_t));
        ec3node.exp.test.alternate = nn1;
        struct node_t *nnp = calloc(1, sizeof(struct node_t));
        memcpy(nnp, &ec3node, sizeof(struct node_t));
        memset(&ec3node, 0, sizeof(node));
        return nnp;
    }

    struct node_t *nn1 = calloc(1, sizeof(struct node_t)), *nn2 = NULL;
    memcpy(nn1, n1, sizeof(struct node_t));
    if (n2) {
        nn2 = calloc(1, sizeof(struct node_t));
        memcpy(nn2, n2, sizeof(struct node_t));
    }
    if (np->exp.opp) {
        if (np->exp.opp->order == OPERATOR_PRECEDENCE_LEFT_TO_RIGHT) {
            if (nn2) 
                np->exp.left = nn2;
            np->exp.right = nn1;
        } else if (np->exp.opp->order == OPERATOR_PRECEDENCE_RIGHT_TO_LEFT) {
            np->exp.left = nn1;
            if (nn2)
                np->exp.right = nn2;
        }
    }
    return np;
}

void node_create_declare(lexer_process *process, token *tk)
{
    node *n = NULL;
    switch (tk->type)
    {
        case TOKEN_TYPE_KEYWORD:
        case TOKEN_TYPE_IDENTIFIER:
            n = node_create(&(node){
                    .value = { .sval.val = tk->sval, .sval.ttype = tk->type }, 
                    .type = NODE_TYPE_STATEMENT });
            break;
    }
}