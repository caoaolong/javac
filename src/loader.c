#include "loader.h"
#include <stdbool.h>
#include <zip.h>
#include <string.h>

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