#ifndef JAVAC_AST_H
#define JAVAC_AST_H

#include "javac.h"
#include "lexer.h"

enum 
{
    OPERATOR_PRECEDENCE_LEFT_TO_RIGHT,
    OPERATOR_PRECEDENCE_RIGHT_TO_LEFT
};

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