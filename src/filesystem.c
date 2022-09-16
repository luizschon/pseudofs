#include <string.h>
#include "filesystem.h"
#include "utils.h"

fs_t * fs_init(alloc_type type, size_t size) {
    fs_t *filesystem = alloc_or_panic(sizeof(fs_t));
    filesystem->type = type;
    filesystem->size = size;
    filesystem->blocks = alloc_or_panic(size * sizeof(block_t));
    filesystem->free_blocks = alloc_or_panic(size * sizeof(int));
    filesystem->root.file_count = 0;
    filesystem->root.attributes = NULL;

    // Inicializa disco com caracteres 0 (zero) e marca blocos como livres
    for (size_t i = 0; i < size; i++) {
        filesystem->blocks[i].filename = '0';
        filesystem->free_blocks[i] = 1;
    }

    return filesystem;
}

/* Função responsável por criar sistema de arquivos usando dados providos
 * no arquivo de operações. */
fs_t * create_filesystem(FILE *op_file) {
    alloc_type type;
    size_t disk_size;
    unsigned int num_files;

    // Lê informações sobre o sistema de arquivos no .txt de operações
    fscanf(op_file, "%d\n%zu\n%u\n", &type, &disk_size, &num_files);

    fs_t *filesystem = fs_init(type, disk_size);
    fs_populate_blocks(op_file, filesystem, num_files);

    return filesystem;
}

/* Função que popula disco com arquivos armazenados inicialmente, providos
 * pelo arquivo de operações. */
void fs_populate_blocks(FILE *op_file, fs_t *filesystem, unsigned int num_files) {
    // Variáveis para armazenar valores lidos do arquivo
    char filename;
    size_t idx, size;

    // Lê num_files linhas do arquivo de operações: arquivos inicialmente no disco
    for (unsigned int i = 0; i < num_files; i++) {
        fscanf(op_file, "%c, %zu, %zu\n", &filename, &idx, &size);

        // Checa se o arquivo sendo inserido respeita os limites do disco
        if (idx <= filesystem->size && idx+size <= filesystem->size) {
            for (size_t i = 0; i < size; i++) {
                filesystem->blocks[idx+i].filename = filename;
                filesystem->free_blocks[idx+i] = 0;
            }
            fs_add_to_root(filesystem, 0, filename, size, idx);
        } else {
            fprintf(stderr, COLOR_RED"[ERRO]"COLOR_RST" Nao pode inserir arquivo no disco, "
                    "arquivo %c ultrapassa limite de armazenamento.\n", filename);
        }
    }
}

void fs_add_to_root(fs_t *fs, int owner_proc_id, char filename, size_t size, size_t first_block) {
    // Inicializa atributos do arquivo
    file_attr_t file;
    file.owner_proc_id = owner_proc_id;
    file.name = filename;
    file.size = size;
    file.first_block = first_block;

    // Adiciona atributos do arquivo no array de atributos de arquivos no root
    file_attr_t *file_attr_arr = fs->root.attributes;
    fs->root.file_count++;

    fs->root.attributes = realloc_or_panic(file_attr_arr, fs->root.file_count * sizeof(file_attr_t));
    fs->root.attributes[fs->root.file_count-1] = file;
}

void fs_remove_from_root(fs_t *fs, char filename) {

}

file_attr_t * fs_get_file_attr(fs_t *fs, char filename) {
    size_t i;
    int file_count = fs->root.file_count;
    file_attr_t *attributes = fs->root.attributes;

    // Busca linear no diretório raiz, caso não encontre, retorna nulo
    for (i = 0; i < file_count; i++) {
        if (attributes[i].name == filename)
            return &attributes[i];
    }
    return NULL;
}

status contiguous_create_file(fs_t *fs, int process_id, char filename, size_t size, char *res_message) {
    size_t i, j;
    int contiguous_blocks;

    for (i = 0; i < fs->size; i += j+1) {
        contiguous_blocks = 0;
        for (j = 0; j < size; j++) {
            if (fs->free_blocks[i+j] == 1)
                contiguous_blocks++;

            if (contiguous_blocks == size) {
                for (size_t k = 0; k < size; k++) {
                    fs->blocks[i+k].filename = filename;
                    fs->free_blocks[i+k] = 0;
                }
                fs_add_to_root(fs, process_id, filename, size, i);
                snprintf(res_message, BUFFER_SIZE, "O processo %d criou o arquivo %c.", process_id, filename);
                return SUCCESS;
            }
        }
    }
    snprintf(res_message, BUFFER_SIZE, "O processo %d nao criou o arquivo %c (falta de espaco).", process_id, filename);
    return FAILURE;
}

status linked_create_file(fs_t *fs, int process_id, char filename, size_t size, char *res_message) {
    return FAILURE;
}

status indexed_create_file(fs_t *fs, int process_id, char filename, size_t size, char *res_message) {
    return FAILURE;
}

/* Função que recebe arquivo para criado e decide aonde no
 * disco o arquivo será inserido baseado no tipo de alocação
 * e os blocos livres e ocupa esses blocos. */
