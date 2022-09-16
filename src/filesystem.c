#include <string.h>
#include <math.h>
#include "filesystem.h"
#include "utils.h"

/* Função que inicializa estrutura do sistema de arquivos e retorna ponteiro. */
fs_t * fs_init(alloc_type type, size_t size) {
    fs_t *filesystem = alloc_or_panic(sizeof(fs_t)); filesystem->type = type;
    filesystem->size = size;
    filesystem->blocks = alloc_or_panic(size * sizeof(block_t));
    filesystem->root.attributes = NULL;
    filesystem->free_blocks = alloc_or_panic(size * sizeof(int));
    filesystem->root.file_count = 0;
    filesystem->root.attributes = NULL;

    // Inicializa disco com caracteres 0 (zero) e marca blocos como livres
    for (size_t i = 0; i < size; i++) {
        filesystem->blocks[i].filename = '0';
        filesystem->blocks[i].size = 0;
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
            // Insere blocos de forma contígua
            for (size_t i = 0; i < size; i++) {
                filesystem->blocks[idx+i].filename = filename;
                filesystem->free_blocks[idx+i] = 0;
            }
            // Adiciona arquivo no root
            fs_add_to_root(filesystem, 0, filename, size, idx, 1);
        } else {
            fprintf(stderr, COLOR_RED"[ERRO]"COLOR_RST" Nao pode inserir arquivo no disco, "
                    "arquivo %c ultrapassa limite de armazenamento.\n", filename);
        }
    }
}

/* Libera a memória alocada para o sistema de arquivos. */
void fs_destroy(fs_t *filesystem) {
    free(filesystem->blocks);
    free(filesystem->free_blocks);
    free(filesystem->root.attributes);
    free(filesystem);
}

/* Recebe atributos do arquivo e insere no final da lista de atributos
 * do diretório root. */
void fs_add_to_root(fs_t *fs, int owner_proc_id, char filename, size_t size, size_t first_block, int initial) {
    // Inicializa atributos do arquivo
    file_attr_t file;
    file.owner_proc_id = owner_proc_id;
    file.name = filename;
    file.size = size;
    file.initial = initial;
    file.first_block = first_block;

    // Adiciona atributos do arquivo no array de atributos de arquivos no root
    file_attr_t *file_attr_arr = fs->root.attributes;
    fs->root.file_count++;
    fs->root.attributes = realloc_or_panic(file_attr_arr, fs->root.file_count * sizeof(file_attr_t));
    fs->root.attributes[fs->root.file_count-1] = file;
}

/* Recebe nome do arquivo e remove da lista de atributos no diretório
 * root. */
void fs_remove_from_root(fs_t *fs, char filename) {
    int file_count = fs->root.file_count;
    int idx_to_remove = -1;

    // Busca linear para encontrar índice do arquivo a ser removido
    for (size_t i = 0; i < file_count; i++) {
        if (fs->root.attributes[i].name == filename) {
            idx_to_remove = i;
            break;
        }
    }

    // Caso não encontre, imprime mensagem de erro
    if (idx_to_remove == -1) {
        fprintf(stderr, COLOR_RED"[ERRO]"COLOR_RST" Tentativa de remover arquivo do root "
                "que nao existe (arquivo %c).\n", filename);
        return;
    }

    // Aloca array temporário
    file_count = fs->root.file_count--;
    file_attr_t *temp = alloc_or_panic(file_count * sizeof(file_attr_t));

    // Copia valores ANTES do índice a ser removido
    if (idx_to_remove != 0)
        memcpy(temp, fs->root.attributes, idx_to_remove * sizeof(file_attr_t));

    // Copia valores DEPOIS do índice a ser removido
    if (idx_to_remove != file_count-1)
        memcpy(temp, fs->root.attributes+idx_to_remove+1, (file_count-idx_to_remove-1) * sizeof(file_attr_t));
    
    free(fs->root.attributes);
    fs->root.attributes = temp;
}

/* Função que retorna ponteiro para atributos do arquivo cujo nome
 * é recebido por parâmetro. */
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

