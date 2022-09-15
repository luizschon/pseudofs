#ifndef __FILESYSTEM_H
#define __FILESYSTEM_H

/* Header para funções relacionadas ao sistema de arquivos. */

#include <stdlib.h>
#include <stdio.h>

typedef char block;

typedef enum { CONTIGUOUS, LINKED, INDEXED } alloc_type;

typedef struct {
    char name;
    size_t size;
    union {
        size_t index_block;
        size_t first_block;
    };
} file_attr_t;

typedef struct {
    size_t file_count;
    file_attr_t *attributes;
} dir_t;

typedef struct {
    alloc_type type;
    size_t size;
    block *blocks;
} fs_t;

fs_t * fs_init(alloc_type type, size_t size);
fs_t * create_filesystem(FILE *operations_file);
void fs_populate_blocks(FILE *operations_file, fs_t *filesystem, unsigned int num_files);
void fs_add_file(fs_t *filesystem, size_t idx, char filename, size_t size);
void fs_destroy(fs_t *filesystem);
void fs_create_file(fs_t *fs, char filename, size_t size);
void fs_delete_file(fs_t *fs, char filename);
void dump_blocks(fs_t *filesystem);

#endif // __FILESYSTEM_H

