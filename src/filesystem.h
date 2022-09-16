#ifndef __FILESYSTEM_H
#define __FILESYSTEM_H

/* Header para funções relacionadas ao sistema de arquivos. */

#include <stdlib.h>
#include <stdio.h>
#include "operations.h"

#define MAX 1000

typedef enum { CONTIGUOUS = 1, LINKED = 2, INDEXED = 3 } alloc_type;
typedef int* bitmap;

/* Uma "Super" struct contendo todos os dados que um bloco do
 * disco (de qualquer tipo de alocação) pode conter. */
typedef struct {
    char filename;      // Nome do arquivo (TODOS), efetivamente o dado armazenado no disco
    int next;           // Ponteiro (índice do disco) para o próximo bloco do arquivo (ALOC. ENCADEADA)
    size_t size;        // Tamanho da tabela de índices (ALOC. INDEXADA)
    int indexes[MAX];   // Tabela de índices dos blocos de um arquivo (ALOC. INDEXADA)
} block_t;

/* Estrutura que armazena os atributos dos arquivos amarzenados. */
typedef struct {
    int initial;        // Flag que denota se o arquivo foi inicializado junto com o disco
    int owner_proc_id;  // ID do processo que criou o arquivo
    char name;          // Nome do arquivo
    size_t size;        // Tamanho do arquivo efetivo (após ser inserido no disco)
    size_t first_block; // Ponteiro (índice do disco) para o primeiro bloco do arquivo
} file_attr_t;

/* Estrutura que representa o diretório onde os arquivos estão
 * armazenados, implementando uma lista de atributos do arquivos. */
typedef struct {
    size_t file_count;          // Número de arquivos na lista
    file_attr_t *attributes;    // Lista de atributos de arquivos
} dir_t;

/* Estrutura que representa o sistema de arquivo sendo utilizado
 * na simulação. */
typedef struct {
    alloc_type type;    // Tipo de alocação (CONTÍGUA, ENCADEADA, INDEXADA)
    size_t size;        // Tamanho do disco
    block_t *blocks;    // Array de blocos do disco
    dir_t root;         // Diretório raiz do sistema de arquivos (onde se encontram os atributos dos arquivos)
    bitmap free_blocks; // Bitmap dos blocos livres
} fs_t;

fs_t * fs_init(alloc_type type, size_t size);
fs_t * create_filesystem(FILE *operations_file);
void fs_populate_blocks(FILE *operations_file, fs_t *filesystem, unsigned int num_files);
void fs_destroy(fs_t *filesystem);

int fs_count_free_blocks(fs_t *fs);
file_attr_t * fs_get_file_attr(fs_t *fs, char filename);
void fs_add_to_root(fs_t *fs, int owner_proc_id, char filename, size_t size, size_t first_block, int initial);
void fs_remove_from_root(fs_t *fs, char filename);

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

