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
/*
void add_log2(const char *texte) {
    // Récupère la pile d'appels
    #ifdef _WIN32
    void *buffer[10];
    USHORT frames = CaptureStackBackTrace(0, 10, buffer, NULL);

    // Initialiser le débogueur
    HANDLE process = GetCurrentProcess();
    SymInitialize(process, NULL, TRUE);

    // Allouer de la mémoire pour les symboles
    SYMBOL_INFO *symbol = (SYMBOL_INFO *)calloc(sizeof(SYMBOL_INFO) + 256 * sizeof(char), 1);
    symbol->MaxNameLen = 255;
    symbol->SizeOfStruct = sizeof(SYMBOL_INFO);

    // Obtenir les symboles pour chaque frame
    for (USHORT i = 0; i < frames; i++) {
        SymFromAddr(process, (DWORD64)(buffer[i]), 0, symbol);
        printf("%i: %s - 0x%0X\n", frames - i - 1, symbol->Name, symbol->Address);
    }

    // Libérer la mémoire allouée pour les symboles
    free(symbol);
    SymCleanup(process);

    const char *origine = (frames > 1) ? "Captured Stack Trace" : "Unknown";
    #else
    void *buffer[10];
    char **callstack;
    int frames = backtrace(buffer, 10);
    callstack = backtrace_symbols(buffer, frames);
    const char *origine = (frames > 1) ? callstack[1] : "Unknown";
    #endif

    FILE *file = fopen(last_logfile, "a");
    if (file != NULL) {
        printf("%s: %s", origine, texte);
        fprintf(file, "%s: %s", origine, texte);
        fclose(file);
    } 
    else {
        perror("Error opening last_logfile");
    }

    #ifndef _WIN32
    free(callstack);
    #endif
}
*/

//Ajoute une chaine de caractère au dernier fichier de log
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