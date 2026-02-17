// Grupo 53
// Leonardo Pardal 61836
// Martim Cardoso 61876
// Nuno Nobre 61823

#include <main.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <process.h>
#include <server.h>
#include <wallet.h>
#include <unistd.h>



void main_args(int argc, char *argv[], struct info_container *info) {

    // verifica se o número de argumentos é correto
    if (argc != 6) {
        printf("[Main] Uso: ./SOchain init_balance n_wallets n_servers buff_size max_txs\n");
        printf("[Main] Exemplo: ./SOchain 100.0 2 1 5 5\n");
        return;
    }

    int c = 0;                                                          // incializa o arg counter

    while (++c < argc) {

        int arg = atoi(argv[c]);                                        // converte o argumento para inteiro

        // verifica se o argumento é valido e também se a conversão funcionou
        if (arg > 0) {
            if (c == 1) {
                info->init_balance = atof(argv[c]);                     // balance é float não usa 'int arg'
            }
            else if (c == 2) {
                info->n_wallets = arg;         
            }
            else if (c == 3) {
                info->n_servers = arg;
            }

            else if (c == 4) {
                info->buffers_size = arg;
            }

            else if (c == 5) {
                info->max_txs = arg;
            }
        }
        else {
            printf("[Main] Parâmetros incorretos! Exemplo de uso: ./SOchain 100.0 2 1 5 5\n");
            return;
        }
    }
    FILE *file = fopen("ledger.txt", "w");                         // cria ou substitui o file ledger.txt

    // verifica se o file aberto com sucesso
    if (file == NULL) {
        printf("Erro ao abrir o arquivo");  
    }

    fprintf(file, "Aqui são armazenadas as transações!\n");        

    fclose(file);                                                   // fecha o file

    printf("[Main] Parâmetros corretos!\n");
}




void create_dynamic_memory_structs(struct info_container* info, struct buffers* buffs) {

    // criar as extruturas que armazenam os pids
    info->wallets_pids = (int*) allocate_dynamic_memory(info->n_wallets * sizeof(int));
    info->servers_pids = (int*) allocate_dynamic_memory(info->n_servers * sizeof(int));

    // criar as estruturas dos buffers
    buffs->buff_main_wallets = (struct ra_buffer*) allocate_dynamic_memory(sizeof(struct ra_buffer));
    buffs->buff_wallets_servers = (struct circ_buffer*) allocate_dynamic_memory(sizeof(struct circ_buffer));
    buffs->buff_servers_main = (struct ra_buffer*) allocate_dynamic_memory(sizeof(struct ra_buffer));


}


void create_shared_memory_structs(struct info_container* info, struct buffers* buffs) {

    // cria as estruturas de memória partilhada com base nos nomes em memory.h
    buffs->buff_main_wallets->ptrs = (int*) create_shared_memory(ID_SHM_MAIN_WALLETS_PTR, sizeof(int)* info->buffers_size);
    buffs->buff_wallets_servers->ptrs = (struct pointers*) create_shared_memory(ID_SHM_WALLETS_SERVERS_PTR, sizeof(struct pointers));
    buffs->buff_servers_main->ptrs = (int*) create_shared_memory(ID_SHM_SERVERS_MAIN_PTR, sizeof(int)* info->buffers_size);
    buffs->buff_main_wallets->buffer = (struct transaction*) create_shared_memory(ID_SHM_MAIN_WALLETS_BUFFER, sizeof(struct transaction) * info->buffers_size);
    buffs->buff_wallets_servers->buffer = (struct transaction*) create_shared_memory(ID_SHM_WALLETS_SERVERS_BUFFER, sizeof(struct transaction) * info->buffers_size);
    buffs->buff_servers_main->buffer = (struct transaction*) create_shared_memory(ID_SHM_SERVERS_MAIN_BUFFER, sizeof(struct transaction) * info->buffers_size);
    info->wallets_stats = (int*) create_shared_memory(ID_SHM_WALLETS_STATS, sizeof(int) * info->n_wallets);
    info->servers_stats = (int*) create_shared_memory(ID_SHM_SERVERS_STATS, sizeof(int) * info->n_servers);
    info->balances = (float*) create_shared_memory(ID_SHM_BALANCES, sizeof(float) * info->n_wallets);
    info->terminate = (int*) create_shared_memory(ID_SHM_TERMINATE, sizeof(int));

    // coloca a flag terminate a 0
    *info->terminate = 0;
}

