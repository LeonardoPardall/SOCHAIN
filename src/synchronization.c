#include <semaphore.h>  
#include <fcntl.h>      
#include <sys/stat.h>  
#include <stdio.h>

sem_t* create_semaphore(char *name, unsigned value) {
    sem_t *sem = sem_open(name, O_CREAT | O_EXCL, 0644, value);
    if (sem == SEM_FAILED) {                                        // verifica o pointer
        perror("sem_open failed");
        return NULL;
    }
    return sem;
}


void destroy_semaphore(char *name, sem_t *sem) {
    sem_close(sem);                                 // fecha o semáforo (por processo)
    sem_unlink(name);                              // quebra a estrutura
}


struct semaphores* create_all_semaphores(unsigned v) {
    struct semaphores* sem = malloc(sizeof(struct semaphores));     // alocar espaço para a estrutura
    sem->main_wallet = create_main_wallet_sems( v);    
    sem->wallet_server = create_wallet_server_sems(v);
    sem->server_main = create_server_main_sems(v);
    return sem;
}


// Funcoes pra criar cada um dos semaforos 

struct triplet_sems* create_main_wallet_sems(unsigned v) {
    char *freespace = STR_SEM_MAIN_WALLET_FREESPACE;
    char *unread = STR_SEM_MAIN_WALLET_UNREAD;
    char *mutex = STR_SEM_MAIN_WALLET_MUTEX;
    return create_triplet_sems(v,freespace,unread,mutex);
    
}

struct triplet_sems* create_wallet_server_sems(unsigned v) {
    char *freespace = STR_SEM_WALLET_SERVER_FREESPACE;
    char *unread = STR_SEM_WALLET_SERVER_UNREAD;
    char *mutex = STR_SEM_WALLET_SERVER_MUTEX;
    return create_triplet_sems(v,freespace,unread,mutex);
}

struct triplet_sems* create_server_main_sems(unsigned v) {

    char *freespace = STR_SEM_SERVER_MAIN_FREESPACE;
    char *unread = STR_SEM_SERVER_MAIN_UNREAD;
    char *mutex = STR_SEM_SERVER_MAIN_MUTEX;
    return create_triplet_sems(v,freespace,unread,mutex);
}


struct triplet_sems* create_triplet_sems(unsigned v, char*freespace_name1,char*unread_name,char*mutex_name) {
    struct triplet_sems* triple_sems = malloc(sizeof(struct triplet_sems));

    // criar os semáforos com o nome correspondente
    triple_sems->freespace = create_semaphore(freespace_name1, v);
    triple_sems->unread = create_semaphore(unread_name, v);
    triple_sems->mutex  = create_semaphore(mutex_name, v);

    return triple_sems;
}


void print_semaphores(struct semaphores* sems) {
    int unread, freespace, mutex;

    sem_getvalue(sems->main_wallet->unread, &unread);
    sem_getvalue(sems->main_wallet->free, &freespace);
    sem_getvalue(sems->main_wallet->mutex, &mutex);

    printf("main_wallet -> unread: %d, freespace: %d, mutex: %d\n", unread, freespace, mutex);

    sem_getvalue(sems->wallet_server->unread, &unread);
    sem_getvalue(sems->wallet_server->free, &freespace);
    sem_getvalue(sems->wallet_server->mutex, &mutex);
    printf("wallet_server -> unread: %d, freespace: %d, mutex: %d\n", unread, freespace, mutex);
    
    sem_getvalue(sems->server_main->unread, &unread);
    sem_getvalue(sems->server_main->free, &freespace);
    sem_getvalue(sems->server_main->mutex, &mutex);
    printf("server_main -> unread: %d, freespace: %d, mutex: %d\n", unread, freespace, mutex);
}

void destroy_triplet_sems(struct triplet_sems* triple, char *name_free, char *name_unread, char *name_mutex) {
    sem_close(triple->freespace);
    sem_unlink(name_free);
    sem_close(triple->unread);
    sem_unlink(name_unread);
    sem_close(triple->mutex);
    sem_unlink(name_mutex);
    free(triple);
}

void destroy_semaphores(struct semaphores* sems) {
    destroy_triplet_sems(sems->main_wallet,STR_SEM_MAIN_WALLET_FREESPACE,STR_SEM_MAIN_WALLET_UNREAD,STR_SEM_MAIN_WALLET_MUTEX);
    destroy_triplet_sems(sems->wallet_server,STR_SEM_WALLET_SERVER_FREESPACE,STR_SEM_WALLET_SERVER_UNREAD,STR_SEM_WALLET_SERVER_MUTEX);
    destroy_triplet_sems(sems->server_main,STR_SEM_SERVER_MAIN_FREESPACE,STR_SEM_SERVER_MAIN_UNREAD,STR_SEM_SERVER_MAIN_MUTEX);
    free(sems);
}

