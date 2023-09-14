#ifndef JAVAC_LOADER_H
#define JAVAC_LOADER_H

#include "javac.h"

typedef struct {
    struct vector *class;
} loader;

loader *loader_create();
void loader_init(loader *class_loader, const char *classpath);
void loader_append_entry(loader *class_loader, const char *name);

#endif