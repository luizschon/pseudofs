#ifndef __FILESYSTEM_H
#define __FILESYSTEM_H

/* Header para funções relacionadas ao sistema de arquivos. */

#include <stdlib.h>
#include <stdio.h>
#include "operations.h"

#define MAX 1000

typedef enum { CONTIGUOUS = 1, LINKED = 2, INDEXED = 3 } alloc_type;
typedef int* bitmap;

typedef struct {
    char filename;
    int next;
    size_t size;
    int indexes[MAX];
} block_t;

typedef struct {
    int initial;
    int owner_proc_id;
    char name;
    size_t size;
    size_t first_block;
} file_attr_t;

typedef struct {
    size_t file_count;
    file_attr_t *attributes;
} dir_t;

typedef struct {
    alloc_type type;
    size_t size;
    block_t *blocks;
    dir_t root;
    bitmap free_blocks;
} fs_t;

fs_t * fs_init(alloc_type type, size_t size);
fs_t * create_filesystem(FILE *operations_file);
void fs_populate_blocks(FILE *operations_file, fs_t *filesystem, unsigned int num_files);
void fs_add_to_root(fs_t *fs, int owner_proc_id, char filename, size_t size, size_t first_block, int initial);
void fs_remove_from_root(fs_t *fs, char filename);
void fs_destroy(fs_t *filesystem);
file_attr_t * fs_get_file_attr(fs_t *fs, char filename);
int fs_count_free_blocks(fs_t *fs);

status contiguous_create_file(fs_t *fs, int process_id, char filename, size_t size, char *err_message);
status linked_create_file(fs_t *fs, int process_id, char filename, size_t size, char *err_message);
status indexed_create_file(fs_t *fs, int process_id, char filename, size_t size, char *err_message);
status fs_create_file(fs_t *fs, int process_id, char filename, size_t size, char *err_message);

status contiguous_delete_file(fs_t *fs, process_t *process, file_attr_t *attributes, char *err_message);
status linked_delete_file(fs_t *fs, process_t *process, file_attr_t *attributes, char *err_message);
status indexed_delete_file(fs_t *fs, process_t *process, file_attr_t *attributes, char *err_message);
status fs_delete_file(fs_t *fs, process_t * process, char filename, char *err_message);

void simulate_fs(FILE *file, fs_t *filesystem, p_list_t *process_list);
void dump_blocks(fs_t *filesystem);

#endif // __FILESYSTEM_H

