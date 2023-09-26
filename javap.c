#include "javac.h"
#include "javap.h"
#include "loader.h"
#include <stdio.h>

void print_flags(int flags);
void print_class_info(class *cls);
void print_class_version(class *cls);
void print_constant_pool(class *cls);
void print_class_methods(class *cls);
void print_attribute(class *cls, attribute_info *ai);
void print_code_attribute(struct buffer *info, int16_t size);
void print_code(struct buffer *codes, unsigned char code);
int64_t ci32toi64(int32_t bl, int32_t bh);

int main(int argc, char *argv[])
{
    loader *class_loader = loader_create();
    loader_init(class_loader, CLASSPATH);
    class *cls = NULL;
    cls = vector_peek(class_loader->class);
    print_class_info(cls);
    print_class_version(cls);
    print_constant_pool(cls);
    print_class_methods(cls);
    return 0;
}

int64_t ci32toi64(int32_t bl, int32_t bh)
{
    int64_t vl = bh;
    int64_t vh = bl;
    return (vh << 32) | vl;
}

char *get_utf8(class *cls, int16_t index)
{
    cp_info *ci = (cp_info *)vector_peek_at(cls->constant_pool, index - 1);
    if (ci->tag == CONSTANT_Utf8) {
        return (char *)buffer_ptr((ci->value.utf8.value));
    }
    return NULL;
}

static void print_class(class *cls, int16_t index)
{
    cp_info *ci = (cp_info *)vector_peek_at(cls->constant_pool, index - 1);
    if (ci->tag == CONSTANT_Class) {
        printf("%s ", get_utf8(cls, ci->value.bit16.index));
    }
}

void print_flags(int flags)
{
    if (flags & ACC_PUBLIC) {
        printf("%s ", ACC_PUBLIC_Name);
    } else if (flags & ACC_PROTECTED) {
        printf("%s ", ACC_PROTECTED_Name);
    } else if (flags & ACC_PRIVATE) {
        printf("%s ", ACC_PRIVATE_Name);
    }
    if (flags & ACC_STATIC) {
        printf("%s ", ACC_STATIC_Name);
    }
    if (flags & ACC_VOLATILE) {
        printf("%s ", ACC_VOLATILE_Name);
    }
    if (flags & ACC_TRANSIENT) {
        printf("%s ", ACC_TRANSIENT_Name);
    }
    if (flags & ACC_ABSTRACT) {
        printf("%s ", ACC_ABSTRACT_Name);
    }
    if (flags & ACC_FINAL) {
        printf("%s ", ACC_FINAL_Name);
    }
    if (flags & ACC_INTERFACE) {
        printf("%s ", ACC_INTERFACE_Name);
    }
}

void print_class_info(class *cls)
{
    print_flags(cls->accss_flags);
    printf("class ");
    print_class(cls, cls->this_class);
    if (cls->super_class > 0) {
        printf("extends ");
        print_class(cls, cls->super_class);
    }
    printf("\n");
}

void print_class_version(class *cls)
{
    printf("  %s: %d\n", MINOR_Version_Name, cls->version.minor);
    printf("  %s: %d\n", MAJOR_Version_Name, cls->version.major);
}

void print_constant_pool(class *cls)
{
    cp_info *ci = NULL;
    int index = 1;
    printf("Constant pool:\n");
    for (int i = 0; i < cls->constant_pool_count - 1; i++) {
        ci = vector_peek_at(cls->constant_pool, i);
        if (ci->tag == 0) {
            continue;
        }
        printf("  ");
        switch (ci->tag) {
            case CONSTANT_Utf8:
                printf("#%3d = %-20s%s\n", index++, CONSTANT_Utf8_Name, 
                    (char*)buffer_ptr(ci->value.utf8.value));
                break;
            case CONSTANT_Integer:break;
            case CONSTANT_Float:break;
            case CONSTANT_Long:
                printf("#%3d = %-20s%lldl\n", index++, CONSTANT_Long_Name, 
                    ci32toi64(ci->value.bit64.bytes_h, ci->value.bit64.bytes_l));
                index++;
                break;
            case CONSTANT_Double:
                printf("#%3d = %-20s%lld\n", index++, CONSTANT_Double_Name, 
                    ci32toi64(ci->value.bit64.bytes_h, ci->value.bit64.bytes_l));
                index++;
                break;
            case CONSTANT_Class:
                printf("#%3d = %-20s#%d\n", index++, CONSTANT_Class_Name, 
                    ci->value.bit16.index);
                break;
            case CONSTANT_String:break;
            case CONSTANT_Fieldref:break;
            case CONSTANT_Methodref:
                printf("#%3d = %-20s#%d.#%d\n", index++, CONSTANT_Methodref_Name, 
                    ci->value.bit1616.class_index, ci->value.bit1616.index);
                break;
            case CONSTANT_InterfaceMethodref:break;
            case CONSTANT_NameAndType:
                printf("#%3d = %-20s#%d.#%d\n", index++, CONSTANT_NameAndType_Name, 
                    ci->value.bit1616.class_index, ci->value.bit1616.index);
                break;
            case CONSTANT_MethodHandle:break;
            case CONSTANT_MethodType:break;
            case CONSTANT_InvokeDynamic:break;
            default: break;
        }
    }
}

