#ifndef JAVAC_AST_H
#define JAVAC_AST_H

#include "javac.h"
#include "lexer.h"

enum 
{
    OPERATOR_PRECEDENCE_LEFT_TO_RIGHT,
    OPERATOR_PRECEDENCE_RIGHT_TO_LEFT
};

typedef struct {
    const char *operator;
    int order;
    int precedence;
    int ec;
} operator_precedence;

enum
{
    NODE_TYPE_EXPRESSION = 0x10,
    NODE_TYPE_DELIMITER,
    NODE_TYPE_STATEMENT
};

typedef struct node_t node;

struct node_t {
    int type;
    position pos;
    struct {
        struct node_t *owner;
        struct node_t *function;
    } bind;
    union {
        struct exp {
            struct node_t *left;
            struct node_t *right;
            const char *op;
        } exp;
    };
    struct {
        int ttype;
        const char *val;
    } value;
};

void node_set_vector(struct vector *vec, struct vector *root_vec);

node *node_create(struct node_t *node);
void node_push(struct node_t *node);
void node_push_delimiter(int type);
node *node_pop(int type);
node *node_peek(int type);

void node_create_expression(lexer_process *process, token *tk);

#endif