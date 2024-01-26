#include "javac.h"
#include "lexer.h"
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

void write_const_method(struct buffer *pool, int8_t kind, int16_t index)
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

void write_const_pool(struct buffer *class, struct vector *tokens)
{
    struct buffer *const_pool = buffer_create();
    vector_set_peek_pointer(tokens, 0);
    token *tk = NULL;
    while (true) {
        tk = vector_peek(tokens);
        if (tk == NULL) break;
        // 排除import
        if (tk->type == TOKEN_TYPE_KEYWORD && !strcmp(tk->sval, "import")) {
            while (true) {
                tk = vector_peek(tokens);
                if (tk->type == TOKEN_TYPE_SYMBOL && tk->cval == ';')
                    break;
            }
        } else if (tk->type == TOKEN_TYPE_IDENTIFIER) {
            printf("%s\n", tk->sval);
        }
    }
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
    write_const_pool(class, process->tokens);
    size_t bytes = fwrite(buffer_ptr(class), 1, class->len, ofp);
    if (bytes == class->len) {
        fclose(ofp);
        return JAVAC_FORMAT_OK;
    }
    return JAVAC_FORMAT_ERROR;
}