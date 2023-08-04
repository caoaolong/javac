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
    // STATEMENT
    NODE_TYPE_STATEMENT_VARIABLE,
    NODE_TYPE_STATEMENT_STRUCT,
    NODE_TYPE_STATEMENT_BODY,
    NODE_TYPE_STATEMENT_FUNCTION
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
        // 算数运算表达式结构
        struct {
            struct node_t *left;
            struct node_t *right;
            const char *op;
            const char symbol;
            operator_precedence *opp;
            // 三目运算
            struct {
                struct node_t *consequent;
                struct node_t *alternate;
            } test;
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
node *node_create_expression_tree(struct node_t *n1, struct node_t *n2, struct node_t *np);

#endif