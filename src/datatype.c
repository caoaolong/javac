#include "parser.h"
#include "token.h"

datatype *datatype_create(const char *name, int flags, int type, struct node_t *value)
{
    // TODO: datatype_create
    return NULL;
}

static int array_length = 0;

int datatype_get_array_length()
{
    return array_length;
}

bool datatype_parse_array(token *tk, datatype *dtype)
{
    static bool array = false;
    if (tk->type == TOKEN_TYPE_SYMBOL) {
        if (tk->cval == '[')
            array = true;
        else if (tk->cval == ']' && array)
            array = false;
    } else if (tk->type == TOKEN_TYPE_NUMBER && array)
        array_length = strtol(tk->sval, NULL, 10);
    else 
        return false;

    return true;
}

bool datatype_parse_flags(token *tk, datatype *dtype)
{
    if (tk->type != TOKEN_TYPE_IDENTIFIER) 
        return false;

    if (SEQ(tk->sval, "public")) {
        dtype->flags |= DATATYPE_FLAG_ACCESS_PUBLIC;
    } else if (SEQ(tk->sval, "protected")) {
        dtype->flags |= DATATYPE_FLAG_ACCESS_PROTECTED;
    } else if (SEQ(tk->sval, "private")) {
        dtype->flags |= DATATYPE_FLAG_ACCESS_PRIVATE;
    } else if (SEQ(tk->sval, "static")) {
        dtype->flags |= DATATYPE_FLAG_STATIC;
    } else if (SEQ(tk->sval, "final")) {
        dtype->flags |= DATATYPE_FLAG_FINAL;
    } else if (SEQ(tk->sval, "volatile")) {
        dtype->flags |= DATATYPE_FLAG_VOLATILE;
    } else if (SEQ(tk->sval, "transient")) {
        dtype->flags |= DATATYPE_FLAG_TRANSIENT;
    } else if (SEQ(tk->sval, "abstract")) {
        dtype->flags |= DATATYPE_FLAG_ABSTRACT;
    } else if (SEQ(tk->sval, "transient")) {
        dtype->flags |= DATATYPE_FLAG_TRANSIENT;
    } else if (SEQ(tk->sval, "extends")) {
        dtype->flags |= DATATYPE_FLAG_EXTENDS;
    } else if (SEQ(tk->sval, "implements")) {
        dtype->flags |= DATATYPE_FLAG_IMPLEMENTS;
    } else
        return false;
    
    return true;
}

bool datatype_parse_type(token *tk, datatype* dtype)
{
    if (tk->type != TOKEN_TYPE_KEYWORD)
        return false;

    if (SEQ(tk->sval, "void")) {
        dtype->type = DATA_TYPE_VOID;
        dtype->size = 0;
    } else if (SEQ(tk->sval, "byte")) {
        dtype->type = DATA_TYPE_BYTE;
        dtype->size = 1;
    } else if (SEQ(tk->sval, "char")) {
        dtype->type = DATA_TYPE_CHAR;
        dtype->size = 2;
    } else if (SEQ(tk->sval, "int")) {
        dtype->type = DATA_TYPE_INTEGER;
        dtype->size = 4;
    } else if (SEQ(tk->sval, "long")) {
        dtype->type = DATA_TYPE_LONG;
        dtype->size = 8;
    } else if (SEQ(tk->sval, "float")) {
        dtype->type = DATA_TYPE_FLOAT;
        dtype->size = 4;
    } else if (SEQ(tk->sval, "double")) {
        dtype->type = DATA_TYPE_DOUBLE;
        dtype->size = 8;
    } else if (SEQ(tk->sval, "boolean")) {
        dtype->type = DATA_TYPE_BOOLEAN;
        dtype->size = 1;
    } else
        return false;

    return true;
}