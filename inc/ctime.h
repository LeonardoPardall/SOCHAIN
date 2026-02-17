// Grupo 53
// Leonardo Pardal 61836
// Martim Cardoso 61876
// Nuno Nobre 61823

#ifndef CTIME_H_GUARD
#define CTIME_H_GUARD

#include <time.h>

/**
 * O bjetivo deste ficheiro é permitir guardar 
 * o tempo em que cada transação é alterada
 */
struct timestamps {
    struct timespec wallet_change;
    struct timespec server_change;
    struct timespec main_change;
};

struct transaction; // forward declaration

void set_time_wallet(struct transaction *tx);

void set_time_server(struct transaction *tx);

void set_time_main(struct transaction *tx);


#endif