void print_class_methods(class *cls)
{
    printf("{\n");
    method_info *mi = NULL;
    for (int i = 0; i < cls->methods_count; i++) {
        mi = (method_info *)vector_peek_at(cls->methods, i);
        printf("  ");
        print_flags(mi->access_flags);
        if (!strcmp(get_utf8(cls, mi->name_index), "<init>")) {
            print_class(cls, cls->this_class);
        }
        printf("%s:\n", get_utf8(cls, mi->descriptor_index));
        if (mi->attributes_count > 0) {
            attribute_info *ai = NULL;
            for (int k = 0; k < mi->attributes_count; k++) {
                ai = vector_peek_at(mi->attributes, k);
                print_attribute(cls, ai);
            }
        }
    }
    printf("}\n");
}

void print_attribute(class *cls, attribute_info *ai)
{
    char *attr = get_utf8(cls, ai->attribute_name_index);
    printf("    %s:\n", attr);
    if (!strcmp(attr, "Code")) {
        print_code_attribute(ai->info, ai->attribute_length);
    }
}

void print_code_attribute(struct buffer *info, int16_t size)
{
    Code_attribute c_ai;
    c_ai.max_stack = parse_read_int16(info);
    c_ai.max_locals = parse_read_int16(info);
    c_ai.code_length = parse_read_int32(info);
    if (c_ai.code_length > 0) {
        c_ai.code = buffer_create();
        for (int i = 0; i < c_ai.code_length; i++) {
            buffer_write(c_ai.code, buffer_read(info));
        }
    }
    printf("      stack=%d, locals=%d\n", 
        c_ai.max_stack, c_ai.max_locals);
    while (true) {
        if (c_ai.code->rindex == c_ai.code->len) {
            break;
        }
        printf("        ");
        print_code(c_ai.code, (unsigned char)buffer_read(c_ai.code));
    }
}

