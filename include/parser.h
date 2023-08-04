#ifndef JAVAC_PARSER_H
#define JAVAC_PARSER_H

#include "ast.h"
#include "lexer.h"
#include "helpers/vector.h"

enum {
    DATATYPE_FLAG_ACCESS_PUBLIC     = 0b00000001,
    DATATYPE_FLAG_ACCESS_PROTECTED  = 0b00000010,
    DATATYPE_FLAG_ACCESS_PRIVATE    = 0b00000100,
    DATATYPE_FLAG_STATIC            = 0b00001000,
    DATATYPE_FLAG_FINAL             = 0b00010000,
    DATATYPE_FLAG_VOLATILE          = 0b00100000,
    DATATYPE_FLAG_TRANSIENT         = 0b01000000,
    DATATYPE_FLAG_ABSTRACT          = 0b10000000,
    DATATYPE_FLAG_EXTENDS          = 0b100000000,
    DATATYPE_FLAG_IMPLEMENTS      = 0b1000000000, 
    DATATYPE_FLAG_ARRAY          = 0b10000000000
};

enum 
{
    DATA_TYPE_VOID,
    DATA_TYPE_BYTE,
    DATA_TYPE_CHAR,
    DATA_TYPE_SHORT,
    DATA_TYPE_INTEGER,
    DATA_TYPE_LONG,
    DATA_TYPE_FLOAT,
    DATA_TYPE_DOUBLE,
    DATA_TYPE_BOOLEAN,
    DATA_TYPE_CLASS,
    DATA_TYPE_INTERFACE,
    DATA_TYPE_ENUM,
    DATA_TYPE_ANNOTATION
};

typedef struct datatype_t {
    int flags;
    int type;
    int size;
    const char *name;
    union {
        struct {
            struct node_t *value;
        } var;
        struct {
            struct node_t *body, *extends;
            struct vector* implements;
        } cls;
        struct {
            struct node_t *body;
        } func;
    };
} datatype;

int parse(lexer_process *process);

datatype *datatype_create(const char *name, int flags, int type, struct node_t *value);
int datatype_get_array_length();
bool datatype_parse_flags(token *tk, datatype* dtype);
bool datatype_parse_type(token *tk, datatype* dtype);

#endif