void destroy_dynamic_memory_structs(struct info_container* info, struct buffers* buffs) {

    // liberta toda a memoria dinamica alocada
    deallocate_dynamic_memory(info->wallets_pids);
    deallocate_dynamic_memory(info->servers_pids);
    deallocate_dynamic_memory(buffs->buff_main_wallets);
    deallocate_dynamic_memory(buffs->buff_wallets_servers);
    deallocate_dynamic_memory(buffs->buff_servers_main);

}

void destroy_shared_memory_structs(struct info_container* info, struct buffers* buffs) {

    // liberta toda a memoria partilhada alocada
    destroy_shared_memory(ID_SHM_BALANCES, info->balances, sizeof(float) * info->n_wallets);
    destroy_shared_memory(ID_SHM_TERMINATE, info->terminate, sizeof(int));
    destroy_shared_memory(ID_SHM_MAIN_WALLETS_PTR, buffs->buff_main_wallets->ptrs, sizeof(int) * info->buffers_size);
    destroy_shared_memory(ID_SHM_WALLETS_SERVERS_PTR, buffs->buff_wallets_servers->ptrs, sizeof(struct pointers));
    destroy_shared_memory(ID_SHM_SERVERS_MAIN_PTR, buffs->buff_servers_main->ptrs, sizeof(int) * info->buffers_size);
    destroy_shared_memory(ID_SHM_MAIN_WALLETS_BUFFER, buffs->buff_main_wallets->buffer, sizeof(struct transaction) * info->buffers_size);
    destroy_shared_memory(ID_SHM_WALLETS_SERVERS_BUFFER, buffs->buff_wallets_servers->buffer, sizeof(struct transaction) * info->buffers_size);
    destroy_shared_memory(ID_SHM_SERVERS_MAIN_BUFFER, buffs->buff_servers_main->buffer, sizeof(struct transaction) * info->buffers_size);
    destroy_shared_memory(ID_SHM_WALLETS_STATS, info->wallets_stats, sizeof(int) * info->n_wallets);
    destroy_shared_memory(ID_SHM_SERVERS_STATS, info->servers_stats, sizeof(int) * info->n_servers);
}


void create_processes(struct info_container* info, struct buffers* buffs) {

    // incializa as carteiras 
    for (int i = 0; i < info->n_wallets; i++) {
        info->balances[i] = info->init_balance;                   // coloca o balance em cada carteira
        info->wallets_pids[i] = launch_wallet(i, info, buffs);    // lança a carteira
    }

    // incializa os servidores
    for (int i = 0; i < info->n_servers; i++) {
        info->servers_pids[i] = launch_server(i,info,buffs);     // lança o servidor
    }
}


void user_interaction(struct info_container* info, struct buffers* buffs) {

    int tx_counter = 0;                                 // contador de transações criadas
    char input[10];                                     // buffer para guardar o input do utilizador
    while(1) {

        printf("\n[Main] Introduzir operação: ");
        scanf("%s", input);                             // lê a primeira palavra do input

        // verifica todos os comandos disponíveis
        // executa a função correspondente

        if (strcmp(input,"bal") == 0) {
            print_balance(info);
            
        }

        else if (strcmp(input,"trx") == 0) {
            create_transaction(&tx_counter,info,buffs);
        }

        else if (strcmp(input,"rcp") == 0) {
            receive_receipt(info,buffs);
        }

        else if (strcmp(input,"stat") == 0) {
            print_stat(tx_counter,info);
        }

        else if (strcmp(input,"help") == 0) {
            help();
        }

        else if (strcmp(input,"end") == 0) {
            end_execution(info,buffs);
            break;
        }
        
        else {
            printf("\n[Main] Operação não reconhecida, insira 'help' para assistência.\n");
            
        }

        usleep(300000);                     // espera 0.3 segundos antes de continuar
    }
}  


