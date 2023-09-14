#include "ast.h"

struct vector *nodes_vec = NULL;

static struct node_t delimiter = {
    .type = NODE_TYPE_DELIMITER
};

void node_set_vector(struct vector *vec)
{
    nodes_vec = vec;
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
    vector_push(nodes_vec, node);
}

void node_push_delimiter()
{
    vector_push(nodes_vec, &delimiter);
    vector_set_peek_pointer(nodes_vec, vector_count(nodes_vec));
}

node *node_pop()
{
    struct vector *node_vector = NULL;
    node_vector = nodes_vec;

    if (vector_empty(node_vector))
        return NULL;
    struct node_t *node = vector_back(node_vector);
    vector_pop(node_vector);
    return node;
}

node *node_peek()
{
    return (node *)vector_back(nodes_vec);;
}

void node_create_expression(lexer_process *process, token *tk)
{
    node *n = NULL;
    switch (tk->type) {
        case TOKEN_TYPE_NUMBER:
            n = node_create(&(node){
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
        case TOKEN_TYPE_SYMBOL:
            n = node_create(&(node){
                .exp.symbol = tk->cval, .value.ttype = tk->type, .type = NODE_TYPE_EXPRESSION});
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

#define CONVERT_CVAL_SVAL(tk)   \
    char *sym_str = malloc(2); \
    sym_str[0] = (tk)->cval; \
    sym_str[1] = 0

void node_create_declare(lexer_process *process, token *tk)
{
    node *n = NULL;
    switch (tk->type)
    {
        case TOKEN_TYPE_KEYWORD:
        case TOKEN_TYPE_IDENTIFIER:
            n = node_create(&(node){
                    .value = { .val = tk->sval, .ttype = tk->type }, 
                    .type = NODE_TYPE_STATEMENT });
            break;
        case TOKEN_TYPE_SYMBOL: {
            CONVERT_CVAL_SVAL(tk);
            n = node_create(&(node){
                    .value = { .val = sym_str, .ttype = tk->type }, 
                    .type = NODE_TYPE_STATEMENT });
            break;
        }
    }
}

void node_create_new(lexer_process *process, token *tk)
{
    node *n = NULL;
    switch (tk->type) {
        case TOKEN_TYPE_KEYWORD: {
            n = node_create(&(node){
                    .value = { .val = tk->sval, .ttype = tk->type }, 
                    .type = NODE_TYPE_STATEMENT | NODE_TYPE_STATEMENT_NEW });
            break;
        }
        case TOKEN_TYPE_SYMBOL: {
            CONVERT_CVAL_SVAL(tk);
            n = node_create(&(node){
                    .value = { .val = sym_str, .ttype = tk->type }, 
                    .type = NODE_TYPE_STATEMENT });
            break;
        }
    }
}

void node_create_import(lexer_process *process, token *tk)
{
    node *n = NULL;
    switch (tk->type) {
        case TOKEN_TYPE_KEYWORD:
            n = node_create(&(node){.value = { 
                .val = tk->sval, .ttype = tk->type }, 
                .type = NODE_TYPE_STATEMENT | NODE_TYPE_STATEMENT_IMPORT,
                .import.list = vector_create(sizeof(void*)) });
            break;
        case TOKEN_TYPE_IDENTIFIER:
            n = node_create(&(node){.value = { 
                .val = tk->sval, .ttype = tk->type }, 
                .type = NODE_TYPE_STATEMENT});
            break;
        case TOKEN_TYPE_SYMBOL:
            if (tk->cval == '.') {
                CONVERT_CVAL_SVAL(tk);
                n = node_create(&(node){.value = { 
                    .val = sym_str, .ttype = tk->type }, 
                    .type = NODE_TYPE_STATEMENT});
                ;
            }
            break;
        case TOKEN_TYPE_OPERATOR:
            if (SEQ(tk->sval, "*")) {
                n = node_create(&(node){.value = { 
                    .val = tk->sval, .ttype = tk->type }, 
                    .type = NODE_TYPE_STATEMENT});
                ;
            }
            break;
    }
}