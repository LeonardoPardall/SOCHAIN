
#include <stdio.h>
#include <stddef.h> 
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <memory.h>
#include <fcntl.h>
#include <string.h>




void* allocate_dynamic_memory(int size) {
    void *buffer;                                 
    if ((buffer = calloc(1,size)) == NULL) {     
        printf("Erro ao alocar memória\n");
        return NULL;
    }
    return buffer;
}


void* create_shared_memory(char* name, int size) {
    void * ptr;
    char memoryName[128];  
    snprintf(memoryName, sizeof(memoryName), "/%s%d", name,getuid());       // concatena o nome do ficheiro com userid
    int fd = shm_open(memoryName, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);     // gerar o fd da memória com as respectivas flags

    // Verifica se houve um erro ao criar o fd      
    if ( fd == -1) {  
        perror("Erro ao criar memória compartilhada.");
        return NULL;
    }              
    // ajusta o tamanho da memória para 'size'            
    if (ftruncate(fd,size) == -1) {
        close(fd);
        perror("Erro ao truncar");
        return NULL;
    }
    ptr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);  

    // verifica se o mapeamento falhou 
    if (ptr == MAP_FAILED) {                                 // MAP_FAILED = (void*)-1
        close(fd);
        perror("Erro ao mapear a memória compartilhada");
        return NULL; 
    }
   // fecha o file descriptor e retorna o pointer 
    close(fd);                                                         
    return ptr;

}

void deallocate_dynamic_memory(void *ptr) {
    free(ptr);
}


void destroy_shared_memory(char* name, void* ptr, int size) {
    char memoryName[128];
    snprintf(memoryName, sizeof(memoryName), "/%s%d", name,getuid());        // concatena o nome do ficheiro com userid
    // Liberta a memória e verifica se falhou
    if (munmap(ptr, size) == -1) {
        perror("Erro libertar a memória!");
    }                       
    // Apaga a memória e verifica se falhou                             
    if (shm_unlink(memoryName) == -1) {
        perror("Erro ao apagar a memória!");
    }
}



void write_main_wallets_buffer(struct ra_buffer* buffer, int buffer_size, struct transaction* tx) {

    for (int i =0; i < buffer_size; i++) {
        if (buffer->ptrs[i] == 0) {             // encontra posição livre
            buffer->ptrs[i] = 1;                // coloca como ocupada
            buffer->buffer[i] = *tx;            // escreve o conteúdo de tx
            break;
        }
    }
}

void write_wallets_servers_buffer(struct circ_buffer* buffer, int buffer_size, struct transaction* tx) {

    int i = buffer->ptrs->in;
    int j = buffer->ptrs->out;

    // verifica se o buffer está cheio 
    if ((i+1)%buffer_size == j) {          
        return;
    }

    buffer->buffer[i] = *tx;                        // escreve o conteúdo de tx na posição livre
    buffer->ptrs->in = (i+1)%buffer_size;           // atualiza o índice de entrada

}


void write_servers_main_buffer(struct ra_buffer* buffer, int buffer_size, struct transaction* tx) {


    for (int i = 0; i < buffer_size; i++) {
        if (buffer->ptrs[i] == 0) {                     // encontra posição livre   
            buffer->ptrs[i] = 1;                        // coloca como ocupada
            buffer->buffer[i] = *tx;                    // escreve o conteúdo de tx
            break;
        }
    }
}


void read_main_wallets_buffer(struct ra_buffer* buffer, int wallet_id, int buffer_size, struct transaction* tx) {

    for (int i = 0; i < buffer_size; i++) {
        if (buffer->ptrs[i] == 1 && buffer->buffer[i].src_id == wallet_id) {       // procura uma posição ocupada cujo src_id é wallet_id                                             
            *tx = buffer->buffer[i];                                               // coloca o conteúdo em tx
            buffer->ptrs[i] = 0;                                                   // coloca a posição como livre                      
            return;                                                                 
        }
    }
    // se não encontrar coloca o id como -1
    tx->id = -1;     

}

void read_wallets_servers_buffer(struct circ_buffer* buffer, int buffer_size, struct transaction* tx) {
    int i = buffer->ptrs->in;                   
    int j = buffer->ptrs->out;

    // verifica se o buffer não está vazio
    if (i != j) {                                            
        *tx = buffer->buffer[j];                               // coloca o conteúdo em tx
        buffer->ptrs->out = (j+1) % buffer_size;               // atualiza o índice de saída
        
    }
    // se não houver nada para ler coloca o id como -1
    else {
        tx->id = -1;          
    }

}


void read_servers_main_buffer(struct ra_buffer* buffer, int tx_id, int buffer_size, struct transaction* tx) {

    for (int i = 0; i < buffer_size; i++) {
        if (buffer->ptrs[i] == 1 && buffer->buffer[i].id == tx_id) {                // procura uma posição ocupada cujo id é tx_id
            *tx = buffer->buffer[i];                                                // coloca o conteúdo em tx
            buffer->ptrs[i] = 0;                                                    // coloca a posição como livre
            return;
        }
    }
    // se não encontrar coloca o id como -1
    tx->id = -1;
}