/* Função que percorre o bitmap de blocos livres e conta os 
 * blocos livres do disco. */
int fs_count_free_blocks(fs_t *fs) {
    int count = 0;

    for (size_t i = 0; i < fs->size; i++)
        if (fs->free_blocks[i] == 1)
            count++;

    return count;
}

/* Função que implementa criação de arquivos usando alocação contígua 
 * usando first-fit. Retorna o status da operação (SUCCESS, FAILURE)*/
status contiguous_create_file(fs_t *fs, int process_id, char filename, size_t size, char *res_message) {
    size_t i, j;
    int contiguous_blocks;

    // Percorre blocos do disco
    for (i = 0; i < fs->size; i += j+1) {
        contiguous_blocks = 0;
        // Percorre size blocos a partir do índice i
        for (j = 0; j < size; j++) {
            if (fs->free_blocks[i+j] == 1)
                contiguous_blocks++;

            // Se o número de blocos livres contíguos for igual ao tamanho
            // do arquivo, armazena ele naquela posição
            if (contiguous_blocks == size) {
                for (size_t k = 0; k < size; k++) {
                    fs->blocks[i+k].filename = filename;
                    fs->free_blocks[i+k] = 0;
                }
                fs_add_to_root(fs, process_id, filename, size, i, 0);
                snprintf(res_message, BUFFER_SIZE, "O processo %d criou o arquivo %c.", process_id, filename);
                return SUCCESS;
            }
        }
    }
    snprintf(res_message, BUFFER_SIZE, "O processo %d nao criou o arquivo %c (falta de espaco).", process_id, filename);
    return FAILURE;
}

/* Função que implementa criação de arquivos usando alocação encadeada 
 * usando first-fit. Retorna o status da operação (SUCCESS, FAILURE)*/
status linked_create_file(fs_t *fs, int process_id, char filename, size_t size, char *res_message) {
    int n_free_blocks = fs_count_free_blocks(fs);
    int prev_idx = -1;
    int first_block = -1;

    // Calcula quantos blocos serão ocupados considerando que 10% é ocupado pelo ponteiro
    int new_size = ceil(size*1.1);

    // Caso o número de blocos livres seja maior que o tamanho do
    // arquivo (contabilizando overhead do ponteiro), insere no 
    // disco
    if (n_free_blocks >= new_size) {
        // Percorre blocos até que todos os blocos do arquivo sejam
        // alocados (first-fit)
        for (int i = 0; i < fs->size && new_size > 0; i++) {
            if (fs->free_blocks[i] == 1) {
                new_size--;

                // Armazena primeiro bloco do arquivo
                if (first_block < 0) {
                    first_block = i;
                }

                // Adiciona ponteiro no bloco anterior ou marca com -1,
                // caso seja o último bloco do arquivo
                if (prev_idx >= 0)
                    fs->blocks[prev_idx].next = i;
                if (new_size == 0)
                    fs->blocks[i].next = -1;

                fs->blocks[i].filename = filename;
                fs->free_blocks[i] = 0;
                prev_idx = i;
            }
        }
        fs_add_to_root(fs, process_id, filename, size, first_block, 0);
        snprintf(res_message, BUFFER_SIZE, "O processo %d criou o arquivo %c.", process_id, filename);
        return SUCCESS;
    } else {
        snprintf(res_message, BUFFER_SIZE, "O processo %d nao criou o arquivo %c (falta de espaco).", process_id, filename);
    }
    return FAILURE;
}

/* Função que implementa criação de arquivos usando alocação indexada 
 * usando first-fit. Retorna o status da operação (SUCCESS, FAILURE)*/
