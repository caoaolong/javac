#ifndef JAVAC_JAVAC_H
#define JAVAC_JAVAC_H

#include "helpers/buffer.h"
#include "helpers/vector.h"
#include <string.h>

#define CLASSPATH       "/usr/lib/jvm/java-8-openjdk-arm64/jre/lib/rt.jar"
#define VERSION         0x34000000
#define MAGIC           0xcafebabe
#define SEQ(s1, s2)     ((s1) && (s2) && !strcmp((s1), (s2)))
#define JEOF            0xFF
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

#define CASE_QUOTE      \
        case '\''

enum {
    JAVAC_OK,
    JAVAC_ERROR,
    JAVAC_LEXER_OK,
    JAVAC_LEXER_ERROR,
    JAVAC_PARSE_OK,
    JAVAC_PARSE_ERROR,
    JAVAC_FORMAT_OK,
    JAVAC_FORMAT_ERROR
};

typedef struct {
    int line;
    int col;
    const char *filename;
} position;

int compile_file(const char *ifile, const char *ofile, int flags);

bool strendswith(const char *str, const char *suffix);
bool strstartswith(const char *str, const char *prefix);
int strindextotal(const char *str, const char c);

#endif