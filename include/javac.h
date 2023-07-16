#ifndef JAVAC_JAVAC_H
#define JAVAC_JAVAC_H

#include "helpers/buffer.h"
#include "helpers/vector.h"
#include <string.h>

#define SEQ(s1, s2)     ((s1) && (s2) && !strcmp((s1), (s2)))

#define CASE_SPACE      \
        case ' ':       \
        case '\t':      \
        case '\n'

#define CASE_NUMBER     \
        case '0':       \
        case '1':       \
        case '2':       \
        case '3':       \
        case '4':       \
        case '5':       \
        case '6':       \
        case '7':       \
        case '8':       \
        case '9'

#define CASE_ADD_SUB    \
        case '+':       \
        case '-'

#define CASE_OPERATOR   \
        case '*':       \
        case '/':       \
        case '%':       \
        case '!':       \
        case '^':       \
        case '|':       \
        case '&':       \
        case '>':       \
        case '<':       \
        case '=':       \
        case '+':       \
        case '-'

#define CASE_STRING     \
        case '"'

enum {
    JAVAC_OK,
    JAVAC_ERROR,
    JAVAC_LEXER_ERROR,
    JAVAC_LEXER_OK
};

int compile_file(const char *ifile, const char *ofile, int flags);

#endif