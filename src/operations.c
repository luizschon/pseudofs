#include <stdio.h>
#include "operations.h"
#include "filesystem.h"
#include "utils.h"

/* Inicializa estrutura do tipo op_result_info e retorna ponteiro. */
op_result_info_t * op_result_info_init(unsigned int op_number, int process_id) {
    op_result_info_t *res = (op_result_info_t *) alloc_or_panic(sizeof(op_result_info_t));
    res->op_number = op_number;
    res->process_id = process_id;
    return res;
}

/* Inicializa estrutura do tipo op_log e retorna ponteiro. */
op_log_t * op_log_init() {
    op_log_t *log = (op_log_t *) alloc_or_panic(sizeof(op_log_t));
    log->size = 0;
    log->res_arr = NULL;
    return log;
}

/* Adiciona op_result_info na lista do log. */
void op_log_append(op_log_t *log, op_result_info_t res) {
    log->size++;
    log->res_arr = (op_result_info_t*) realloc_or_panic(log->res_arr, log->size * sizeof(op_result_info_t));
    log->res_arr[log->size - 1] = res;
}

/* Libera memória alocado para o log */
void op_log_destroy(op_log_t *log) {
    free(log->res_arr);
    free(log);
}

/* Imprime entradas do log. */
void dump_log(op_log_t *log) {
    op_result_info_t res;

    // Para cada resultado armazenado no log, imprime status e descrição
    for (size_t i = 0; i < log->size; i++) {
        res = log->res_arr[i];
        printf(COLOR_GRN"\n[INFO]\n"COLOR_RST);
        printf("Operacao n.%d - Operacao do processo %d - %s:\n", res.op_number, res.process_id, res.description);
        printf("    STATUS: %s\n"COLOR_RST, (res.s == SUCCESS) ? COLOR_GRN"SUCESSO" : COLOR_RED"FALHA");
        printf("    RESULTADO: %s\n", res.message);
    }
}

