#ifndef JAVAC_JAVAP_H
#define JAVAC_JAVAP_H

#define MINOR_Version_Name               "minor version"
#define MAJOR_Version_Name               "major version"

#define CONSTANT_Utf8_Name               "Utf8"
#define CONSTANT_Integer_Name            "Integer"
#define CONSTANT_Float_Name              "Float"
#define CONSTANT_Long_Name               "Long"
#define CONSTANT_Double_Name             "Double"
#define CONSTANT_Class_Name              "Class"
#define CONSTANT_String_Name             "String"
#define CONSTANT_Fieldref_Name           "Field_ref"
#define CONSTANT_Methodref_Name          "Method_ref"
#define CONSTANT_InterfaceMethodref_Name "InterfaceMethod_ref"
#define CONSTANT_NameAndType_Name        "NameAndType"
#define CONSTANT_MethodHandle_Name       "MethodHandle"
#define CONSTANT_MethodType_Name         "MethodType"
#define CONSTANT_InvokeDynamic_Name      "InvokeDynamic"

#define ACC_PUBLIC          0x0001
#define ACC_PUBLIC_Name     "public"
#define ACC_PRIVATE         0x0002
#define ACC_PRIVATE_Name    "private"
#define ACC_PROTECTED       0x0004
#define ACC_PROTECTED_Name  "protected"
#define ACC_STATIC          0x0008
#define ACC_STATIC_Name     "static"
#define ACC_FINAL	        0x0010
#define ACC_FINAL_Name      "final"
#define ACC_SUPER	        0x0020
#define ACC_SUPER_Name      "super"
#define ACC_VOLATILE	    0x0040
#define ACC_VOLATILE_Name   "volatile"
#define ACC_TRANSIENT	    0x0080
#define ACC_TRANSIENT_Name  "transient"
#define ACC_INTERFACE	    0x0200
#define ACC_INTERFACE_Name  "interface"
#define ACC_ABSTRACT	    0x0400
#define ACC_ABSTRACT_Name   "abstract"
#define ACC_SYNTHETIC	    0x1000
#define ACC_SYNTHETIC_Name  "synthetic"
#define ACC_ANNOTATION	    0x2000
#define ACC_ANNOTATION_Name "annotation"
#define ACC_ENUM	        0x4000
#define ACC_ENUM_Name       "enum"

typedef struct {
    int16_t start_pc;
    int16_t end_pc;
    int16_t handler_pc;
    int16_t catch_type;
} exception_table;

typedef struct {
    int16_t max_stack;
    int16_t max_locals;
    int32_t code_length;
    struct buffer *code;
    int16_t exception_table_length;
    struct vector *exception_table;
    int16_t attributes_length;
    struct vector *attributes;
} Code_attribute;
#endif