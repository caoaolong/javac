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
    NODE_TYPE_STATEMENT                  = 0b00000001,
    NODE_TYPE_STATEMENT_DECLARE          = 0b00000010,
    NODE_TYPE_STATEMENT_DECLARE_LIST     = 0b00000100,
    NODE_TYPE_STATEMENT_STRUCT           = 0b00001000,
    NODE_TYPE_STATEMENT_BODY             = 0b00010000,
    NODE_TYPE_STATEMENT_FUNCTION         = 0b00100000,
    NODE_TYPE_STATEMENT_NEW              = 0b01000000
};

typedef struct node_t node;
typedef struct datatype_t datatype;

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
        // declare
        struct {
            struct datatype_t *type;
            const char *name;
            struct node_t *value;
        } var;
        // declare list
        struct {
            struct vector *list;
        } varlist;
        // new
        struct {
            union {
                // new array
                struct {
                    size_t dim;
                    struct node_t *type;
                    struct vector *values;
                } array;
            };
        } new;
    };
    struct {
        int ttype;
        const char *val;
    } value;
};

void node_set_vector(struct vector *nodes_vec);

node *node_create(struct node_t *node);
void node_push(struct node_t *node);
void node_push_delimiter();
node *node_pop();
node *node_peek();

void node_create_expression(lexer_process *process, token *tk);
void node_create_declare(lexer_process *process, token *tk);
void node_create_new(lexer_process *process, token *tk);
node *node_create_expression_tree(struct node_t *n1, struct node_t *n2, struct node_t *np);

#endif