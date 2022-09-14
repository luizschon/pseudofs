#ifndef __FILES_H
#define __FILES_H

/* Header para funções relacionadas a arquivos. */

#include <stdlib.h>

typedef struct {
    char name;
    size_t size;
} file_t;

file_t * file_init(char name, size_t size);

#endif // __FILES_H
