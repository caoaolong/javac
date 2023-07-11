#ifndef JAVAC_JAVAC_H
#define JAVAC_JAVAC_H

#include "helpers/buffer.h"
#include "helpers/vector.h"

enum {
    JAVAC_OK,
    JAVAC_ERROR,
    JAVAC_LEXER_ERROR,
    JAVAC_LEXER_OK
};

int compile_file(const char *ifile, const char *ofile, int flags);

#endif