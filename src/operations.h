#ifndef __OPERATIONS_H
#define __OPERATIONS_H

/* Header para funções relationadas a operações. */

#include <stdlib.h>
#include <stdio.h>
#include "processes.h"

#define BUFFER_SIZE 256

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
    char description[BUFFER_SIZE];
    char message[BUFFER_SIZE];
    op_t operation;
} op_result_info_t;

typedef struct {
    size_t size;
    op_result_info_t *res_arr;
} op_log_t;

// op_result_info_t * op_result_info_init(unsigned int op_num, status s, char *desc);
op_result_info_t * op_result_info_init(unsigned int op_number, op_t operation);
op_log_t * op_log_init();
void op_log_append(op_log_t *log, op_result_info_t res);
void op_log_destroy(op_log_t *log);
void dump_log(op_log_t *log);

#endif // __OPERATIONS_H
