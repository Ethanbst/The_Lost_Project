#include <SDL.h>
#include <SDL_mixer.h>
#include <C:/cJSON-master/cJSON.h>
#include <stdio.h>
#include <string.h>
#include "../logs_utils/log.h"

//Coordonnées x:y d'apparition du joueur lorsqu'il démarre sur le monde
typedef struct coords{
    int x;
    int y;
}coords;

typedef struct battle{ //Structure contenant l'id et les coordonnées d'un combat.
    int id;
    coords battle_coords;
}battle;

typedef struct dialog{ //Structure contenant l'id et les coordonnées d'un dialogue.
    int id;
    coords dialog_coords;
}dialog;

//Contient les informations d'un monde (matrice, texture, positions du joueur dans la matrice et sur l'écran)
typedef struct world{
    int matrice[13][20]; //Matrice de la map
    coords start_spawn; //Coordonnées X.Y d'apparition du joueur sur le monde de la map
    coords end_spawn; //Coordonnées X.Y d'apparition du joueur si il reviens dans le monde
    coords back_portal; //Coordonnées X.Y du trigger pour téléporter le joueur au monde précédent
    coords next_portal; //Coordonnées X.Y du trigger pour téléporter le joueur au monde suivant
    char *music_path; //Musique du monde
    char *next_world; //Nom du monde suivant "worldX+1"
    char *previous_world; //Nom du monde précédent "worldX-1"
    char *actual_world; //Nom du monde actuel "worldX"
    SDL_Texture *global_texture; //Texture globale
    char *wall_texture_path; //Chemin de la texture des murs
    char *floor_texture_path; //Chemin de la texture du sol
    int nb_battles; //Nombre de combats dans le monde
    battle *battles; //Tableau contenant la liste des combats du monde, leur id ainsi que leurs coordonnées de trigger
    int nb_dialogs; //Nombre de dialogues dans le monde
    dialog *dialogs; //Tableau contenant la liste des dialogues du monde, leur id ainsi que leurs coordonnées de trigger
}world;

void print_world_info(world *w) {
    printf("\n###########################################################\n");
    printf("Actual World: %s\n", w->actual_world);
    printf("Next World: %s\n", w->next_world);
    printf("Previous World: %s\n", w->previous_world);
    printf("Start Spawn: (%d:%d)\n", w->start_spawn.x, w->start_spawn.y);
    printf("Next Portal: (%d:%d)\n", w->next_portal.x, w->next_portal.y);
    printf("End Spawn: (%d:%d)\n", w->end_spawn.x, w->end_spawn.y);
    printf("Back Portal: (%d:%d)\n", w->back_portal.x, w->back_portal.y);
    printf("Wall Texture Path: %s\n", w->wall_texture_path);
    printf("Music Path: %s\n", w->music_path);
    for(int i=0;i<w->nb_battles;i++){
        printf("Battle | id: %d, coords: (%d:%d)\n", w->battles[i].id, w->battles[i].battle_coords.x, w->battles[i].battle_coords.y);
    }
    for(int i=0;i<w->nb_battles;i++){
        printf("Dialog | id: %d, coords: (%d:%d)\n", w->dialogs[i].id, w->dialogs[i].dialog_coords.x, w->dialogs[i].dialog_coords.y);
    }

    printf("Matrice:\n");
    for (int i = 0; i < 13; i++) {
        for (int j = 0; j < 20; j++) {
            printf("%d ", w->matrice[i][j]);
        }
        printf("\n");
    }
    printf("\n###########################################################\n");
}

//Libère les ressources utilisées par world
void free_world(world *world) {
    if (world) {
        if (world->music_path) {
            free(world->music_path);
            world->music_path = NULL;
        }
        if (world->next_world) {
            free(world->next_world);
            world->next_world = NULL;
        }
        if (world->previous_world) {
            free(world->previous_world);
            world->previous_world = NULL;
        }
        if (world->actual_world) {
            free(world->actual_world);
            world->actual_world = NULL;
        }
        if (world->global_texture) {
            SDL_DestroyTexture(world->global_texture);
            world->global_texture = NULL;
        }
        if (world->wall_texture_path) {
            free(world->wall_texture_path);
            world->wall_texture_path = NULL;
        }
        if(world->floor_texture_path){
            free(world->floor_texture_path);
            world->floor_texture_path = NULL;
        }
        if(world->battles){
            free(world->battles);
            world->battles = NULL;
        }
        if(world){
            free(world);
            world = NULL;
        }
    }
}

