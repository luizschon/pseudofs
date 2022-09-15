#ifndef __OPERATIONS_H
#define __OPERATIONS_H

/* Header para funções relationadas a operações. */

#include <stdlib.h>
#include "processes.h"

typedef enum { CREATE,  DELETE  } opcode;
typedef enum { FAILURE, SUCCESS } status;

typedef struct {
    int process_id;
    opcode code;
    char filename;
    size_t n_blocks;
} op_t;

typedef struct {
    unsigned int op_number;
    status s;
    char *description;
} op_result_info_t;

typedef struct {
    size_t size;
    op_result_info_t *res_arr;
} op_log_t;

// op_result_info_t * op_result_info_init(unsigned int op_num, status s, char *desc);
op_log_t * op_log_init();
void op_log_append(op_log_t *log, op_result_info_t res);
void op_log_destroy(op_log_t *log);
void dump_log(op_log_t *log);
void execute_operations(const char *filename, p_list_t *process_list);

#endif // __OPERATIONS_H
