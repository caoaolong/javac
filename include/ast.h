#ifndef JAVAC_AST_H
#define JAVAC_AST_H

#include "javac.h"

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
    const char *value;
};

void node_set_vector(struct vector *vec, struct vector *root_vec);

node *node_create(struct node_t *node);
void node_push(struct node_t *node);
node *node_pop();
node *node_peek();

#endif