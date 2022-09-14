#include "files.h"
#include "utils.h"

file_t * file_init(char name, size_t size) {
    file_t *file = alloc_or_panic(sizeof(file_t));
    file->name = name;
    file->size = size;
    return file;
}
