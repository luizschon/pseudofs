#include <stdio.h>
#include "utils.h"

void * alloc_or_panic (size_t size) {
    void *ptr = NULL;
    ptr = malloc(size);

    if (ptr == NULL) {
        fprintf(stderr, "[ERRO] Nao foi possivel alocar %zu blocos de memoria.\n", size);
        exit(EXIT_FAILURE);
    }
    return ptr;
}

void * realloc_or_panic (void *ptr, size_t size) {
    void *ptr_aux = NULL;
    ptr_aux = realloc(ptr, size);

    if (ptr_aux == NULL) {
        fprintf(stderr, "[ERRO] Nao foi possivel realocar %zu blocos de memoria.\n", size);
        exit(EXIT_FAILURE);
    }
    return ptr_aux;
}

