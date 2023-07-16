#ifndef JAVAC_PROCESS_H
#define JAVAC_PROCESS_H

#include "javac.h"

typedef struct {
    int line;
    int col;
    const char *filename;
} position;

typedef struct {

    struct {
        FILE *ifp;
        const char *path;
    } ifile;
    FILE *ofp;
    int flags;

    position pos;

} compile_process;

compile_process* compile_process_create(const char *ifile, const char *ofile, int flags);
void compile_process_free(compile_process *process);

void compile_warning(compile_process *process, const char *fmt, ...);
void compile_error(compile_process *process, const char *fmt, ...);
#endif