void end_execution(struct info_container* info, struct buffers* buffs) {

    printf("[Main] A encerrar a execução do SOchain! As estatísticas da execução são: \n");

    *info->terminate = 1;                                       // coloca a flag a 1 para terminar a execução

    // espera os processos das wallets e servers terminarem
    for (int i = 0; i < info->n_wallets; i++) {
        wait_process(info->wallets_pids[i]);
    }
    for (int i = 0; i < info->n_servers; i++) {
        wait_process(info->servers_pids[i]);
    }

    write_final_statistics(info);                               // escreve as estatísticas finais
}

void write_final_statistics(struct info_container* info) {

    // escreve o numero de transações assinadas por cada carteira e os SOT's finais
    for (int i = 0; i < info->n_wallets; i++) {
        printf("[Main] A carteira %d assinou %d transações e terminou com %.2f SOT!\n",i,info->wallets_stats[i],info->balances[i]);
    } 

    // escreve o numero de transações assinadas por cada servidor
    for (int i = 0; i < info->n_servers; i++) {
        printf("[Main] O servidor %d assinou %d transações!\n",i,info->servers_stats[i]);
    }  
}
void print_balance(struct info_container* info) {
    int id;                                             // inicializa a variável que vai guardar o id da carteira
    int valid = scanf("%d",&id);                        // lê o id da carteira

    // verifica se a leitura deu erro
    if (valid < 1) {    
        printf("\n[Main] Operação não reconhecida, insira 'help' para assistência.\n");
        return;
    }
    // verifica se o id é válido
    if (id < 0 || id >= info->n_wallets) {
        printf("[Main] Não foi possível verificar o saldo da carteira %d\n",id);
        return;
    }
    printf("[Main] O saldo da carteira %d é de %.2f SOT atualmente.\n",id,info->balances[id]);    
}

void create_transaction(int* tx_counter, struct info_container* info, struct buffers* buffs) {
    // coloca o counter em id pra evitar problemas de referencia 
    // inicializa as variáveis da transação
    int id = *tx_counter;                                                                      
    int src_id, dest_id;                                                                       
    float amount;                                                                              
    int valid = scanf("%d %d %f",&src_id, &dest_id, &amount);                              // lê os valores do stdin

    // verifica se a leitura funcionou corretamente
    if (valid < 3) {
        printf("\n[Main] Operação não reconhecida, insira 'help' para assistência.\n");
        return;
    }

    // verifica se existem transações disponíveis para criar
    // verifica apenas aqui pra evitar sair da funcao com o stdin por ler
    if (*tx_counter == info->max_txs) {
        printf("[Main] O número máximo de transações foi alcançado!\n");
        return;
    }


    // Verifica a validade dos argumentos lidos 

    if (src_id < 0 || src_id >= info->n_wallets) {
        printf("[Main] A carteira de origem não existe!\n");
        return;
    }
    if (dest_id < 0 || dest_id >= info->n_wallets) {
        printf("[Main] A carteira de destino não existe!\n");
        return;
    }
    if (amount <= 0) {
        printf("[Main] O valor a transferir deve ser positivo!\n");
        return;
    }
    if (info->balances[src_id] < amount) {
        printf("[Main] A carteira de origem não tem saldo suficiente!\n");
        return;
    }

    printf("[Main] A transação %d foi criada para transferir %.2f SOT da carteira %d para a carteira %d!\n",*tx_counter,amount,src_id,dest_id);

    // inicializa a estrutura e coloca os valores correspondentes
    struct transaction tx;
    tx.id = id;
    tx.src_id = src_id;
    tx.dest_id = dest_id;
    tx.amount = amount;

    write_main_wallets_buffer(buffs->buff_main_wallets,info->buffers_size,&tx);     // escreve no buffer de wallets_main
    (*tx_counter)++;                                                                // incrementa o contador de transações 

}

