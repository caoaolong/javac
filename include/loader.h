#ifndef JAVAC_LOADER_H
#define JAVAC_LOADER_H

#include "javac.h"

typedef struct {
    struct vector *class;
    const char *path;
} loader;

#define CONSTANT_Utf8               1
#define CONSTANT_Integer            3
#define CONSTANT_Float              4
#define CONSTANT_Long               5
#define CONSTANT_Double             6
#define CONSTANT_Class              7
#define CONSTANT_String             8
#define CONSTANT_Fieldref           9
#define CONSTANT_Methodref          10
#define CONSTANT_InterfaceMethodref 11
#define CONSTANT_NameAndType        12
#define CONSTANT_MethodHandle       15
#define CONSTANT_MethodType         16
#define CONSTANT_InvokeDynamic      18

typedef struct {
    int8_t tag;
    union {
        // class | string | method type
        struct {
            int16_t index;
        } bit16;
        // field/method/interafce ref | name and type | invoke dynamic
        struct {
            int16_t class_index;
            int16_t index;
        } bit1616;
        // integer float
        struct {
            int32_t bytes;
        } bit32;
        // double long
        struct {
            int32_t bytes_h;
            int32_t bytes_l;
        } bit64;
        // utf8
        struct {
            int16_t length;
            struct buffer *value;
        } utf8;
        // method handle
        struct {
            int8_t reference_kind;
            int16_t reference_index;
        } bit816;
    } value;
} cp_info;

typedef struct {
    int16_t attribute_name_index;
    int16_t attribute_length;
    struct buffer *info;
} attribute_info;

typedef struct {
    int16_t access_flags;
    int16_t name_index;
    int16_t descriptor_index;
    int16_t attributes_count;
    struct vector *attributes;
} field_info;

typedef field_info method_info;

typedef struct {
    const char *name;
    int32_t magic;
    struct {
        int16_t minor;
        int16_t major;
    } version;
    int16_t constant_pool_count;
    struct vector *constant_pool;
    int16_t accss_flags;
    int16_t this_class;
    int16_t super_class;
    int16_t interfaces_count;
    int16_t *interfaces;
    int16_t fields_count;
    struct vector *fields;
    int16_t methods_count;
    struct vector *methods;
    int16_t attributes_count;
    struct vector *attributes;
} class;

loader *loader_create();
void loader_init(loader *class_loader, const char *classpath);
void loader_load(loader *class_loader, const char *prefix);
void loader_append_entry(loader *class_loader, class *cls, const char *name);

int32_t parse_read_int32(struct buffer *data);
int16_t parse_read_int16(struct buffer *data);

#endif