//Récupère les information d'un fichier worldX.json donné en paramètre et retourne une structure contenant ces paramètres
world* get_world_info(char world_name[256]){
    add_log("worlds_utils.c - get_world_info()", "Recuperation des informations du monde.");
    char location[] = "res/worlds/";
    char world_path[256];
    strcpy(world_path, location);
    strcat(world_path, world_name);

    // Structure pour accueillir les données
    world *world = (struct world *)malloc(sizeof(struct world));

    // Lire le fichier JSON
    FILE *file = fopen(world_path, "r");
    if (!file) {
        add_log_error("worlds_utils.c - get_world_info()", "Impossible d'ouvrir le fichier worldX.json");
        world = NULL;
        return world;
    }

    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *data = (char *)malloc(length + 1);
    if (!data) {
        add_log_error("worlds_utils.c - get_world_info()", "Echec de l'allocation de memoire pour data.");
        fclose(file);
        world = NULL;
        return world;
    }
    fread(data, 1, length, file);
    fclose(file);

    data[length] = '\0';

    // Parser le JSON
    cJSON *root = cJSON_Parse(data);
    if (root) {
        // Récupération des paramètres
        cJSON *parameter = cJSON_GetObjectItem(root, "actual_world");
        world->actual_world = (char *)malloc(strlen(parameter->valuestring) + 1);
        if (world->actual_world) strcpy(world->actual_world, parameter->valuestring);
        else {
            add_log_error("worlds_utils.c - get_world_info()", "Echec de l'allocation de memoire pour actual_world.");
            free_world(world);
            world = NULL;
            return world;
        }

        parameter = cJSON_GetObjectItem(root, "next_world");
        world->next_world = (char *)malloc(strlen(parameter->valuestring) + 1);
        if (world->next_world) strcpy(world->next_world, parameter->valuestring);
        else {
            add_log_error("worlds_utils.c - get_world_info()", "Echec de l'allocation de memoire pour next_world.");
            free_world(world);
            world = NULL;
            return world;
        }

        parameter = cJSON_GetObjectItem(root, "previous_world");
        world->previous_world = (char *)malloc(strlen(parameter->valuestring) + 1);
        if (world->previous_world) strcpy(world->previous_world, parameter->valuestring);
        else {
            add_log_error("worlds_utils.c - get_world_info()", "Echec de l'allocation de memoire pour previous_world.");
            free_world(world);
            world = NULL;
            return world;
        }

        parameter = cJSON_GetObjectItem(root, "start_spawn");
        cJSON *x = cJSON_GetObjectItem(parameter, "x");
        cJSON *y = cJSON_GetObjectItem(parameter, "y");
        world->start_spawn.x = x->valueint;
        world->start_spawn.y = y->valueint;

        parameter = cJSON_GetObjectItem(root, "end_spawn");
        x = cJSON_GetObjectItem(parameter, "x");
        y = cJSON_GetObjectItem(parameter, "y");
        world->end_spawn.x = x->valueint;
        world->end_spawn.y = y->valueint;

        parameter = cJSON_GetObjectItem(root, "next_door");
        x = cJSON_GetObjectItem(parameter, "x");
        y = cJSON_GetObjectItem(parameter, "y");
        world->next_portal.x = x->valueint;
        world->next_portal.y = y->valueint;

        parameter = cJSON_GetObjectItem(root, "back_door");
        x = cJSON_GetObjectItem(parameter, "x");
        y = cJSON_GetObjectItem(parameter, "y");
        world->back_portal.x = x->valueint;
        world->back_portal.y = y->valueint;

        parameter = cJSON_GetObjectItem(root, "music");
        world->music_path = (char *)malloc(strlen(parameter->valuestring) + 1);
        if (world->music_path) strcpy(world->music_path, parameter->valuestring);

        // Accéder à l'objet "map"
        cJSON *map = cJSON_GetObjectItemCaseSensitive(root, "map");
        if (cJSON_IsArray(map)) {
            // Déclarer un tableau 2D pour stocker la matrices
            // Parcourir les lignes de la matrice
            for (int i = 0; i < 13; i++) {
                cJSON *row = cJSON_GetArrayItem(map, i);
                if (cJSON_IsArray(row)) {
                    // Parcourir les colonnes de la matrice
                    for (int j = 0; j < 20; j++) {
                        cJSON *item = cJSON_GetArrayItem(row, j);
                        if (cJSON_IsNumber(item)) {
                            world->matrice[i][j] = item->valueint;
                        } 
                        else {
                            fprintf(stderr, "Erreur : L'élément de la matrice n'est pas un nombre.\n");
                            world->matrice[i][j] = 0; // Valeur par défaut en cas d'erreur
                            add_log_error("worlds_utils.c - get_world_info()", "Erreur : L'élément de la matrice n'est pas un nombre.");
                        }
                    }
                } 
                else {
                    fprintf(stderr, "Erreur : La ligne de la matrice n'est pas un tableau.\n");
                    add_log_error("worlds_utils.c - get_world_info()", "Erreur : La ligne de la matrice n'est pas un tableau.");
                }
            }
        }

        parameter = cJSON_GetObjectItem(root, "wall_texture");
        if(!parameter){
            add_log_error("worlds_utils.c - get_world_info()", "Erreur : Aucun chemin de texture de mur n'a été trouvé.");
            free_world(world);
            world = NULL;
            return world;
        }
        else{
            char location[] = "res/textures/wall/";
            char texture_path[256];
            strcpy(texture_path, location);
            strcat(texture_path, parameter->valuestring);
            world->wall_texture_path = (char *)malloc(strlen(texture_path) + 1);
            if (world->wall_texture_path) {
                strcpy(world->wall_texture_path, texture_path);
            } 
            else {
                fprintf(stderr, "Erreur : Echec de l'allocation de memoire pour wall_texture_path.\n");
                add_log_error("worlds_utils.c - get_world_info()", "Erreur : Echec de l'allocation de memoire pour wall_texture_path.");
                free_world(world);
                world = NULL;
                return world;
            }
        }

        parameter = cJSON_GetObjectItem(root, "floor_texture");
        if(!parameter){
            add_log_error("worlds_utils.c - get_world_info()", "Erreur : Aucun chemin de texture de sol n'a été trouvé.");
            free_world(world);
            world = NULL;
            return world;
        }
        else{
            char location[] = "res/textures/floor/";
            char texture_path[256];
            strcpy(texture_path, location);
            strcat(texture_path, parameter->valuestring);
            world->floor_texture_path = (char *)malloc(strlen(texture_path) + 1);
            if (world->floor_texture_path) {
                strcpy(world->floor_texture_path, texture_path);
            } 
            else {
                fprintf(stderr, "Erreur : Echec de l'allocation de memoire pour floor_texture_path.\n");
                add_log_error("worlds_utils.c - get_world_info()", "Erreur : Echec de l'allocation de memoire pour floor_texture_path.");
                free_world(world);
                world = NULL;
                return world;
            }

            //Récupération de la liste des combats
            parameter = cJSON_GetObjectItem(root, "battle_list");
            if(!parameter){
                add_log_error("worlds_utils.c - get_world_info()", "Erreur : Aucun combat n'a été trouvé.");
                free_world(world);
                world = NULL;
                return world;
            }
            else{
                int nb_battle = cJSON_GetArraySize(parameter);
                world->nb_battles = nb_battle; //Nombre de combats dans le monde
                //printf("Nombre de combats : %d\n", world->nb_battles);
                world->battles = (battle *)malloc(nb_battle * sizeof(battle));
                if(!world->battles){
                    add_log_error("worlds_utils.c - get_world_info()", "Erreur : Echec de l'allocation de memoire pour battles.");
                    free_world(world);
                    world = NULL;
                    return world;
                }
                else{
                    for(int i = 0; i < nb_battle; i++){
                        cJSON *battle = cJSON_GetArrayItem(parameter, i);
                        cJSON *id = cJSON_GetObjectItem(battle, "id");
                        cJSON *x = cJSON_GetObjectItem(battle, "x");
                        cJSON *y = cJSON_GetObjectItem(battle, "y");
                        if(!id || !x || !y || !battle){
                            add_log_error("worlds_utils.c - get_world_info()", "Erreur : Echec de récupération des informations du combat.");
                            free_world(world);
                            world = NULL;
                            return world;
                        }
                        world->battles[i].id = id->valueint;
                        world->battles[i].battle_coords.x = x->valueint;
                        world->battles[i].battle_coords.y = y->valueint;
                        }
                }

            }

            //Récupération de la liste des combats
            parameter = cJSON_GetObjectItem(root, "dialog_list");
            if(!parameter){
                add_log_error("worlds_utils.c - get_world_info()", "Erreur : Aucun dialog n'a été trouvé.");
                free_world(world);
                world = NULL;
                return world;
            }
            else{
                int nb_dialog = cJSON_GetArraySize(parameter);
                world->nb_dialogs = nb_dialog; //Nombre de dialogues dans le monde
                world->dialogs = (dialog *)malloc(nb_dialog * sizeof(dialog));
                if(!world->dialogs){
                    add_log_error("worlds_utils.c - get_world_info()", "Erreur : Echec de l'allocation de memoire pour dialogs.");
                    free_world(world);
                    world = NULL;
                    return world;
                }
                else{
                    for(int i = 0; i < nb_dialog; i++){
                        cJSON *dialog = cJSON_GetArrayItem(parameter, i);
                        cJSON *id = cJSON_GetObjectItem(dialog, "id");
                        cJSON *x = cJSON_GetObjectItem(dialog, "x");
                        cJSON *y = cJSON_GetObjectItem(dialog, "y");
                        if(!id || !x || !y || !dialog){
                            add_log_error("worlds_utils.c - get_world_info()", "Erreur : Echec de récupération des informations du dialog.");
                            free_world(world);
                            world = NULL;
                            return world;
                        }
                        world->dialogs[i].id = id->valueint;
                        world->dialogs[i].dialog_coords.x = x->valueint;
                        world->dialogs[i].dialog_coords.y = y->valueint;
                        }
                }

            }
        }
    }
    else {
        add_log("\nGET_WORLD", "Erreur: Echec du parsing du fichier JSON\n");
        free_world(world);
        world = NULL;
        return world;
    }

    cJSON_Delete(root);
    free(data);
    return world;
}
