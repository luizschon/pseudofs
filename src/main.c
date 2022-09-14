#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "processes.h"
#include "filesystem.h"

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "USAGE: %s <processes file> <files file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    p_list_t *p_list = parse_processes(argv[1]);

#ifndef NDEBUG
    dump_processes(p_list);
#endif

    // Testa funções de fs
    fs_t *fs = fs_init(1, 12);
    fs_add_file(fs, 0, 'A', 3);
    fs_add_file(fs, 4, 'B', 1);
    fs_add_file(fs, 7, 'C', 2);
    dump_blocks(fs);

    p_list_destroy(p_list);

    return 0;
}
