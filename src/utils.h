#ifndef __UTILS_H
#define __UTILS_H

/* Header para funções utilitárias que podem ser usadas no projeto. */

#define COLOR_RED "\033[0;31m"
#define COLOR_GRN "\033[0;32m"
#define COLOR_YLW "\033[0;33m"
#define COLOR_RST "\033[0m"

#include <stdlib.h>
#include <stdio.h>

void * alloc_or_panic(size_t size);
void * realloc_or_panic(void *ptr, size_t size);
FILE * fopen_or_panic(const char *filename, const char *mode);

#endif // __UTILS_H