status indexed_create_file(fs_t *fs, int process_id, char filename, size_t size, char *res_message) {
    int n_free_blocks = fs_count_free_blocks(fs);
    int first_block = -1;
    int idx_table_size = 0;
    int *indexes = NULL;

    // O tamanho do arquivo vai ser o original + um bloco de índices
    int new_size = size+1;

    // Caso o número de blocos livres seja maior que o tamanho do
    // arquivo (contabilizando overhead do bloco de índicies), insere
    // no disco
    if (n_free_blocks >= new_size) {
        // Percorre blocos até que todos os blocos do arquivo sejam
        // alocados (first-fit)
        for (size_t i = 0; i < fs->size && new_size > 0; i++) {
            if (fs->free_blocks[i] == 1) {
                new_size--;

                // Armazena primeiro bloco do arquivo (tabela de índices)
                if (first_block < 0) {
                    first_block = i;
                    fs->blocks[i].filename = 'I';
                    fs->free_blocks[i] = 0;
                    continue;
                }

                // Adiciona ponteiro no bloco no bloco de índices
                fs->blocks[first_block].size++;
                idx_table_size = fs->blocks[first_block].size;
                fs->blocks[first_block].indexes[idx_table_size-1] = i;

                fs->blocks[i].filename = filename;
                fs->free_blocks[i] = 0;
            }
        }
        fs_add_to_root(fs, process_id, filename, size, first_block, 0);
        snprintf(res_message, BUFFER_SIZE, "O processo %d criou o arquivo %c.", process_id, filename);
        return SUCCESS;
    } else {
        snprintf(res_message, BUFFER_SIZE, "O processo %d nao criou o arquivo %c (falta de espaco).", process_id, filename);
    }
    return FAILURE;
}

/* Função que recebe arquivo para criado e decide qual função de 
 * criação de arquivo será usada baseado no tipo de alocação do
 * sistema de arquivos. Retorna o status da operação (SUCCESS, FAILURE). */
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

/* Função que implementa deleção de arquivos usando alocação contígua
 * lendo o primeiro bloco e o tamanho do arquivo nos atributos. Retorna
 * o status da operação (SUCCESS, FAILURE). */
status contiguous_delete_file(fs_t *fs, process_t *process, file_attr_t *attributes, char *res_message) {
    int idx = attributes->first_block;

    // Percorre blocos contíguos, limpando os dados
    for (size_t j = 0; j < attributes->size; j++) {
        fs->blocks[idx+j].filename = '0';
        fs->free_blocks[idx+j] = 1;
    }
    fs_remove_from_root(fs, attributes->name);

    snprintf(res_message, BUFFER_SIZE, "O processo %d deletou o arquivo %c.", process->id, attributes->name);
    return SUCCESS;
}

/* Função que implementa deleção de arquivos usando alocação encadeada
 * lendo o primeiro bloco nos atributos e os ponteiros armazenados no 
 * bloco. Retorna o status da operação (SUCCESS, FAILURE). */
status linked_delete_file(fs_t *fs, process_t *process, file_attr_t *attributes, char *res_message) {
    int next = attributes->first_block;

    // Percorre ponteiros de cada bloco do arquivo, limpando os dados
    do {
        fs->blocks[next].filename = '0';
        fs->free_blocks[next] = 1;
        next = fs->blocks[next].next;
    } while (next > 0);

    fs_remove_from_root(fs, attributes->name);
        
    snprintf(res_message, BUFFER_SIZE, "O processo %d deletou o arquivo %c.", process->id, attributes->name);
    return SUCCESS;
}

/* Função que implementa deleção de arquivos usando alocação indexada
 * lendo o primeiro bloco nos atributos (tabela de índices) e os ponteiros
 * (índices) armazenados no tabela. Retorna o status da operação
 * (SUCCESS, FAILURE). */
status indexed_delete_file(fs_t *fs, process_t *process, file_attr_t *attributes, char *res_message) {
    int idx_block = attributes->first_block;
    size_t idx_size = fs->blocks[idx_block].size;
    int *indexes = fs->blocks[idx_block].indexes;

    // Acessa cada bloco indexado pela tabela de índices, limpando os dados
    for (size_t i = 0; i < idx_size; i++) {
        int idx = indexes[i];
        fs->blocks[idx].filename = '0';
        fs->free_blocks[idx] = 1;
    }

    // Limpa bloco contendo tabela de índices
    fs->blocks[idx_block].filename = '0';
    fs->free_blocks[idx_block] = 1;

    fs_remove_from_root(fs, attributes->name);
        
    snprintf(res_message, BUFFER_SIZE, "O processo %d deletou o arquivo %c.", process->id, attributes->name);
    return SUCCESS;
}

