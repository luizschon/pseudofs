#ifndef __FILESYSTEM_H
#define __FILESYSTEM_H

/* Header para funções relacionadas ao sistema de arquivos. */

#include <stdlib.h>
#include <stdio.h>

typedef char block;

typedef enum { CONTIGUOUS, LINKED, INDEXED } alloc_type;

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
void dump_blocks(fs_t *filesystem);

#endif // __FILESYSTEM_H

