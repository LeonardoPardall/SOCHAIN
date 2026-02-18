
#ifndef C_SIGNAL_H_GUARD
#define C_SIGNAL_H_GUARD

#include <signal.h>
#include <main.h>


/**
 * Este programa tem de, a cada x-tempo printar todas as transações e o tempo decorrido
 * apenas necessitamos para isso da funcao print_transactions_info
 * Essa struct serve para guardar todas as transações e o tempo que começaram e o tempo que acabararam
 */

extern volatile sig_atomic_t ctrl_c;

void setup_sigint_handler(void);       

struct transaction_info {
    int *finished;            // verifica o estado da transação
    struct timespec *start;   // regista o começo
    struct timespec *end;     // regista o fim
};

typedef struct {
    int time;                        // interrupt
    struct info_container* info;     // para max_txs
} print_thread_args;


// Regista o tempo de comeco de uma tx
void register_start_transaction_time(int id);

// Regista o tempo de termino de uma tx
void register_end_transaction_time(int id);

//Cria a thread 
void start_print_transactions_info(int time,struct info_container* info);

//Aguarda o fim da thread 
void end_print_transaction_time();

// retorna 1 se a transação esta a decorrer e 0 se nao esta
int tx_isrunning(int tx_id, int max_txs);

// Numero de transacoes cujo recibo foi pedido 
int how_many_finished(struct info_container *info);


// Numero de transacoes iniciadas no sistema
int how_many_tx(struct info_container *info);



#endif
