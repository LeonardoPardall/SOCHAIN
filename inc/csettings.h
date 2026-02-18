
#ifndef CSETTINGS_H_GUARD
#define CSETTINGS_H_GUARD

/*Este file lê um txt e guarda o nome do ficheiro de escrita 
e do ficheiro de stats, além de guardar o periodo para o file csignal*/

struct settings {
    char log_filename[100];
    char statistics_filename[100];
    int period;
};

int load_settings(char *filepath);

char* get_log_file_name();

char *get_stats_file_name();

int get_period();


#endif
