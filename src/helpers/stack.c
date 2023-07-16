#include "helpers/stack.h"
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

stack *stack_create()
{
    stack *s = calloc(1, sizeof(stack));
    s->top = s->capacity = 0;
    return s;
}

void stack_free(stack *stack)
{

}

void stack_push(stack *s, void *entry)
{
    // 检测是否需要扩容
    if (s->top == s->capacity) {
        int oc = s->capacity;
        s->capacity = s->capacity << 2;
        if (s->top > 0) {
            void *ndata = malloc(s->capacity * sizeof(void*));
            memcpy(ndata, s->data, oc);
            s->data = ndata;
        }
    }
}

void *stack_pop(stack *stack)
{

}

void stack_count(stack *stack)
{

}