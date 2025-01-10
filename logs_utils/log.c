#include <time.h>
#include <stdio.h> 

// Variable globale pour stocker le nom du fichier de log
char last_logfile[100];

//Créé un fichier de log portant comme nom, la date exacte de sa création
void create_log_file() {
    time_t now = time(NULL); // Récupère l'heure actuelle
    struct tm *t = localtime(&now); // Convertit l'heure en une structure tm

    strftime(last_logfile, sizeof(last_logfile)-1, "log-%d%m%y%H%M%S.txt", t); // Crée un nom de fichier avec la date et l'heure

    FILE *file = fopen(last_logfile, "w");
    if (file != NULL) {
        fprintf(file, "Log file created at %s", asctime(t));
        fclose(file);
    } 
    else {
        perror("Error creating log file");
    }
}


//Ajoute une chaine de caractère au dernnier fichier de log
void add_log(const char *origine, const char *texte) {
    
    FILE *file = fopen(last_logfile, "a");
    if (file != NULL) {
        printf("%s: %s", origine, texte);
        fprintf(file, "%s: %s", origine, texte);
        fclose(file);
    } 
    else {
        perror("Error opening last_logfile");
    }
}