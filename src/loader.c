#include "loader.h"
#include <stdbool.h>
#include <zip.h>
#include <string.h>

int32_t parse_read_int32(struct buffer *data)
{
    int i = 0;
    i |= (buffer_read(data) & 0xFF) << 24;
    i |= (buffer_read(data) & 0xFF) << 16;
    i |= (buffer_read(data) & 0xFF) << 8;
    i |= buffer_read(data) & 0xFF;
    return i;
}

int16_t parse_read_int16(struct buffer *data)
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

static cp_info CP_INFO_NULL = { 0 };

static void parse_class_constant_pool(class *cls, struct buffer *data)
{
    cls->constant_pool_count = parse_read_int16(data);
    cls->constant_pool = vector_create(sizeof(cp_info));
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
        vector_push(cls->constant_pool, &ci);
        if (ci.tag == CONSTANT_Long || ci.tag == CONSTANT_Double) {
            vector_push(cls->constant_pool, &CP_INFO_NULL);
            i++;
        }
    }
}

static void parse_class_interface(class *cls, struct buffer *data)
{
    cls->interfaces_count = parse_read_int16(data);
    if (cls->interfaces_count > 0) {
        cls->interfaces = malloc(sizeof(int16_t) * cls->interfaces_count);
        for (int i = 0; i < cls->interfaces_count; i++) {
            cls->interfaces[i] = parse_read_int16(data);
        }
    }
}

static struct vector *parse_class_attributes(struct buffer *data, int16_t count)
{
    if (count == 0) {
        return NULL;
    }

    struct vector *vec = vector_create(sizeof(attribute_info));
    attribute_info ai;
    for (int i = 0; i < count; i++) {
        ai.attribute_name_index = parse_read_int16(data);
        ai.attribute_length = parse_read_int32(data);
        if (ai.attribute_length > 0) {
            ai.info = buffer_create();
            for (int k = 0; k < ai.attribute_length; k ++) {
                buffer_write(ai.info, buffer_read(data));
            }
        }
        vector_push(vec, &ai);
    }
    return vec;
}

static void parse_class_fields(class *cls, struct buffer *data)
{
    cls->fields_count = parse_read_int16(data);
    cls->fields = vector_create(sizeof(field_info));
    field_info fi;
    for (int i = 0; i < cls->fields_count; i++) {
        fi.access_flags = parse_read_int16(data);
        fi.name_index = parse_read_int16(data);
        fi.descriptor_index = parse_read_int16(data);
        fi.attributes_count = parse_read_int16(data);
        if (fi.attributes_count > 0) {
            fi.attributes = parse_class_attributes(data, fi.attributes_count);
        }
        vector_push(cls->fields, &fi);
    }
}

static void parse_class_methods(class *cls, struct buffer *data)
{
    cls->methods_count = parse_read_int16(data);
    cls->methods = vector_create(sizeof(field_info));
    method_info mi;
    for (int i = 0; i < cls->methods_count; i++) {
        mi.access_flags = parse_read_int16(data);
        mi.name_index = parse_read_int16(data);
        mi.descriptor_index = parse_read_int16(data);
        mi.attributes_count = parse_read_int16(data);
        if (mi.attributes_count > 0) {
            mi.attributes = parse_class_attributes(data, mi.attributes_count);
        }
        vector_push(cls->methods, &mi);
    }
}

static class* loader_parse(struct buffer *data) 
{
    class *cls = malloc(sizeof(class));
    // magic
    cls->magic = parse_read_int32(data);
    // version number
    parse_class_version(cls, data);
    // constant pool
    parse_class_constant_pool(cls, data);
    // access flags
    cls->accss_flags = parse_read_int16(data);
    // this class
    cls->this_class = parse_read_int16(data);
    // super class
    cls->super_class = parse_read_int16(data);
    // interfaces
    parse_class_interface(cls, data);
    // fields
    parse_class_fields(cls, data);
    // methods
    parse_class_methods(cls, data);
    // attributes
    cls->attributes_count = parse_read_int16(data);
    cls->attributes = parse_class_attributes(data, cls->attributes_count);
    return data->len == data->rindex ? cls : NULL;
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
    class_loader->class = vector_create(sizeof(class));
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
        if (strstartswith(filename, prefix) && strindextotal(filename, '/') == 2) {
            // 解析类
            loader_append_entry(class_loader, 
                loader_parse(loader_read(zip, filename)), filename);
        }
    }
    zip_close(zip);
}

void loader_init(loader *class_loader, const char *classpath)
{
    class_loader->path = classpath;
    loader_load(class_loader, "java/lang/");
}

void loader_append_entry(loader *class_loader, class *cls, const char *name)
{
    if (cls == NULL)
        return;
    
    struct buffer *buffer = buffer_create();
    for (int i = 0; i < strlen(name) - 6; i++) {
        if (name[i] == '/') {
            buffer_write(buffer, '.');
        } else {
            buffer_write(buffer, name[i]);
        }
    }
    buffer_write(buffer, 0);
    cls->name = buffer_ptr(buffer);
    vector_push(class_loader->class, cls);
}