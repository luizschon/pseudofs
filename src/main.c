#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "processes.h"

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "USAGE: %s <processes file> <files file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    p_list_t *p_list = parse_processes(argv[1]);

#ifndef NDEBUG
    dump_processes(p_list);
#endif

    p_list_destroy(p_list);

    return 0;
}
