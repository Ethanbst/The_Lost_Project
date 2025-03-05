#include <stdio.h>
#include <stdlib.h>
#include <C:/cJSON-master/cJSON.h>
#include "../logs_utils/log.h"
#include <string.h>


//Modifie le fichier save.json en y ajoutant le nom du denrier monde joué
void save_progress(char* world_name) {
    cJSON *root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "world", world_name);

    char *json_string = cJSON_Print(root);
    if (json_string == NULL) {
        add_log_error("save.c - save_progress()", "Erreur de création de la chaine JSON");
        return;
    }

    FILE *file = fopen("save.json", "w");
    if (file == NULL) {
        add_log_error("save.c - save_progress()", "Erreur d'ouverture du fichier save.json");
        return;
    }

    fprintf(file, "%s", json_string);
    fclose(file);
    cJSON_Delete(root);
    free(json_string);
    return;
}

char* getlastworldname(){
    FILE *file = fopen("save.json", "r");
    if (file == NULL) {
        add_log_error("save.c - getlastworldname()", "Erreur d'ouverture du fichier save.json");
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long fsize = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *json_string = (char *)malloc(fsize + 1);
    if (json_string == NULL) {
        add_log_error("save.c - getlastworldname()", "Erreur d'allocation de la chaine JSON");
        return NULL;
    }

    fread(json_string, 1, fsize, file);
    fclose(file);

    cJSON *root = cJSON_Parse(json_string);
    if (root == NULL) {
        add_log_error("save.c - getlastworldname()", "Erreur de parsing du fichier save.json");
        return NULL;
    }

    cJSON *world = cJSON_GetObjectItem(root, "world");
    if (world == NULL) {
        add_log_error("save.c - getlastworldname()", "Erreur de récupération du monde");
        return NULL;
    }

    char *world_name = (char *)malloc(strlen(world->valuestring) + 1);
    if (world_name == NULL) {
        add_log_error("save.c - getlastworldname()", "Erreur d'allocation du nom du monde");
        return NULL;
    }

    strcpy(world_name, world->valuestring);
    cJSON_Delete(root);
    free(json_string);
    return world_name;
}

//Retourne 1 si une sauvegarde existe, 0 sinon
int exist_save() {
    FILE *file = fopen("save.json", "r");
    if (file == NULL) {
        return 0;
    }
    fclose(file);
    return 1;
}