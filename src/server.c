
#include <stdio.h>
#include <server.h>
#include <memory.h>
#include <main.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>


int write_ledger(char *message);


int execute_server(int server_id, struct info_container* info, struct buffers* buffs) {
    // corre ate a flag terminate ser 1
    while(1) {
        struct transaction tx;                                      // cria a estrutura que vai armazenar a transação
        
        server_receive_transaction(&tx,info,buffs);                 // lê no buffer uma transação para armazenar em tx

        // verifica se a flag terminate está ativa
        if (*info->terminate == 1) {           
            break;
        }

        // verifica se a transação é válida
        if (tx.id != -1) {
            server_process_transaction(&tx,server_id,info);          // processa a transação

            // escreve tx apenas se tiver assinado
            if (tx.server_signature == server_id) {
                server_send_transaction(&tx,info,buffs);            // escreve em memory_main
            }
        }
        else {
            usleep(10000);                                          // aguarda antes de ler novamente
        }
    }

    return info->servers_stats[server_id];                          // retorna o número de transações processadas
}


void server_receive_transaction(struct transaction* tx, struct info_container* info, struct buffers* buffs) {

    // verifica se a flag terminate está ativa
    if (*info->terminate == 1) {
        return;
    }
    // lê no buffer uma transação para armazenar em tx
    read_wallets_servers_buffer(buffs->buff_wallets_servers,info->buffers_size,tx);     

}

void server_process_transaction(struct transaction* tx, int server_id, struct info_container* info) {

    char message[256];  // Cria um buffer para armazenar a mensagem da transação

    // verifica se a wallet assinou a transação
    if (tx->src_id != tx->wallet_signature) {
        printf("Carteira não assinou a transação");
        return;
    }
    
    info->balances[tx->dest_id]+=tx->amount;                     // atualiza o saldo da carteira de destino
    info->balances[tx->src_id]-=tx->amount;                      // atualiza o saldo da carteira de origem
    tx->server_signature = server_id;                            // assina a transação                               
    (info->servers_stats[server_id])+=1;                         // incrementa o contador de transações processadas

    // coloca o texto da transação em message
    snprintf(message, sizeof(message),"[Server %d] Ledger <- [tx.id %d, src_id %d, dest_id %d, amount %.2f]\n",server_id,tx->id, tx->src_id, tx->dest_id, tx->amount);

    // escreve no stdout
    printf("[Server %d] Li a transação %d do buffer e esta foi processada corretamente!\n",server_id,tx->id);
    printf("[Server %d] Ledger <- [tx.id %d, src_id %d, dest_id %d, amount %.2f]\n",server_id,tx->id,tx->src_id,tx->dest_id,tx->amount);

     // escreve no ledger
    write_ledger(message);                                      

}

void server_send_transaction(struct transaction* tx, struct info_container* info, struct buffers* buffs) {
    // escreve a transação no buffer de servers_main
    write_servers_main_buffer(buffs->buff_servers_main, info->buffers_size,tx);

}


int write_ledger(char *message) {
    
    // Abre o arquivo ledger.txt ou cria se não existir
    FILE *file = fopen("ledger.txt", "a");                      // "a" é para append
 
    // Verifica se o arquivo foi aberto
    if (file == NULL) {
        printf("Erro ao abrir o ledger\n");
        return 1;  
    }
  
    fprintf(file, "%s", message);                           // escreve a mensagem no file

    fclose(file);                                           // fecha o file
}