/* Função que verifica se um processo tem permissão para deletar um arquivo.
 */
bool can_delete_file(process_t* process, file_attr_t *attributes) {
    // Processo real-time pode deletar qualquer arquivo.
    if (process->priority == 0)
        return true;

    // Arquivos iniciais so podem ser deletados por processo real-time.
    if (attributes->initial)
        return false;

    // Demais processos (de usuario) so podem deletar um raquivo se o forem o dono do arquivo.  
    return attributes->owner_proc_id == process->id;
}

/* Função que recebe arquivo para criado e decide qual função de 
 * deleção de arquivo será usada baseado no tipo de alocação do
 * sistema de arquivos. Retorna o status da operação (SUCCESS, FAILURE). */
status fs_delete_file(fs_t *fs, process_t *process, char filename, char *res_message) {
    file_attr_t *attributes = fs_get_file_attr(fs, filename);

    if (attributes == NULL) {
        snprintf(res_message, BUFFER_SIZE, "Arquivo %c nao existe.", filename);
        return FAILURE;
    }

    if (!can_delete_file(process, attributes)) {
        snprintf(res_message, BUFFER_SIZE, "O Processo %d não pode deletar o arquivo %c porque nao foi criado por ele.", process->id, attributes->name);
        return FAILURE;
    }
    
    if (attributes->initial == 1) {
        return contiguous_delete_file(fs, process, attributes, res_message);
    } 
    
    switch (fs->type) {
        case CONTIGUOUS:
            return contiguous_delete_file(fs, process, attributes, res_message);
        case LINKED:
            return linked_delete_file(fs, process, attributes, res_message);
        case INDEXED:
            return indexed_delete_file(fs, process, attributes, res_message);
    }
}

/* Função que executa a simulação das operações no sistema de arquivos,
 * lendo o arquivo de operações linha por linha e executando-as. */
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
        op_result_info_t *res = op_result_info_init(op_count, process_id);
        op_count++;

        // Busca o process de id 'process_id'
        process_t * proc = get_process(process_list, process_id);

        // Formata string da entrada do log
        if (code == CREATE)
            snprintf(res->description, BUFFER_SIZE, "Criar arquivo %c", filename);
        else
            snprintf(res->description, BUFFER_SIZE, "Deletar arquivo %c", filename);


        // Caso a busca do processo falhe (não há processo com o id
        // provido), sinaliza erro.
        if (proc == NULL) {
            res->s = FAILURE;
            snprintf(res->message, BUFFER_SIZE, "O processo %d nao existe", process_id);
        // Caso o tempo de CPU do processo tenha acabado, sinaliza erro.
        } else if (proc->cpu_time == 0) {
            res->s = FAILURE;
            snprintf(res->message, BUFFER_SIZE, "O tempo de execucao do processo %d ja acabou", process_id);
        // Caso contrária, executa operação
        } else {
            proc->cpu_time--;

            if (code == CREATE)
                res->s = fs_create_file(filesystem, process_id, filename, file_size, res->message);
            else
                res->s = fs_delete_file(filesystem, proc, filename, res->message);
        } 

        // Adiciona resultado no log e imprime mapa do disco
        op_log_append(log, *res);
        printf("\n----------- OPERACAO %d\n", res->op_number);
        dump_blocks(filesystem);
        free(res);
    }
    // Imprime resultado de todas as operações
    dump_log(log);
    dump_blocks(filesystem);
    op_log_destroy(log);

    fclose(op_file);
}

/* Função que imprime imagem (mapa) do estado do disco */
void dump_blocks(fs_t *filesystem) {
    size_t size = filesystem->size;

    printf("\nMapa de ocupacao do disco:\n");

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

    printf("Quantidade de blocos livres: %d\n", fs_count_free_blocks(filesystem));
}
