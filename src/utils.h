#ifndef __UTILS_H
#define __UTILS_H

/* Header para funções utilitárias que podem ser usadas no projeto. */

#include <stdlib.h>

void * alloc_or_panic(size_t size);
void * realloc_or_panic(void *ptr, size_t size);

#endif // __UTILS_H
