#include "ast.h"

struct vector *node_vector = NULL;
struct vector *node_vector_root = NULL;

void node_set_vector(struct vector *vec, struct vector *root_vec)
{
    node_vector = vec;
    node_vector_root = root_vec;
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
    vector_push(node_vector, &node);
}

node *node_pop()
{
    if (vector_empty(node_vector))
        return NULL;
    struct node_t *node = vector_back(node_vector);
    vector_pop(node_vector);
    return node;
}

node *node_peek()
{
    return *(node **)(vector_back_ptr(node_vector));
}