void receive_receipt(struct info_container* info, struct buffers* buffs) {

    // inicializa id e tx
    int tx_id;
    struct transaction tx;

    int valid = scanf("%d",&tx_id);                                                 // lê o id da transação

    // verifica se a leitura funcionou corretamente
    if (valid < 1) {
        printf("\n[Main] Operação não reconhecida, insira 'help' para assistência.\n");
        return;
    }
    read_servers_main_buffer(buffs->buff_servers_main,tx_id,info->buffers_size,&tx);      // lê a transação com id tx_id do buffer memory_main e guarda em tx

    // verifica se a transação foi encontrada
    if (tx.id == -1) {
        printf("[Main] Não foi possível encontrar o recibo da transação %d\n",tx_id);
        return;
    }

    printf("[Main] O comprovativo da transação %d foi obtido.\n",tx_id);
    printf("[Main] O comprovativo da transação %d contém src_id %d, dest_id %d, amount %.2f e foi assinado pela carteira %d e servidor %d.\n",tx_id,tx.src_id,tx.dest_id,tx.amount,tx.wallet_signature,tx.server_signature);
}

// printa todas as informações do sistema
void print_stat(int tx_counter, struct info_container* info) {

    printf("- Configuração inicial:\n");
    printf("\tPropriedade\tValor\n");
    printf("\tinit_balance\t%.2f\n",info->init_balance);
    printf("\tn_wallets\t%d\n",info->n_wallets);
    printf("\tn_servers\t%d\n",info->n_servers);
    printf("\tbuffers_size\t%d\n",info->buffers_size);
    printf("\tmax_txs\t\t%d\n",info->max_txs);

    printf("- Variáveis atuais:\n");
    printf("\tterminate\t%d\n",*info->terminate);
    printf("\ttx_counter\t%d\n",tx_counter);

    printf("- Informações sobre as carteiras:\n");
    printf("\tCarteira\tPID\t\tSaldo\t\tTransações assinadas\n");
    for (int i = 0; i < info->n_wallets; i++) {
        printf("\t%d\t\t%d\t\t%.2f\t%d\n",i,info->wallets_pids[i],info->balances[i],info->wallets_stats[i]);
    }

    printf("- Informações sobre os servidores:\n");
    printf("\tServidor\tPID\t\tTransações processadas\n");
    for (int i = 0; i < info->n_servers; i++) {
        printf("\t%d\t\t%d\t\t%d\n",i,info->servers_pids[i],info->servers_stats[i]);
    }
}

//printa as operações disponíveis e a sua descrição
void help() {
    printf("help\n");
    printf("[Main]  Operações disponíveis:\n");
    printf("[Main]  bal id - consultar o saldo da carteira identificada por id.\n");
    printf("[Main]  trx src_id dest_id amount - criar uma nova transação.\n");
    printf("[Main]  rcp id - obter o comprovativo da transação de número id.\n");
    printf("[Main]  stat - Apresenta o estado atual do sistema\n");
    printf("[Main]  help - imprime a informação sobre as operações disponíveis.\n");
    printf("[Main]  end - termina a execução do SOchain.\n");
}


int main(int argc, char *argv[]) {

    // inicializa as estruturas que vão guardar a informação e os buffers
    struct info_container* info = allocate_dynamic_memory(sizeof(struct info_container));
    struct buffers* buffs = allocate_dynamic_memory(sizeof(struct buffers));
    
    // lê os argumentos da linha de comandos e cria o ledger
    main_args(argc, argv, info);

    // cria as estruturas de memória dinâmica e partilhada
    create_dynamic_memory_structs(info, buffs);
    create_shared_memory_structs(info, buffs);

    // inicializa as wallets e os server
    create_processes(info, buffs);

    // Começa a ler os comandos do utilizador
    user_interaction(info, buffs);

    // destrói a memória partilhada e dinâmica
    destroy_shared_memory_structs(info, buffs);
    destroy_dynamic_memory_structs(info, buffs);

    // termina
    return 0;
}

