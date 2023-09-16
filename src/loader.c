#include "loader.h"
#include <stdbool.h>
#include <zip.h>
#include <string.h>

static int32_t parse_read_int32(struct buffer *data)
{
    int i = 0;
    i |= (buffer_read(data) & 0xFF) << 24;;
    i |= (buffer_read(data) & 0xFF) << 16;
    i |= (buffer_read(data) & 0xFF) << 8;
    i |= buffer_read(data) & 0xFF;
    return i;
}

static int16_t parse_read_int16(struct buffer *data)
{
    int16_t i = 0;
    i |= (buffer_read(data) & 0xFF) << 8;
    i |= buffer_read(data) & 0xFF;
    return i;
}

static struct buffer* parse_read_utf8(struct buffer *data, int16_t length)
{
    struct buffer *value = buffer_create();
    char c;
    while (length --) {
        buffer_write(value, buffer_read(data));
    };
    return value;
}

static void parse_class_version(class *cls, struct buffer *data)
{
    cls->version.minor = parse_read_int16(data);
    cls->version.major = parse_read_int16(data);
}

static void parse_class_constant_pool(class *cls, struct buffer *data)
{
    cls->constant_pool_count = parse_read_int16(data);
    cp_info ci = { 0 };
    for (int i = 0; i < cls->constant_pool_count - 1; i++) {
        ci.tag = buffer_read(data);
        switch (ci.tag) {
            case CONSTANT_Class:
            case CONSTANT_String:
            case CONSTANT_MethodType:
                ci.value.bit16.index = parse_read_int16(data);
                break;
            case CONSTANT_Fieldref:
            case CONSTANT_Methodref:
            case CONSTANT_InterfaceMethodref:
            case CONSTANT_NameAndType:
            case CONSTANT_InvokeDynamic:
                ci.value.bit1616.class_index = parse_read_int16(data);
                ci.value.bit1616.index = parse_read_int16(data);
                break;
            case CONSTANT_Integer:
            case CONSTANT_Float:
                ci.value.bit32.bytes = parse_read_int32(data);
                break;
            case CONSTANT_Long:
            case CONSTANT_Double:
                ci.value.bit64.bytes_h = parse_read_int32(data);
                ci.value.bit64.bytes_l = parse_read_int32(data);
                break;
            case CONSTANT_Utf8: {
                int16_t length = parse_read_int16(data);
                ci.value.utf8.length = length;
                ci.value.utf8.value = parse_read_utf8(data, length);
            }
                break;
            case CONSTANT_MethodHandle:
                ci.value.bit816.reference_kind = buffer_read(data);
                ci.value.bit816.reference_index = parse_read_int16(data);
                break;
        }
    }
}

static class* loader_parse(struct buffer *data) 
{
    class *cls = malloc(sizeof(class));
    // 魔数
    cls->magic = parse_read_int32(data);
    // 版本号
    parse_class_version(cls, data);
    // 常量池
    cls->constant_pool = vector_create(sizeof(cp_info));
    parse_class_constant_pool(cls, data);
    return cls;
}

static struct buffer* loader_read(zip_t *zip, const char *filename)
{
    char buffer[1024];
    zip_int64_t nbytes = 0;
    zip_file_t *file = zip_fopen(zip, filename, ZIP_FL_COMPRESSED);
    if (!file)
        return NULL;
    struct buffer *result = buffer_create();
    do {
        nbytes = zip_fread(file, buffer, sizeof(buffer));
        for (int i = 0; i < nbytes; i++) {
            buffer_write(result, buffer[i]);
        }
    } while(nbytes == sizeof(buffer));
    zip_fclose(file);
    return result;
}

loader* loader_create()
{
    loader *class_loader = malloc(sizeof(loader));
    class_loader->class = vector_create(sizeof(char*));
    return class_loader;
}

void loader_load(loader *class_loader, const char *prefix)
{
    char buffer[1024];
    zip_t *zip = zip_open(class_loader->path, 0, NULL);
    if (zip == NULL) {
        return;
    }
    zip_int64_t num_files = zip_get_num_entries(zip, ZIP_FL_UNCHANGED);
    zip_file_t *file = NULL;
    zip_int64_t nbytes = 0;
    for (int i = 0; i < num_files; i++) {
        const char *filename = zip_get_name(zip, i, 0);
        if (!strendswith(filename, ".class")) {
            continue;
        }
        if (strstartswith(filename, prefix)) {
            loader_append_entry(class_loader, filename);
            // 解析类
            struct buffer* class = loader_read(zip, filename);
            loader_parse(class);
        }
    }
    zip_close(zip);
}

void loader_init(loader *class_loader, const char *classpath)
{
    class_loader->path = classpath;
    loader_load(class_loader, "java/lang/");
}

void loader_append_entry(loader *class_loader, const char *name)
{
    struct buffer *buffer = buffer_create();
    for (int i = 0; i < strlen(name) - 6; i++) {
        if (name[i] == '/') {
            buffer_write(buffer, '.');
        } else {
            buffer_write(buffer, name[i]);
        }
    }
    buffer_write(buffer, 0);
    vector_push(class_loader->class, buffer);
}