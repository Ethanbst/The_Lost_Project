#include <stdio.h>
#include <stdlib.h>
#include <C:/cJSON-master/cJSON.h>
#include "../logs_utils/log.h"
#include <string.h>
#include "../battle_utils/battle.h"

//Structure contenant les informations de la sauvegarde
typedef struct save {
    char *world_name;
} save;

//Modifie le fichier save.json en y ajoutant le nom du dernier monde joué ainsi que la liste des batailles effectuées
void save_progress(char* world_name, int battles_done[]) {
    cJSON *root = cJSON_CreateObject();
    
    //Ajout du nom du dernier monde avant la sauvegarde
    cJSON_AddStringToObject(root, "world", world_name);

    //Ajout des batailles effectuées 
    cJSON *battles = cJSON_CreateIntArray(battles_done, NB_BATTLES);
    cJSON_AddItemToObject(root, "battles_done", battles);


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

int* get_battles(){
    FILE *file = fopen("save.json", "r");
    if (file == NULL) {
        add_log_error("save.c - get_battles()", "Erreur d'ouverture du fichier save.json");
        exit(1);
    }

    fseek(file, 0, SEEK_END);
    long fsize = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *json_string = (char *)malloc(fsize + 1);
    if (json_string == NULL) {
        add_log_error("save.c - get_battles()", "Erreur d'allocation de la chaine JSON");
        exit(1);
    }

    fread(json_string, 1, fsize, file);
    fclose(file);

    cJSON *root = cJSON_Parse(json_string);
    if (root == NULL) {
        add_log_error("save.c - get_battles()", "Erreur de parsing du fichier save.json");
        exit(1);
    }

    cJSON *battles = cJSON_GetObjectItem(root, "battles_done");
    if (battles == NULL) {
        add_log_error("save.c - get_battles()", "Erreur de récupération des batailles");
        exit(1);
    }

    int* battles_done = (int *)malloc(NB_BATTLES * sizeof(int));

    for (int i = 0; i < NB_BATTLES; i++) {
        battles_done[i] = cJSON_GetArrayItem(battles, i)->valueint;
    }

    cJSON_Delete(root);
    free(json_string);
    return battles_done;
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