
#include <csignal.h>
#include <main.h>
#include <memory.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

struct transaction_info txi;
pthread_t thread;


volatile sig_atomic_t ctrl_c = 0;

/**
 * Handler para o sinal SIGINT (Ctrl+C).
 * Define a flag ctrl_c para 1, permitindo que o programa termine de forma controlada.
 */
void handle_sigint(int sig) {
    ctrl_c = 1;
}

/**
 * Configura o handler para o sinal SIGINT.
 * Permite que o programa intercepte Ctrl+C e execute ações de terminação segura.
 */
void setup_sigint_handler(void) {
    struct sigaction sa;
    sa.sa_handler = handle_sigint;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    if (sigaction(SIGINT, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }
}

/**
 * Calcula a diferença de tempo (em segundos) entre dois instantes.
 */
double calculate_time_difference(struct timespec start, struct timespec end);

void initialize_transaction_info(int max_txs) {
  // Aloca memória para os arrays de controlo das transações
  txi.finished = allocate_dynamic_memory(max_txs * sizeof(int));
  txi.start = allocate_dynamic_memory(max_txs * sizeof(struct timespec));
  txi.end = allocate_dynamic_memory(max_txs * sizeof(struct timespec));
  if (txi.finished == NULL || txi.start == NULL || txi.end == NULL) {
    printf("Erro ao alocar memória!\n");
    exit(1);
  }
  // Inicializa o estado das transações como não iniciadas
  for (int i = 0; i < max_txs; i++) {
    txi.finished[i] = -1;
  }
}

/**
 * Verifica se uma transação está a decorrer.
 * @param tx_id ID da transação
 * @param max_txs Número máximo de transações
 * @return 1 se a transação está a decorrer, 0 caso contrário
 */
int tx_isrunning(int tx_id, int max_txs) {
  if (tx_id < max_txs && txi.finished[tx_id] == 0) {
    return 1;
  }
  return 0;
}

/**
 * Liberta a memória alocada para o controlo das transações.
 */
void break_transaction_info() {

  free(txi.finished);
  free(txi.start);
  free(txi.end);
}

/**
 * Imprime o tempo decorrido de uma transação.
 * Se a transação terminou, imprime o tempo total.
 * Se está a decorrer, imprime o tempo até ao momento atual.
 * Se não foi iniciada, não imprime nada.
 * @param id ID da transação
 */
void print_time_from_transaction(int id) {

  int finished = txi.finished[id];
  struct timespec current_time;
  double time;
  if (finished == -1) {

    return;
  } else if (finished == 0) {
    clock_gettime(CLOCK_REALTIME, &current_time);
    time = calculate_time_difference(txi.start[id], current_time);

  } else {

    time = calculate_time_difference(txi.start[id], txi.end[id]);
  }
  printf("\n%d %.3f\n", id, time);
}

/**
 * Calcula a diferença de tempo (em segundos) entre dois instantes.
 */
double calculate_time_difference(struct timespec start, struct timespec end) {
  double start_seconds = start.tv_sec + start.tv_nsec / 1000000000.0;
  double end_seconds = end.tv_sec + end.tv_nsec / 1000000000.0;
  return end_seconds - start_seconds;
}

/**
 * Regista o instante de início de uma transação.
 * @param id ID da transação
 */
void register_start_transaction_time(int id) {
  clock_gettime(CLOCK_REALTIME, &txi.start[id]);
  txi.finished[id] = 0; // em progresso
}

/**
 * Regista o instante de fim de uma transação.
 * @param id ID da transação
 */
void register_end_transaction_time(int id) {
  clock_gettime(CLOCK_REALTIME, &txi.end[id]);
  txi.finished[id] = 1; // Terminou
}

/**
 * Função executada pela thread que imprime periodicamente o tempo das transações.
 * @param arg Argumentos para a thread (tempo de espera e info_container)
 */
void *print_transactions_info_thread(void *arg) {
  print_thread_args *args = (print_thread_args *)arg;
  int time = args->time;
  struct info_container *info = args->info;

  if (time > 0) {
    while (*info->terminate != 1) {
      for (int i = 0; i < info->max_txs; i++) {
        if (txi.finished[i] == -1) {
          break;
        }
        print_time_from_transaction(i);
      }
      for (int j = 0; j < 10; j++) {
          sleep(time/10);
          if (*info->terminate == 1) {
            break;
          }
        }
    }          
  }
  break_transaction_info();
  free(args);
  pthread_exit(NULL);
}

/**
 * Inicia a thread responsável por imprimir periodicamente o estado das transações.
 * @param time Intervalo de tempo entre impressões (segundos)
 * @param info Ponteiro para a estrutura info_container
 */
void start_print_transactions_info(int time, struct info_container *info) {
  if (time != 0) {
    initialize_transaction_info(info->max_txs);
    print_thread_args *args = malloc(sizeof(print_thread_args));
    args->time = time;
    args->info = info;
    pthread_create(&thread, NULL, print_transactions_info_thread, args);
  }
}

/**
 * Aguarda o término da thread de impressão das transações.
 */
void end_print_transaction_time() {
  pthread_join(thread,NULL);
} 

/**
 * Conta quantas transações já terminaram (recibo pedido).
 * @param info Ponteiro para a estrutura info_container
 * @return Número de transações terminadas
 */
int how_many_finished(struct info_container *info) {
  int counter = 0;
  for (int i = 0; i < info->max_txs; i++) {
    if (txi.finished[i] == 1) {
      counter++;
    }
  }
  return counter;
}

/**
 * Conta quantas transações foram iniciadas no sistema.
 * @param info Ponteiro para a estrutura info_container
 * @return Número de transações iniciadas
 */
int how_many_tx(struct info_container *info) {
  int counter = 0;
  for (int i = 0; i < info->max_txs; i++) {
    if (txi.finished[i] != -1) {
      counter++;
    }
  }
  return counter;
}
