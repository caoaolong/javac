#include "process.h"
#include <stdlib.h>

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