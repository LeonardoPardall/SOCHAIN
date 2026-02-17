// Grupo 53
// Leonardo Pardal 61836
// Martim Cardoso 61876
// Nuno Nobre 61823

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <memory.h>
#include <wallet.h>

int execute_wallet(int wallet_id, struct info_container* info, struct buffers* buffs) {

    // corre ate a flag terminate ser 1
    while(1) {

        struct transaction tx;                                  // cria a estrutura tx
        
        wallet_receive_transaction(&tx,wallet_id,info,buffs);   // le no buffer uma transação para armazenar em tx   
        
        // verifica se a flag terminate está ativa
        if (*info->terminate == 1) {
            break;
        }

        // verifica se a transação é válida
        if (tx.id == -1) {
            usleep(10000);                                     // aguarda antes de ler novamente
            continue;
        }
        else {
            // processa a transação
            wallet_process_transaction(&tx,wallet_id,info);

            // escreve apenas se tiver assinada
            if (tx.wallet_signature == wallet_id) {
                wallet_send_transaction(&tx,info,buffs);
            }
        }
    }
    // retorna o número de transações assinadas
    return info->wallets_stats[wallet_id];

}


void wallet_receive_transaction(struct transaction* tx, int wallet_id, struct info_container* info, struct buffers* buffs) {
    // verifica se a flag terminate está ativa
    if (*info->terminate == 1) {
        return;
    } 
    // le no buffer uma transação para armazenar em tx
    read_main_wallets_buffer(buffs->buff_main_wallets,wallet_id,info->buffers_size,tx);        
}


void wallet_process_transaction(struct transaction* tx, int wallet_id, struct info_container* info) {

    // verifica se a transação é valida pra ser assinada, se sim processa-a
    if (tx->src_id == wallet_id) {
        tx->wallet_signature = wallet_id;                           // assina a transação
        info->wallets_stats[wallet_id]++;                           // incrementa o contador de transações assinadas

        printf("[Wallet %d] Li a transação %d do buffer e assinei-a\n",wallet_id,tx->id); 
    }

}

void wallet_send_transaction(struct transaction* tx, struct info_container* info, struct buffers* buffs) {
    //escreve tx no buffer de wallets_servers
    write_wallets_servers_buffer(buffs->buff_wallets_servers,info->buffers_size,tx);                     
}