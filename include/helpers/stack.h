#ifndef JAVAC_STACK_H
#define JAVAC_STACK_H

#include "helpers/buffer.h"

typedef struct {
    int top;
    int capacity;
    void *data;
} stack;

stack *stack_create();

void stack_free(stack *stack);

void stack_push(stack *stack, void *entry);

void *stack_pop(stack *stack);

void stack_count(stack *stack);

#endif