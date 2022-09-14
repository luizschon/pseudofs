#ifndef __PROCESSES_H
#define __PROCESSES_H

#include <stdlib.h>

/* Header para funções relacionadas a processos. */

/* Estrutura que armazena informações providas sobre um 
 * processo (ID, prioridade, tempo de CPU). */
typedef struct {
    int id;
    int priority;
    int cpu_time;
} process_t;

/* Estrutura representando lista de processos */
typedef struct {
    size_t size;
    process_t *processes;
} p_list_t;

p_list_t * parse_processes(char *filename);
void dump_processes(p_list_t * list);

p_list_t * p_list_init(void);
void p_list_append(p_list_t *list, process_t process);
void p_list_destroy(p_list_t *list);

#endif // __PROCESSES_H
