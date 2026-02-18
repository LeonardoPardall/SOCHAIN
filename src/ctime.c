
#include <time.h>
#include "memory.h"


/**
 * Regista o instante em que a transação é assinada pela carteira de origem.
 * Atualiza o campo wallet_change da estrutura timestamps da transação.
 *
 * @param tx Ponteiro para a estrutura da transação a atualizar.
 */
void set_time_wallet(struct transaction* tx) {
    struct timespec current_time;
    clock_gettime(CLOCK_REALTIME, &current_time);
    tx->change_time.wallet_change = current_time;

}

/**
 * Regista o instante em que a transação é processada pelo servidor.
 * Atualiza o campo server_change da estrutura timestamps da transação.
 *
 * @param tx Ponteiro para a estrutura da transação a atualizar.
 */
void set_time_server(struct transaction* tx) {
    struct timespec current_time;
    clock_gettime(CLOCK_REALTIME, &current_time);
    tx->change_time.server_change = current_time;
}
 
/**
 * Regista o instante em que a transação é recebida pela main como comprovativo.
 * Atualiza o campo main_change da estrutura timestamps da transação.
 *
 * @param tx Ponteiro para a estrutura da transação a atualizar.
 */
void set_time_main(struct transaction* tx) {
    struct timespec current_time;
    clock_gettime(CLOCK_REALTIME, &current_time);
    tx->change_time.main_change = current_time;
}

