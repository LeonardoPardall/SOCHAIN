
#ifndef CSTATS_H_GUARD
#define CSTATS_H_GUARD
/*Escreve num ficheiro as estatísticas finais*/

#include "main.h"
struct info_countainer;

int register_stats (struct info_container *info, __pid_t mainPID);
#endif
