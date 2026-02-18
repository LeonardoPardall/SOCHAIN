
#include "csettings.h"
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>

char *fileName_log = NULL; // Nome do ficheiro de log
int flag_log = 0;          // Flag para verificar se o nome do ficheiro já foi obtido


/**
 * Escreve uma mensagem no ficheiro de log.
 * Se for a primeira chamada, obtém o nome do ficheiro de log através da função get_log_file_name().
 * Abre o ficheiro e escreve a mensagem recebida.
 * Retorna 0 em caso de sucesso, 1 em caso de erro ao abrir o ficheiro.
 */
int write_ledger(char *message) {
    if (flag_log == 0) {
        flag_log = 1;
        fileName_log = get_log_file_name();
    }

    // Abre o arquivo ledger.txt em modo append ("a"), ou cria-o se não existir
    FILE *file = fopen(fileName_log, "a");                      
 
    // Verifica se o arquivo foi aberto
    if (file == NULL) {
        printf("Erro ao abrir o ledger\n");
        return 1;  
    }
  
    // Escreve a mensagem no ficheiro de log
    fprintf(file, "%s", message);                           

    // Fecha o ficheiro
    fclose(file);                                           
    return 0;
}


/**
 * Regista uma operação no ficheiro de log.
 * Recebe uma string com a operação a ser registada.
 * Adiciona um timestamp à operação e chama write_ledger para escrever no ficheiro.
 */
void register_operation(char *operation) {
    char buffer_operation[100]; 
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    struct tm *local = localtime(&ts.tv_sec);  
    int ms = ts.tv_nsec / 1000000;  


    snprintf(buffer_operation,sizeof(buffer_operation),"%04d%02d%02d %02d:%02d:%02d.%03d %s\n",
           local->tm_year + 1900,
           local->tm_mon + 1,
           local->tm_mday,
           local->tm_hour,
           local->tm_min,
           local->tm_sec,
           ms, operation);

    write_ledger(buffer_operation);       
}


