#include "javac.h"
#include "lexer.h"
#include "ast.h"
#include "parser.h"
#include "helpers/buffer.h"

void write_short(struct buffer *buffer, int16_t value)
{
    for (int i = 1; i >= 0; i--) {
        char c = (value >> (i * 8)) & 0xFF;
        buffer_write(buffer, c);
    }
}

void write_int(struct buffer *buffer, int32_t value)
{
    for (int i = 3; i >= 0; i--) {
        char c = (value >> (i * 8)) & 0xFF;
        buffer_write(buffer, c);
    }
}

void write_string(struct buffer *buffer, const char *value, int16_t len)
{
    int index = 0;
    while (index < len) {
        buffer_write(buffer, *(value + index));
        index ++;
    }
}

void write_const_utf8(struct buffer *pool, const char *value)
{
    buffer_write(pool, CONSTANT_Utf8);
    int16_t len = (int16_t)strlen(value);
    write_short(pool, len);
    write_string(pool, value, len);
}

void write_const_class(struct buffer *pool, int16_t index)
{
    buffer_write(pool, CONSTANT_Class);
    write_short(pool, index);
}

void write_const_string(struct buffer *pool, int16_t index)
{
    buffer_write(pool, CONSTANT_String);
    write_short(pool, index);
}

void write_const_method_type(struct buffer *pool, int16_t index)
{
    buffer_write(pool, CONSTANT_MethodType);
    write_short(pool, index);
}

void write_const_method_handle(struct buffer *pool, int8_t kind, int16_t index)
{
    buffer_write(pool, CONSTANT_MethodHandle);
    buffer_write(pool, kind);
    write_short(pool, index);
}

void write_const_double(struct buffer *pool, int64_t value)
{
    buffer_write(pool, CONSTANT_Double);
    int32_t vh = (value >> 32) & 0xFFFFFFFF;
    int32_t vl = value & 0xFFFFFFFF;
    write_int(pool, vh);
    write_int(pool, vl);
}

void write_const_long(struct buffer *pool, int64_t value)
{
    buffer_write(pool, CONSTANT_Long);
    int32_t vh = (value >> 32) & 0xFFFFFFFF;
    int32_t vl = value & 0xFFFFFFFF;
    write_int(pool, vh);
    write_int(pool, vl);
}

void write_const_integer(struct buffer *pool, int32_t value)
{
    buffer_write(pool, CONSTANT_Integer);
    write_int(pool, value);
}

void write_const_float(struct buffer *pool, int32_t value)
{
    buffer_write(pool, CONSTANT_Float);
    write_int(pool, value);
}

void write_const_field_ref(struct buffer *pool, int16_t class_index, int16_t index)
{
    buffer_write(pool, CONSTANT_Fieldref);
    write_short(pool, class_index);
    write_short(pool, index);
}

void write_const_method_ref(struct buffer *pool, int16_t class_index, int16_t index)
{
    buffer_write(pool, CONSTANT_Methodref);
    write_short(pool, class_index);
    write_short(pool, index);
}

void write_const_interface_method_ref(struct buffer *pool, int16_t class_index, int16_t index)
{
    buffer_write(pool, CONSTANT_InterfaceMethodref);
    write_short(pool, class_index);
    write_short(pool, index);
}

void write_const_name_and_type(struct buffer *pool, int16_t class_index, int16_t index)
{
    buffer_write(pool, CONSTANT_NameAndType);
    write_short(pool, class_index);
    write_short(pool, index);
}

void write_const_invoke_dynamic(struct buffer *pool, int16_t class_index, int16_t index)
{
    buffer_write(pool, CONSTANT_InvokeDynamic);
    write_short(pool, class_index);
    write_short(pool, index);
}

void write_const_pool(struct buffer *class, struct scope_t *scope)
{
    struct buffer *const_pool = buffer_create();
    // Methodref
    // TODO: write
    write_const_method_ref(const_pool, 0, 0);
    struct node_t *n = vector_peek(scope->statements);
    if (n->type & NODE_TYPE_STATEMENT_DECLARE) {
        struct datatype_t *dtype = n->var.type;
        if (dtype->type == DATA_TYPE_CLASS) {
            // TODO: write
            write_const_class(const_pool, 0);
            // object class
            write_const_class(const_pool, 0);
        }
        // init
        write_const_utf8(const_pool, "<init>");
        write_const_utf8(const_pool, "()V");
        // attribute
        write_const_utf8(const_pool, "Code");
        write_const_utf8(const_pool, "LineNumberTable");
        struct vector *func = n->var.body->statements;
        vector_set_peek_pointer(func, 0);
        struct node_t *nf = vector_peek(func);
        while (nf) {
            if (nf->type & NODE_TYPE_STATEMENT_DECLARE && nf->type & NODE_TYPE_STATEMENT_FUNCTION) {
                // function
                write_const_utf8(const_pool, nf->var.name);
                // arguments
                // TODO: write
            }
        }
    }
    write_const_utf8(const_pool, "SourceFile");
    // TODO: write
    // filename
    write_const_utf8(const_pool, "");
    // TODO: write
    write_const_name_and_type(const_pool, 0, 0);
    // TODO: write
    // class
    write_const_utf8(const_pool, "");
    // object
    write_const_utf8(const_pool, "java/lang/Object");
}

int format(lexer_process *process)
{
    FILE *ofp = process->compiler->ofp;
    struct buffer *class = buffer_create();
    // 写入魔数
    write_int(class, MAGIC);
    // 写入版本号
    write_int(class, VERSION);
    // 写入常量池
    write_const_pool(class, process->compiler->root_scope);
    size_t bytes = fwrite(buffer_ptr(class), 1, class->len, ofp);
    if (bytes == class->len) {
        fclose(ofp);
        return JAVAC_FORMAT_OK;
    }
    return JAVAC_FORMAT_ERROR;
}