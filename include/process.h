#ifndef JAVAC_PROCESS_H
#define JAVAC_PROCESS_H

#include "javac.h"

typedef struct {
    const char *operator;
    int order;
    int precedence;
    int ec;
} operator_precedence;

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
        // declare
        struct {
            struct datatype_t *type;
            const char *name;
            struct node_t *value;
            struct scope_t *body;
            struct vector *args;
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

typedef struct scope_t scope;
struct scope_t {
    scope *parent;
    struct vector *symbols;
    struct vector *statements;
    const char *name;
};

typedef struct {

    struct {
        FILE *ifp;
        const char *path;
    } ifile;
    FILE *ofp;
    int flags;
    position pos;
    struct scope_t *root_scope;
    struct vector *scopes;
    struct vector *nodes;
} compile_process;

compile_process* compile_process_create(const char *ifile, const char *ofile, int flags);
void compile_process_free(compile_process *process);

void compile_warning(compile_process *process, const char *fmt, ...);
void compile_error(compile_process *process, const char *fmt, ...);

scope *scope_new(compile_process *process, const char *name);
scope *scope_alloc();
void scope_free(scope *scp);
#endif