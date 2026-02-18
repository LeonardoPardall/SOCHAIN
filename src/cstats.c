
#include "../inc/cstats.h"
#include "../inc/csettings.h"
#include "../inc/csignal.h"
#include "../inc/main.h"
#include <fcntl.h>
#include <stdio.h> 
#include <string.h>


/**
 * Escreve uma mensagem no ficheiro de estatísticas.
 * O ficheiro é obtido através da função get_stats_file_name().
 * Abre o ficheiro em modo append ("a") e escreve a mensagem recebida.
 * 
 * @param message Mensagem a ser escrita no ficheiro de estatísticas.
 * @return 0 em caso de sucesso, 1 em caso de erro ao abrir o ficheiro.
 */
int write_stat(char *message) {
  char *fileName = get_stats_file_name();
  // Abre o ficheiro de stats ou cria se não existir
  FILE *file = fopen(fileName, "a");

  // Verifica se o arquivo foi aberto
  if (file == NULL) {
    printf("Erro ao abrir o ficheiro\n");
    return 1;
  }

  fprintf(file, "%s", message); // Escreve a mensagem no file

  fclose(file); // Fecha o file
  return 0;
}

/**
 * Escreve no buffer os PIDs dos processos, separados por vírgula.
 * 
 * @param buffer Buffer onde os PIDs serão escritos.
 * @param num Número de PIDs.
 * @param pids Array de PIDs.
 * @return Número de caracteres escritos no buffer.
 */
int printPIDs(char *buffer, int num, int *pids) {
  int written = 0;
  char c;
  for (int i = 0; i < num; i++) {
    c = (i == num - 1) ? ']' : ',';
    written += sprintf(buffer + written, "%d%c", pids[i], c);
  }
  written += sprintf(buffer + written, "\n");
  return written;
}

/**
 * Escreve no buffer o número de transações assinadas por cada carteira.
 * 
 * @param buffer Buffer onde a informação será escrita.
 * @param info Ponteiro para a estrutura info_container.
 * @return Número de caracteres escritos no buffer.
 */
int print_wallets_info(char *buffer, struct info_container *info) {
  int written = 0;
  for (int i = 0; i < info->n_wallets; i++) {
    written +=
        sprintf(buffer + written, "Wallet %d# signed %d transaction(s)!\n", i,
                info->wallets_stats[i]);
  }
  return written;
}

/**
 * Escreve no buffer o número de transações processadas por cada servidor.
 * 
 * @param buffer Buffer onde a informação será escrita.
 * @param info Ponteiro para a estrutura info_container.
 * @return Número de caracteres escritos no buffer.
 */
int print_servers_info(char *buffer, struct info_container *info) {
  int written = 0;
  for (int i = 0; i < info->n_servers; i++) {
    written +=
        sprintf(buffer + written, "Server %d# processed %d transaction(s)!\n",
                i, info->servers_stats[i]);
  }
  return written;
}

/**
 * Escreve no buffer os saldos finais de todas as carteiras.
 * 
 * @param buffer Buffer onde os saldos serão escritos.
 * @param info Ponteiro para a estrutura info_container.
 * @return Número de caracteres escritos no buffer.
 */
int print_final_balance(char *buffer, struct info_container *info) {
  int written = 0;
  char c;
  written += sprintf(buffer + written, "Final Balances [");
  for (int i = 0; i < info->n_wallets; i++) {
    c = (i == info->n_wallets - 1) ? ']' : ',';
    written += sprintf(buffer + written, "%.2f%c", info->balances[i], c);
  }
  written += sprintf(buffer + written, " SOT\n");
  return written;
}

/**
 * Regista as estatísticas finais do sistema no ficheiro de estatísticas.
 * Compila informações sobre PIDs, transações, carteiras, servidores e saldos finais.
 * 
 * @param info Ponteiro para a estrutura info_container.
 * @param mainPID PID do processo principal.
 * @return 0 em caso de sucesso, 1 em caso de erro ao escrever no ficheiro.
 */
int register_stats(struct info_container *info, __pid_t mainPID) {
  char buffer_stats[10000];
  int location = 0; // onde escrever no buffer
  location += sprintf(buffer_stats, "Process Statistics: \nMain has PID[%d]\n",
                      mainPID);
  location += sprintf(buffer_stats + location, "There were %d Wallets, PIDs [",
                      info->n_wallets);
  location +=
      printPIDs(buffer_stats + location, info->n_wallets, info->wallets_pids);
  location += sprintf(buffer_stats + location, "There were %d Servers, PIDs [",
                      info->n_servers);
  location +=
      printPIDs(buffer_stats + location, info->n_servers, info->servers_pids);

  location += sprintf(buffer_stats + location, "Main received %d transaction(s)!\n",
                      how_many_tx(info));
  location += print_wallets_info(buffer_stats + location, info);
  location += print_servers_info(buffer_stats + location, info);
  location +=
      sprintf(buffer_stats + location, "Main read %d receipts.", how_many_finished(info));
  location += print_final_balance(buffer_stats + location, info);

  return write_stat(buffer_stats);
}

/**
 * Escreve uma string de estatísticas no ficheiro de estatísticas.
 * 
 * @param stat String com as estatísticas a registar.
 */
void register_stats_main(char *stat) { write_stat(stat); }
