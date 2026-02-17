// Grupo 53
// Leonardo Pardal 61836
// Martim Cardoso 61876
// Nuno Nobre 61823

#include <stdio.h>
#include <sys/wait.h>  
#include <stdlib.h>     
#include <memory.h>
#include <unistd.h>
#include <server.h>
#include <wallet.h>
#include <main.h>



int launch_wallet(int wallet_id, struct info_container* info, struct buffers* buffs) {
    // cria processo filho
    int pid = fork();      

    // se o processo for o filho entao executa a função
    if (pid == 0) {                                 
        exit(execute_wallet(wallet_id,info,buffs));         // começa a execução da carteira
    }
    else {
        return pid;                                         // o pai retorna o pid do filho
    }
    
}


int launch_server(int server_id, struct info_container* info, struct buffers* buffs) {
    // cria processo filho
    int pid = fork();

    // se o processo for o filho entao executa a função
    if (pid == 0) {
        exit(execute_server(server_id,info,buffs));         // começa a execução do servidor
    }
    else {
        return pid;                                         // o pai retorna o pid do filho
    }
    
}

int wait_process(int process_id) {
    int status;
    // espera o processo terminar e guarda informação no status
    if (waitpid(process_id, &status, 0) == -1) {                         
        printf("Ocorreu um erro ao esperar");
        return -1;
    }
    // verifica se o processo terminou normalmente
    if (WIFEXITED(status)) {                   
        return WEXITSTATUS(status);                     // retorna o valor de saída do processo
    } 
    else {
        return -1;                                      // retorna -1 se não terminou normalmente
    }
    
}