status fs_create_file(fs_t *fs, int process_id, char filename, size_t size, char *res_message) {
    status s = FAILURE;

    switch (fs->type) {
        case CONTIGUOUS:
            s = contiguous_create_file(fs, process_id, filename, size, res_message);
            break;
        case LINKED:
            s = linked_create_file(fs, process_id, filename, size, res_message);
            break;
        case INDEXED:
            s = indexed_create_file(fs, process_id, filename, size, res_message);
            break;
    }
    return s;
}

status contiguous_delete_file(fs_t *fs, process_t *process, file_attr_t *attributes, char *res_message) {
    int idx = attributes->first_block;

    if (process->priority != 0 && attributes->owner_proc_id != process->id) {
        snprintf(res_message, BUFFER_SIZE, "O Processo %d não pode deletar o arquivo %c porque nao foi criado por ele.", process->id, attributes->name);
        return FAILURE;
    }

    for (size_t j = 0; j < attributes->size; j++) {
        fs->blocks[idx+j].filename = '0';
        fs->free_blocks[idx+j] = 1;
    }
    fs_remove_from_root(fs, attributes->name);

    snprintf(res_message, BUFFER_SIZE, "O processo %d deletou o arquivo %c.", process->id, attributes->name);
    return SUCCESS;
}

status linked_delete_file(fs_t *fs, process_t *process, file_attr_t *attributes, char *res_message) {
    return FAILURE;
}

status indexed_delete_file(fs_t *fs, process_t *process, file_attr_t *attributes, char *res_message) {
    return FAILURE;
}

/* Função que recebe arquivo para ser deleteado, sobrescreve
 * os blocos e os marca como blocos livres. */
status fs_delete_file(fs_t *fs, process_t *process, char filename, char *res_message) {
    status s = FAILURE;
    file_attr_t *attributes = fs_get_file_attr(fs, filename);

    if (attributes != NULL) {
        switch (fs->type) {
            case CONTIGUOUS:
                s = contiguous_delete_file(fs, process, attributes, res_message);
                break;
            case LINKED:
                s = linked_delete_file(fs, process, attributes, res_message);
                break;
            case INDEXED:
                s = indexed_delete_file(fs, process, attributes, res_message);
                break;
        }
    } else {
        snprintf(res_message, BUFFER_SIZE, "Arquivo %c nao existe.", filename);
    }
    return s;
}

void fs_destroy(fs_t *filesystem) {
    free(filesystem->blocks);
    free(filesystem->free_blocks);
    free(filesystem->root.attributes);
    free(filesystem);
}

void simulate_fs(FILE *op_file, fs_t *filesystem, p_list_t *process_list) {
    op_log_t *log = op_log_init();
    unsigned int op_count = 1;

    // Variáveis para armazenar valores lidos do arquivo
    int process_id;
    opcode code;
    char filename;
    size_t file_size;

    // Lê operações no arquivo
    while (fscanf(op_file, "%d, %d, %c, %zu", &process_id, &code, &filename, &file_size) != EOF) {
        // Inicializa estruturas de dados para armazenar resultado das operações
        op_t operation;
        operation.process_id = process_id;
        operation.code = code;
        operation.filename = filename;
        operation.n_blocks = file_size;

        op_result_info_t *res = op_result_info_init(op_count, operation);
        op_count++;

        process_t * proc = get_process(process_list, process_id);

        if (code == CREATE)
            snprintf(res->description, BUFFER_SIZE, "Operacao do processo %d - Criar arquivo %c", process_id, filename);
        else
            snprintf(res->description, BUFFER_SIZE, "Operacao do processo %d - Deletar arquivo %c", process_id, filename);


        if (proc == NULL) {
            res->s = FAILURE;
            snprintf(res->message, BUFFER_SIZE, "O processo %d nao existe", process_id);
        } else if (proc->cpu_time == 0) {
            res->s = FAILURE;
            snprintf(res->message, BUFFER_SIZE, "O tempo de execucao do processo %d ja acabou", process_id);
        } else {
            proc->cpu_time--;

            if (code == CREATE)
                res->s = fs_create_file(filesystem, process_id, filename, file_size, res->message);
            else
                res->s = fs_delete_file(filesystem, proc, filename, res->message);
        } 

        // Adiciona resultado no log
        op_log_append(log, *res);
        free(res);
        dump_blocks(filesystem);
    }
    dump_log(log);
    op_log_destroy(log);

    fclose(op_file);
}

void dump_blocks(fs_t *filesystem) {
    size_t size = filesystem->size;

    // Imprime linha separadora de cima
    printf("-");
    for (size_t i = 0; i < 4*size; i++)
        printf("-");
    printf("\n");

    // Imprime valores no disco com separador '|'
    printf("|");
    for (size_t i = 0; i < size; i++)
        printf(" %c |", filesystem->blocks[i].filename);
    printf("\n");

    // Imprime linha separadora de baixo
    printf("-");
    for (size_t i = 0; i < 4*size; i++)
        printf("-");
    printf("\n");
}
