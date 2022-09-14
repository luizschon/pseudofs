#ifndef __FILESYSTEM_H
#define __FILESYSTEM_H

/* Header para funções relacionadas ao sistema de arquivos. */

#include <stdlib.h>
#include <stdio.h>

typedef char block;

typedef struct {
    int alloc_type;
    size_t size;
    block *blocks;
} fs_t;

fs_t * fs_init(int alloc_type, size_t size);
void fs_populate_blocks(FILE *operations_file, fs_t *filesystem, unsigned int num_files);
void fs_add_file(fs_t *filesystem, size_t idx, char filename, size_t size);
void dump_blocks(fs_t *filesystem);

#endif // __FILESYSTEM_H

