#include <stdio.h>
#include "utils.h"

/* Função que aloca memória na heap usando malloc e finaliza 
 * programa com mensagem de erro caso a memória não seja reservada. */
void * alloc_or_panic(size_t size) {
    void *ptr = NULL;
    ptr = malloc(size);

    if (ptr == NULL) {
        fprintf(stderr, "[ERRO] Nao foi possivel alocar %zu blocos de memoria.\n", size);
        exit(EXIT_FAILURE);
    }
    return ptr;
}

/* Função que realoca memória na heap usando realloc e finaliza 
 * programa com mensagem de erro caso o bloco de memória não possa
 * ser extendido. */
void * realloc_or_panic(void *ptr, size_t size) {
    void *ptr_aux = NULL;
    ptr_aux = realloc(ptr, size);

    if (ptr_aux == NULL) {
        fprintf(stderr, "[ERRO] Nao foi possivel realocar %zu blocos de memoria.\n", size);
        exit(EXIT_FAILURE);
    }
    return ptr_aux;
}

/* Função que lê um arquivo e finaliza programa com mensagem de erro
 * caso o arquivo não tenha sido aberto. */
FILE * fopen_or_panic(const char *filename, const char *mode) {
    FILE * file = NULL;
    file = fopen(filename, mode);

    if (file == NULL) {
        fprintf(stderr, COLOR_RED"[ERRO]"COLOR_RST" Nao foi possivel abrir o arquivo %s.\n", filename);
        exit(EXIT_FAILURE);
    }
    return file;
}
