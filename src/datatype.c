#include "parser.h"
#include "token.h"

datatype *datatype_create()
{
    datatype *dt = calloc(1, sizeof(datatype));
    return dt;
}

void datatype_set_name(node *n, datatype *dtype)
{
    size_t size = strlen(n->value.sval.val) + 1;
    dtype->name = malloc(size);
    strncpy((void*)dtype->name, n->value.sval.val, size);
}

bool datatype_parse_array(node *n, datatype *dtype)
{
    static bool array = false;
    if (n->value.sval.ttype == TOKEN_TYPE_SYMBOL) {
        if (SEQ(n->value.sval.val, "["))
            array = true;
        else if (SEQ(n->value.sval.val, "]") && array)
            array = false;
    } else if (n->value.sval.ttype == TOKEN_TYPE_NUMBER && array) {
        long array_length = strtol(n->value.sval.val, NULL, 10);
        dtype->size = dtype->size * array_length;
    } else 
        return false;

    return true;
}

bool datatype_parse_flags(node *n, datatype *dtype)
{
    if (n->value.sval.ttype != TOKEN_TYPE_KEYWORD) 
        return false;

    if (SEQ(n->value.sval.val, "public")) {
        dtype->flags |= DATATYPE_FLAG_ACCESS_PUBLIC;
    } else if (SEQ(n->value.sval.val, "protected")) {
        dtype->flags |= DATATYPE_FLAG_ACCESS_PROTECTED;
    } else if (SEQ(n->value.sval.val, "private")) {
        dtype->flags |= DATATYPE_FLAG_ACCESS_PRIVATE;
    } else if (SEQ(n->value.sval.val, "static")) {
        dtype->flags |= DATATYPE_FLAG_STATIC;
    } else if (SEQ(n->value.sval.val, "final")) {
        dtype->flags |= DATATYPE_FLAG_FINAL;
    } else if (SEQ(n->value.sval.val, "volatile")) {
        dtype->flags |= DATATYPE_FLAG_VOLATILE;
    } else if (SEQ(n->value.sval.val, "transient")) {
        dtype->flags |= DATATYPE_FLAG_TRANSIENT;
    } else if (SEQ(n->value.sval.val, "abstract")) {
        dtype->flags |= DATATYPE_FLAG_ABSTRACT;
    } else if (SEQ(n->value.sval.val, "extends")) {
        dtype->flags |= DATATYPE_FLAG_EXTENDS;
    } else if (SEQ(n->value.sval.val, "implements")) {
        dtype->flags |= DATATYPE_FLAG_IMPLEMENTS;
    } else
        return false;
    
    return true;
}

bool datatype_parse_type(node *n, datatype* dtype)
{
    if (n->value.sval.ttype != TOKEN_TYPE_KEYWORD)
        return false;

    if (SEQ(n->value.sval.val, "void")) {
        dtype->type = DATA_TYPE_VOID;
        dtype->size = 0;
    } else if (SEQ(n->value.sval.val, "byte")) {
        dtype->type = DATA_TYPE_BYTE;
        dtype->size = 1;
    } else if (SEQ(n->value.sval.val, "char")) {
        dtype->type = DATA_TYPE_CHAR;
        dtype->size = 2;
    } else if (SEQ(n->value.sval.val, "int")) {
        dtype->type = DATA_TYPE_INTEGER;
        dtype->size = 4;
    } else if (SEQ(n->value.sval.val, "long")) {
        dtype->type = DATA_TYPE_LONG;
        dtype->size = 8;
    } else if (SEQ(n->value.sval.val, "float")) {
        dtype->type = DATA_TYPE_FLOAT;
        dtype->size = 4;
    } else if (SEQ(n->value.sval.val, "double")) {
        dtype->type = DATA_TYPE_DOUBLE;
        dtype->size = 8;
    } else if (SEQ(n->value.sval.val, "boolean")) {
        dtype->type = DATA_TYPE_BOOLEAN;
        dtype->size = 1;
    } else
        return false;

    return true;
}