#include "process.h"
#include "ast.h"
#include <stdlib.h>
#include <stdarg.h>

compile_process* compile_process_create(const char *ifile, const char *ofile, int flags)
{
    FILE *ifp = fopen(ifile, "r");
    if (!ifp)
        return NULL;
    FILE *ofp = fopen(ofile, "w");

    compile_process *process = calloc(1, sizeof(compile_process));
    process->ifile.path = process->pos.filename = ifile;
    process->ifile.ifp = ifp;
    process->ofp = ofp;
    process->pos.col = process->pos.line = 0;
    process->scopes = vector_create(sizeof(scope));
    process->root_scope = scope_new(process, "file");
    return process;
}

void compile_process_free(compile_process *process)
{
    if (!process)
        return;

    if (process->ifile.ifp)
        fclose(process->ifile.ifp);
    
    if (process->ofp)
        fclose(process->ofp);
    
    free(process);
}

void compile_warning(compile_process *process, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
    fprintf(stderr, "warning: on line %i, col %i in file %s\n",
        process->pos.line, process->pos.col, process->pos.filename);
}

void compile_error(compile_process *process, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
    fprintf(stderr, "error: on line %i, col %i in file %s\n",
        process->pos.line, process->pos.col, process->pos.filename);
    exit(-1);
}

extern scope *current_scope;
extern struct vector *scope_stack;

scope *scope_new(compile_process *process, const char *name) {

    scope *s = scope_alloc();
    s->parent = current_scope;
    current_scope = s;
    process->nodes = current_scope->statements;
    vector_push(process->scopes, s);
    if (!scope_stack) {
        scope_stack = vector_create(sizeof(scope*));
    }
    vector_push(scope_stack, &s);
    return s;
}

scope *scope_alloc() {

    scope *s = malloc(sizeof(scope));
    s->statements = vector_create(sizeof(node));
    s->symbols = vector_create(sizeof(node));
    return s;
}

void scope_free(scope *scp) {

    if (scp) {
        if (scp->statements)
            vector_free(scp->statements);
        if (scp->symbols)
            vector_free(scp->symbols);
    }
    free(scp);
}