void print_code(struct buffer *codes, unsigned char code)
{
    switch (code) {
        case 0x32:
            printf("aaload\n");
            break;
        case 0x53:
            printf("aastore\n");
            break;
        case 0x01:
            printf("aconst_null\n");
            break;
        case 0x19:
            printf("aload #%d\n", buffer_read(codes));
            break;
        case 0x2a:
            printf("aload_0\n");
            break;
        case 0x2b:
            printf("aload_1\n");
            break;
        case 0x2c:
            printf("aload_2\n");
            break;
        case 0x2d:
            printf("aload_3\n");
            break;
        case 0xbd:
            printf("anewarray #%d #%d\n", buffer_read(codes), buffer_read(codes));
            break;
        case 0xb0:
            printf("areturn\n");
            break;
        case 0xbe:
            printf("arraylength\n");
            break;
        case 0x3a:
            printf("astore #%d\n", buffer_read(codes));
            break;
        case 0x4b:
            printf("astore_0\n");
            break;
        case 0x4c:
            printf("astore_1\n");
            break;
        case 0x4d:
            printf("astore_2\n");
            break;
        case 0x4e:
            printf("astore_3\n");
            break;
        case 0xbf:
            printf("athrow\n");
            break;
        case 0x33:
            printf("baload\n");
            break;
        case 0x54:
            printf("bastore\n");
            break;
        case 0x10:
            printf("bipush #%d\n", buffer_read(codes));
            break;
        case 0x34:
            printf("caload\n");
            break;
        case 0x55:
            printf("castore\n");
            break;
        case 0xc0:
            printf("checkcast #%d #%d\n", buffer_read(codes), buffer_read(codes));
            break;
        case 0x90:
            printf("d2f\n");
            break;
        case 0x8e:
            printf("d2i\n");
            break;
        case 0x8f:
            printf("d2l\n");
            break;
        case 0x63:
            printf("dadd\n");
            break;
        case 0x31:
            printf("daload\n");
            break;
        case 0x52:
            printf("dastore\n");
            break;
        case 0x98:
            printf("dcmpg\n");
            break;
        case 0x97:
            printf("dcmpl\n");
            break;
        case 0xe:
            printf("dconst_0\n");
            break;
        case 0xf:
            printf("dconst_1\n");
            break;
        case 0x6f:
            printf("ddiv\n");
            break;
        case 0x18:
            printf("dload #%d\n", buffer_read(codes));
            break;
        case 0x26:
            printf("dload_0\n");
            break;
        case 0x27:
            printf("dload_1\n");
            break;
        case 0x28:
            printf("dload_2\n");
            break;
        case 0x29:
            printf("dload_3\n");
            break;
        case 0x6b:
            printf("dmul\n");
            break;
        case 0x77:
            printf("dneg\n");
            break;
        case 0x73:
            printf("drem\n");
            break;
        case 0xaf:
            printf("dreturn\n");
            break;
        case 0x39:
            printf("dstore #%d\n", buffer_read(codes));
            break;
        case 0x47:
            printf("dstore_0\n");
            break;
        case 0x48:
            printf("dstore_1\n");
            break;
        case 0x49:
            printf("dstore_2\n");
            break;
        case 0x4a:
            printf("dstore_3\n");
            break;
        case 0x67:
            printf("dsub\n");
            break;
        case 0x59:
            printf("dup\n");
            break;
        case 0x5a:
            printf("dup_x1\n");
            break;
        case 0x5b:
            printf("dup_x2\n");
            break;
        case 0x5c:
            printf("dup2\n");
            break;
        case 0x5d:
            printf("dup2_x1\n");
            break;
        case 0x5e:
            printf("dup2_x2\n");
            break;
        case 0x8d:
            printf("f2d\n");
            break;
        case 0x8b:
            printf("f2i\n");
            break;
        case 0x8c:
            printf("f2l\n");
            break;
        case 0x62:
            printf("fadd\n");
            break;
        case 0x30:
            printf("faload\n");
            break;
        case 0x51:
            printf("fastore\n");
            break;
        case 0x96:
            printf("fcmpg\n");
            break;
        case 0x95:
            printf("fmpl\n");
            break;
        case 0xb:
            printf("fconst_0\n");
            break;
        case 0xc:
            printf("fconst_1\n");
            break;
        case 0xd:
            printf("fconst_2\n");
            break;
        case 0x6e:
            printf("fdiv\n");
            break;
        case 0x17:
            printf("fload #%d\n", buffer_read(codes));
            break;
        case 0x22:
            printf("fload_0\n");
            break;
        case 0x23:
            printf("fload_1\n");
            break;
        case 0x24:
            printf("fload_2\n");
            break;
        case 0x25:
            printf("fload_3\n");
            break;
        case 0x6a:
            printf("fmul\n");
            break;
        case 0x76:
            printf("fneg\n");
            break;
        case 0x72:
            printf("frem\n");
            break;
        case 0xae:
            printf("freturn\n");
            break;
        case 0x38:
            printf("fstore\n");
            break;
        case 0x43:
            printf("fstore_0\n");
            break;
        case 0x44:
            printf("fstore_1\n");
            break;
        case 0x45:
            printf("fstore_2\n");
            break;
        case 0x46:
            printf("fstore_3\n");
            break;
        case 0x66:
            printf("fsub\n");
            break;
        case 0xb4:
            printf("getfield #%d #%d\n", buffer_read(codes), buffer_read(codes));
            break;
        case 0xb2:
            printf("getstatic #%d #%d\n", buffer_read(codes), buffer_read(codes));
            break;
        case 0xa7:
            printf("goto #%d #%d\n", buffer_read(codes), buffer_read(codes));
            break;
        case 0xc8:
            printf("goto_w #%d #%d #%d #%d\n", 
                buffer_read(codes), buffer_read(codes), buffer_read(codes), buffer_read(codes));
            break;
        case 0x91:
            printf("i2b\n");
            break;
        case 0x92:
            printf("i2c\n");
            break;
        case 0x87:
            printf("i2d\n");
            break;
        case 0x86:
            printf("i2f\n");
            break;
        case 0x85:
            printf("i2l\n");
            break;
        case 0x93:
            printf("i2s\n");
            break;
        case 0x60:
            printf("iadd\n");
            break;
        case 0x2e:
            printf("iaload\n");
            break;
        case 0x7e:
            printf("iand\n");
            break;
        case 0x4f:
            printf("iastore\n");
            break;
        case 0x2:
            printf("iconst_m1\n");
            break;
        case 0x3:
            printf("iconst_0\n");
            break;
        case 0x4:
            printf("iconst_1\n");
            break;
        case 0x5:
            printf("iconst_2\n");
            break;
        case 0x6:
            printf("iconst_3\n");
            break;
        case 0x7:
            printf("iconst_4\n");
            break;
        case 0x8:
            printf("iconst_5\n");
            break;
        case 0x6c:
            printf("idiv\n");
            break;
        case 0xa5:
            printf("if_acmpeq #%d #%d\n", buffer_read(codes), buffer_read(codes));
            break;
        case 0xa6:
            printf("if_acmpne #%d #%d\n", buffer_read(codes), buffer_read(codes));
            break;
        case 0x9f:
            printf("if_icmpeq #%d #%d\n", buffer_read(codes), buffer_read(codes));
            break;
        case 0xa0:
            printf("if_icmpne #%d #%d\n", buffer_read(codes), buffer_read(codes));
            break;
        case 0xa1:
            printf("if_icmplt #%d #%d\n", buffer_read(codes), buffer_read(codes));
            break;
        case 0xa2:
            printf("if_icmpge #%d #%d\n", buffer_read(codes), buffer_read(codes));
            break;
        case 0xa3:
            printf("if_icmpgt #%d #%d\n", buffer_read(codes), buffer_read(codes));
            break;
        case 0xa4:
            printf("if_icmple #%d #%d\n", buffer_read(codes), buffer_read(codes));
            break;
        case 0x99:
            printf("ifeq #%d #%d\n", buffer_read(codes), buffer_read(codes));
            break;
        case 0x9a:
            printf("ifne #%d #%d\n", buffer_read(codes), buffer_read(codes));
            break;
        case 0x9b:
            printf("iflt #%d #%d\n", buffer_read(codes), buffer_read(codes));
            break;
        case 0x9c:
            printf("ifge #%d #%d\n", buffer_read(codes), buffer_read(codes));
            break;
        case 0x9d:
            printf("ifgt #%d #%d\n", buffer_read(codes), buffer_read(codes));
            break;
        case 0x9e:
            printf("ifle #%d #%d\n", buffer_read(codes), buffer_read(codes));
            break;
        case 0xc7:
            printf("ifnonnull #%d #%d\n", buffer_read(codes), buffer_read(codes));
            break;
        case 0xc6:
            printf("ifnull #%d #%d\n", buffer_read(codes), buffer_read(codes));
            break;
        case 0x84:
            printf("iinc #%d #%d\n", buffer_read(codes), buffer_read(codes));
            break;
        case 0x15:
            printf("iload #%d\n", buffer_read(codes));
            break;
        case 0x1a:
            printf("iload_0 #%d\n", buffer_read(codes));
            break;
        case 0x1b:
            printf("iload_1 #%d\n", buffer_read(codes));
            break;
        case 0x1c:
            printf("iload_2 #%d\n", buffer_read(codes));
            break;
        case 0x1d:
            printf("iload_3 #%d\n", buffer_read(codes));
            break;
        case 0x68:
            printf("imul\n");
            break;
        case 0x74:
            printf("ineg\n");
            break;
        case 0xc1:
            printf("instanceof #%d #%d\n", buffer_read(codes), buffer_read(codes));
            break;
        case 0xba:
            printf("invokedynamic #%d #%d #%d #%d\n", 
                buffer_read(codes), buffer_read(codes), buffer_read(codes), buffer_read(codes));
            break;
        case 0xb9:
            printf("invokeinterface #%d #%d #%d #%d\n", 
                buffer_read(codes), buffer_read(codes), buffer_read(codes), buffer_read(codes));
            break;
        case 0xb8:
            printf("invokestatic #%d #%d\n", buffer_read(codes), buffer_read(codes));
            break;
        case 0xb7:
            printf("invokespecial #%d #%d\n", buffer_read(codes), buffer_read(codes));
            break;
        case 0xb6:
            printf("invokevirtual #%d #%d\n", buffer_read(codes), buffer_read(codes));
            break;
        case 0x80:
            printf("ior\n");
            break;
        case 0x70:
            printf("irem\n");
            break;
        case 0xac:
            printf("ireturn\n");
            break;
        case 0x78:
            printf("ishl\n");
            break;
        case 0x7a:
            printf("ishr\n");
            break;
        case 0x36:
            printf("istore #%d\n", buffer_read(codes));
            break;
        case 0x3b:
            printf("istore_0\n");
            break;
        case 0x3c:
            printf("istore_1\n");
            break;
        case 0x3d:
            printf("istore_2\n");
            break;
        case 0x3e:
            printf("istore_3\n");
            break;
        case 0x64:
            printf("isub\n");
            break;
        case 0x7c:
            printf("iushr\n");
            break;
        case 0x82:
            printf("ixor\n");
            break;
        case 0xa8:
            printf("jsr\n");
            break;
        case 0xc9:
            printf("jsr_w #%d #%d #%d #%d\n", 
                buffer_read(codes), buffer_read(codes), buffer_read(codes), buffer_read(codes));
            break;
        case 0x8a:
            printf("l2d\n");
            break;
        case 0x89:
            printf("l2f\n");
            break;
        case 0x88:
            printf("l2i\n");
            break;
        case 0x61:
            printf("ladd\n");
            break;
        case 0x2f:
            printf("laload\n");
            break;
        case 0x7f:
            printf("land\n");
            break;
        case 0x50:
            printf("lastore\n");
            break;
        case 0x94:
            printf("lcmp\n");
            break;
        case 0x9:
            printf("lconst_0\n");
            break;
        case 0xa:
            printf("lconst_1\n");
            break;
        case 0x12:
            printf("ldc #%x\n", buffer_read(codes));
            break;
        case 0x13:
            printf("ldc_w #%x #%x\n", buffer_read(codes), buffer_read(codes));
            break;
        case 0x14:
            printf("ldc2_w #%x #%x\n", buffer_read(codes), buffer_read(codes));
            break;
        case 0x6d:
            printf("ldiv\n");
            break;
        case 0x16:
            printf("lload #%x\n", buffer_read(codes));
            break;
        case 0x1e:
            printf("lload_0\n");
            break;
        case 0x1f:
            printf("lload_1\n");
            break;
        case 0x20:
            printf("lload_2\n");
            break;
        case 0x21:
            printf("lload_3\n");
            break;
        case 0x69:
            printf("lmul\n");
            break;
        case 0x75:
            printf("lneg\n");
            break;
        case 0xab:
            // TODO: lookupswitch
            printf("lookupswitch\n");
            break;
        case 0xaa:
            // TODO: tableswitch
            printf("tableswitch\n");
            break;
        case 0x81:
            printf("lor\n");
            break;
        case 0x71:
            printf("lrem\n");
            break;
        case 0xad:
            printf("lreturn\n");
            break;
        case 0x79:
            printf("lshl\n");
            break;
        case 0x7b:
            printf("lshr\n");
            break;
        case 0x37:
            printf("lstore\n");
            break;
        case 0x3f:
            printf("lstore_0\n");
            break;
        case 0x40:
            printf("lstore_1\n");
            break;
        case 0x41:
            printf("lstore_2\n");
            break;
        case 0x42:
            printf("lstore_3\n");
            break;
        case 0x65:
            printf("lsub\n");
            break;
        case 0x7d:
            printf("lushr\n");
            break;
        case 0x83:
            printf("lxor\n");
            break;
        case 0xc2:
            printf("monitorenter\n");
            break;
        case 0xc3:
            printf("monitorexit\n");
            break;
        case 0xc5:
            printf("multianewarray #%d #%d #%d\n", 
                buffer_read(codes), buffer_read(codes), buffer_read(codes));
            break;
        case 0xbb:
            printf("new #%d #%d\n", buffer_read(codes), buffer_read(codes));
            break;
        case 0xbc:
            printf("newarray #%d\n", buffer_read(codes));
            break;
        case 0x0:
            printf("nop\n");
            break;
        case 0x57:
            printf("pop\n");
            break;
        case 0x58:
            printf("pop2\n");
            break;
        case 0xb5:
            printf("putfield #%d #%d\n", buffer_read(codes), buffer_read(codes));
            break;
        case 0xb3:
            printf("putstatic #%d #%d\n", buffer_read(codes), buffer_read(codes));
            break;
        case 0xa9:
            printf("ret #%d\n", buffer_read(codes));
            break;
        case 0xb1:
            printf("return\n");
            break;
        case 0x35:
            printf("saload\n");
            break;
        case 0x56:
            printf("sastore\n");
            break;
        case 0x11:
            printf("sipush #%d #%d\n", buffer_read(codes), buffer_read(codes));
            break;
        case 0x5f:
            printf("swap\n");
            break;
        case 0xc4:
            // TODO: wide
            printf("wide\n");
            break;
    }
}