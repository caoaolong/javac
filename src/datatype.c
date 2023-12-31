#include "parser.h"
#include "token.h"
#include "loader.h"
#include <stdio.h>

datatype *datatype_create()
{
    datatype *dt = calloc(1, sizeof(datatype));
    return dt;
}

bool datatype_parse_flags(node *n, datatype *dtype)
{
    if (n->value.ttype != TOKEN_TYPE_KEYWORD) 
        return false;

    if (SEQ(n->value.val, "public")) {
        dtype->flags |= DATATYPE_FLAG_ACCESS_PUBLIC;
    } else if (SEQ(n->value.val, "protected")) {
        dtype->flags |= DATATYPE_FLAG_ACCESS_PROTECTED;
    } else if (SEQ(n->value.val, "private")) {
        dtype->flags |= DATATYPE_FLAG_ACCESS_PRIVATE;
    } else if (SEQ(n->value.val, "static")) {
        dtype->flags |= DATATYPE_FLAG_STATIC;
    } else if (SEQ(n->value.val, "final")) {
        dtype->flags |= DATATYPE_FLAG_FINAL;
    } else if (SEQ(n->value.val, "volatile")) {
        dtype->flags |= DATATYPE_FLAG_VOLATILE;
    } else if (SEQ(n->value.val, "transient")) {
        dtype->flags |= DATATYPE_FLAG_TRANSIENT;
    } else if (SEQ(n->value.val, "abstract")) {
        dtype->flags |= DATATYPE_FLAG_ABSTRACT;
    } else if (SEQ(n->value.val, "extends")) {
        dtype->flags |= DATATYPE_FLAG_EXTENDS;
    } else if (SEQ(n->value.val, "implements")) {
        dtype->flags |= DATATYPE_FLAG_IMPLEMENTS;
    } else
        return false;
    
    return true;
}

bool datatype_parse_java_type(node *n, datatype* dtype, loader *class_loader)
{
    vector_set_peek_pointer(class_loader->class, 0);
    struct buffer *name = vector_peek(class_loader->class);
    while (name) {
        if (strendswith(buffer_ptr(name), n->value.val)) {
            n->value.val = buffer_ptr(name);
            return true;
        }
        name = vector_peek(class_loader->class);
    }
    return false;
}

bool datatype_parse_type(node *n, datatype* dtype)
{
    if (n->value.ttype != TOKEN_TYPE_KEYWORD)
        return false;

    if (SEQ(n->value.val, "void")) {
        dtype->type = DATA_TYPE_VOID;
        dtype->size = 0;
    } else if (SEQ(n->value.val, "byte")) {
        dtype->type = DATA_TYPE_BYTE;
        dtype->size = 1;
    } else if (SEQ(n->value.val, "char")) {
        dtype->type = DATA_TYPE_CHAR;
        dtype->size = 2;
    } else if (SEQ(n->value.val, "int")) {
        dtype->type = DATA_TYPE_INTEGER;
        dtype->size = 4;
    } else if (SEQ(n->value.val, "long")) {
        dtype->type = DATA_TYPE_LONG;
        dtype->size = 8;
    } else if (SEQ(n->value.val, "float")) {
        dtype->type = DATA_TYPE_FLOAT;
        dtype->size = 4;
    } else if (SEQ(n->value.val, "double")) {
        dtype->type = DATA_TYPE_DOUBLE;
        dtype->size = 8;
    } else if (SEQ(n->value.val, "boolean")) {
        dtype->type = DATA_TYPE_BOOLEAN;
        dtype->size = 1;
    } else if (SEQ(n->value.val, "class")) {
        dtype->type = DATA_TYPE_CLASS;
        dtype->size = 0;
    } else if (SEQ(n->value.val, "interface")) {
        dtype->type = DATA_TYPE_CLASS;
        dtype->size = 0;
    } else if (SEQ(n->value.val, "enum")) {
        dtype->type = DATA_TYPE_CLASS;
        dtype->size = 0;
    } else if (SEQ(n->value.val, "@interface")) {
        dtype->type = DATA_TYPE_CLASS;
        dtype->size = 0;
    } else 
        return false;

    return true;
}