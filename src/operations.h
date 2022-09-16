#ifndef __OPERATIONS_H
#define __OPERATIONS_H

/* Header para funções relationadas a operações. */

#include <stdlib.h>
#include <stdio.h>
#include "processes.h"

#define BUFFER_SIZE 256

typedef enum { CREATE,  DELETE  } opcode;
typedef enum { FAILURE, SUCCESS } status;

/* Estrutura que armazena informações relevantes do
 * resultado de cada operação feita no disco. */
typedef struct {
    unsigned int op_number;         // Número da operação
    int process_id;                 // ID do processo
    status s;                       // Status da operação (SUCCESS, FAILURE)
    char description[BUFFER_SIZE];  // Descrição da operação
    char message[BUFFER_SIZE];      // Descrição do resultado da operação
} op_result_info_t;

/* Estrutura que armazena o log da execução das operações, i.e.
 * um array de op_result_info. */
typedef struct {
    size_t size;
    op_result_info_t *res_arr;
} op_log_t;

op_result_info_t * op_result_info_init(unsigned int op_number, int process_id);
op_log_t * op_log_init();
void op_log_append(op_log_t *log, op_result_info_t res);
void op_log_destroy(op_log_t *log);
void dump_log(op_log_t *log);

#endif // __OPERATIONS_H
