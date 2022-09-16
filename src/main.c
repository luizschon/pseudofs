#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "processes.h"
#include "filesystem.h"
#include "utils.h"

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "USAGE: %s <processes file> <operations file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Interpreta processos
    p_list_t *processes = parse_processes(argv[1]);

#ifndef NDEBUG
    dump_processes(processes);
#endif

    FILE *op_file = fopen_or_panic(argv[2], "r");

    // Cria sistema de arquivos
    fs_t *fs = create_filesystem(op_file);
    printf("\n----------- INICIALIZACAO\n");
    dump_blocks(fs);

    // Começa e simulação
    simulate_fs(op_file, fs, processes);

    // Libera memória
    p_list_destroy(processes);
    fs_destroy(fs);

    return 0;
}
