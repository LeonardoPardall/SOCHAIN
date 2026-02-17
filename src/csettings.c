// Grupo 53
// Leonardo Pardal 61836
// Martim Cardoso 61876
// Nuno Nobre 61823

#include "csettings.h"
#include <stdio.h>
#include <string.h>


struct settings config; 


/**
 * Carrega as configurações a partir de um ficheiro de texto.
 * 
 * @param filepath Caminho para o ficheiro de configurações.
 * @return 0 em caso de sucesso, -1 em caso de erro.
 */
int load_settings(char *filepath) {
    FILE *file = fopen(filepath, "r");
    if (!file) {
        perror("Erro ao abrir settings.txt");
        return -1;
    }

    // Lê os nomes dos ficheiros de log e estatísticas
    if (fgets(config.log_filename, sizeof(config.log_filename), file) == NULL ||
        fgets(config.statistics_filename, sizeof(config.statistics_filename), file) == NULL) {
        fclose(file);
        return -1;
    }

    // Remove o carácter '\n' do final das strings
    config.log_filename[strcspn(config.log_filename, "\r\n")] = 0;
    config.statistics_filename[strcspn(config.statistics_filename, "\r\n")] = 0;

    if (fscanf(file, "%d", &config.period) != 1) {
        fclose(file);
        return -1;
    }

    fclose(file);

    return 0;
}


/**
 * Obtém o nome do ficheiro de log a partir das configurações.
 * Cria o ficheiro de log (ou substitui se já existir) e escreve um cabeçalho.
 * 
 * @return Ponteiro para a string com o nome do ficheiro de log.
 */
char *get_log_file_name() {

    FILE *file = fopen(config.log_filename, "w");
    if (file == NULL) {
        perror("Erro ao abrir log_file");
        return NULL;
    }
    fprintf(file, "Ficheiro de Registo de operações:\n");
    fclose(file);
    return config.log_filename;
}

/**
 * Obtém o nome do ficheiro de estatísticas a partir das configurações.
 * Cria o ficheiro de estatísticas (ou substitui se já existir) e escreve um cabeçalho.
 * 
 * @return Ponteiro para a string com o nome do ficheiro de estatísticas.
 */
char *get_stats_file_name() {
    FILE *file = fopen(config.statistics_filename, "w");
    if (file == NULL) {
        perror("Erro ao abrir stat_file");
        return NULL;
    }
    fprintf(file, "Ficheiro de Estatísticas:\n");
    fclose(file);
    return config.statistics_filename;
}

int get_period() {
    return config.period;
}
