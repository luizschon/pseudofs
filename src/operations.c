#include <stdio.h>
#include "operations.h"
#include "filesystem.h"
#include "utils.h"

op_log_t * op_log_init() {
    op_log_t *log = (op_log_t *) alloc_or_panic(sizeof(op_log_t));
    log->size = 0;
    log->res_arr = NULL;
    return log;
}

void op_log_append(op_log_t *log, op_result_info_t res) {
    log->size++;
    log->res_arr = (op_result_info_t*) realloc_or_panic(log->res_arr, log->size * sizeof(op_result_info_t));
    log->res_arr[log->size - 1] = res;
}

void op_log_destroy(op_log_t *log) {
    free(log->res_arr);
    free(log);
}

void dump_log(op_log_t *log) {
    op_result_info_t res;

    // Para cada resultado armazenado no log, imprime status e descrição
    for (size_t i = 0; i < log->size; i++) {
        res = log->res_arr[i];
        printf(COLOR_GRN"\n[INFO]\n"COLOR_RST);
        printf("Operacao n.%d:\n", res.op_number);
        printf("    STATUS: %s\n"COLOR_RST, (res.s == SUCCESS) ? COLOR_GRN"SUCESSO" : COLOR_RED"FALHA");
        printf("    DESCRICAO: %s\n", res.description);
    }
}

void parse_operations(const char *filename, p_list_t *process_list) {
    FILE * op_file = NULL;
    op_file = fopen_or_panic(filename, "r");

    alloc_type type;
    size_t disk_size;
    unsigned int num_file;

    fs_t * filesystem = create_filesystem(op_file);

    fclose(op_file);
}
