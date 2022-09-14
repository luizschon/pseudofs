#include <string.h>
#include "filesystem.h"
#include "utils.h"

fs_t * fs_init(int alloc_type, size_t size) {
    fs_t *filesystem = alloc_or_panic(sizeof(fs_t));
    filesystem->alloc_type = alloc_type;
    filesystem->size = size;
    filesystem->blocks = alloc_or_panic(size * sizeof(block));

    // Inicializa disco com caracteres 0 (zero)
    memset(filesystem->blocks, '0', size);

    return filesystem;
}

/* Função que popula disco com arquivos armazenados inicialmente, providos
 * pelo arquivo de operações. */
void fs_populate_blocks(FILE *operations_file, fs_t *filesystem, unsigned int num_files) {
    // Variáveis para armazenar valores lidos do arquivo
    char filename;
    size_t idx, size;

    // Lê num_files linhas do arquivo de operações: arquivos inicialmente no disco
    for (unsigned int i = 0; i < num_files; i++) {
        fscanf(operations_file, "%c, %zu, %zu", &filename, &idx, &size);

        // Checa se o arquivo sendo inserido respeita os limites do disco
        if (idx <= filesystem->size && idx+size <= filesystem->size) {
            memset(&filesystem->blocks[idx], filename, size);
        } else {
            fprintf(stderr, COLOR_RED"[ERRO]"COLOR_RST" Nao pode popular disco, "
                    "arquivo %c ultrapassa limite de armazenamento.\n", filename);
        }
    }
}

void fs_add_file(fs_t *filesystem, size_t idx, char filename, size_t size) {
    // Checa se o arquivo sendo inserido respeita os limites do disco
    if (idx <= filesystem->size && idx+size <= filesystem->size) {
        memset(&filesystem->blocks[idx], filename, size);
    } else {
        fprintf(stderr, COLOR_RED"[ERRO]"COLOR_RST" Nao pode inserir arquivo no disco, "
                "arquivo %c ultrapassa limite de armazenamento.\n", filename);
    }
}

void dump_blocks(fs_t *filesystem) {
    size_t size = filesystem->size;

    // Imprime linha separadora de cima
    printf("-");
    for (size_t i = 0; i < 4*size; i++)
        printf("-");
    printf("\n");

    // Imprime valores no disco com separador '|'
    printf("|");
    for (size_t i = 0; i < size; i++)
        printf(" %c |", filesystem->blocks[i]);
    printf("\n");

    // Imprime linha separadora de baixo
    printf("-");
    for (size_t i = 0; i < 4*size; i++)
        printf("-");
    printf("\n");
}
