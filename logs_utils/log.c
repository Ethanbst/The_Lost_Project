#include <time.h>
#include <stdio.h> 

//#include <windows.h>
//#include <dbghelp.h>

// Variable globale pour stocker le nom du fichier de log
char last_logfile[100];

//Créé un fichier de log portant comme nom, la date exacte de sa création
void create_log_file() {
    time_t now = time(NULL); // Récupère l'heure actuelle
    struct tm *t = localtime(&now); // Convertit l'heure en une structure tm

    strftime(last_logfile, sizeof(last_logfile)-1, "log-%d%m%y%H%M%S.txt", t); // Crée un nom de fichier avec la date et l'heure

    FILE *file = fopen(last_logfile, "w");
    if (file != NULL) {
        fprintf(file, "Log file created at %s\n", asctime(t));
        fclose(file);
    }
    else {
        perror("Error creating log file");
    }
}

//Ajoute la chaine au journal d'évènements et l'affiche également dans la console
void add_log(const char *origine, const char *texte) {
    
    FILE *file = fopen(last_logfile, "a");
    if (file != NULL) {
        printf("[%s] %s\n", origine, texte);
        fprintf(file, "[%s] %s\n", origine, texte);
        fclose(file);
    } 
    else {
        perror("Error opening last_logfile");
    }
}

//Ajoute la chaine au journal d'évènements et l'affiche également dans la console en bleu
void add_log_info(const char *origine, const char *texte) {
    
    FILE *file = fopen(last_logfile, "a");
    if (file != NULL) {
        printf("\033[0;36m[%s] %s\033[0m\n", origine, texte);
        fprintf(file, "[%s] %s\n", origine, texte);
        fclose(file);
    } 
    else {
        perror("Error opening last_logfile");
    }
}

//Ajoute la chaine au journal d'évènements et l'affiche également dans la console en rouge signalant une erreur
void add_log_error(const char *origine, const char *texte) {
    
    FILE *file = fopen(last_logfile, "a");
    if (file != NULL) {
        printf("\033[0;31m[%s] %s\033[0m\n", origine, texte);
        fprintf(file, "[%s] %s\n", origine, texte);
        fclose(file);
    } 
    else {
        perror("Error opening last_logfile");
    }
}