#include <SDL.h>
#include <SDL_mixer.h>
#include <C:/cJSON-master/cJSON.h>
#include <stdio.h>
#include <string.h>
#include "../logs_utils/log.h"

//Coordonnées x:y d'apparition du joueur lorsqu'il démarre sur le monde
typedef struct start_spawn{
    int x;
    int y;
}start_spawn;

//Coordonnées x:y d'apparition du joueur lorsqu'il revient sur le monde
typedef struct end_spawn{
    int x;
    int y;
}end_spawn;

//Contient les informations d'un monde (matrice, texture, positions du joueur dans la matrice et sur l'écran)
typedef struct world{
    int matrice[13][20]; //Matrice de la map
    start_spawn start_spawn; //Coordonnées X.Y d'apparition du joueur sur le monde de la map
    end_spawn end_spawn; //Coordonnées X.Y d'apparition du joueur si il reviens dans le monde
    Mix_Music *music; //Musique du monde
    char *next_world; //Nom du monde suivant "worldX+1"
    char *previous_world; //Nom du monde précédent "worldX-1"
    char *actual_world; //Nom du monde actuel "worldX"
    SDL_Texture *global_texture; //Texture globale
    char *wall_texture_path;
}world;

void print_world_info(world w) {
    printf("Actual World: %s\n", w.actual_world);
    printf("Next World: %s\n", w.next_world);
    printf("Previous World: %s\n", w.previous_world);
    printf("Start Spawn: (%d, %d)\n", w.start_spawn.x, w.start_spawn.y);
    printf("End Spawn: (%d, %d)\n", w.end_spawn.x, w.end_spawn.y);
    printf("Wall Texture Path: %s\n", w.wall_texture_path);

    printf("Map Matrix:\n");
    for (int i = 0; i < 13; i++) {
        for (int j = 0; j < 20; j++) {
            printf("%d ", w.matrice[i][j]);
        }
        printf("\n");
    }
}

//Récupère les information d'un fichier worldX.json donné en paramètre et retourne une structure contenant ces paramètres
world get_world_info(char world_name[256]){

    //printf("name: %s\n", world_name);
    add_log("GET_WORLD", "Entree\n");
    char location[] = "res/worlds/";
    char world_path[256];
    strcpy(world_path, location);
    strcat(world_path, world_name);
    printf("%s\n", world_path);

    // Structure pour accueillir les données
    world world;

    // Lire le fichier JSON
    FILE *file = fopen(world_path, "r");
    if (!file) {
        add_log("GET_WORLD", "Erreur: Impossible d'ouvrir le fichier worldX.json\n");
        return world;
    }

    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *data = (char *)malloc(length + 1);
    if (!data) {
        add_log("GET_WORLD", "Erreur: Echec de l'allocation de memoire\n");
        fclose(file);
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
        world.actual_world = parameter->valuestring;

        parameter = cJSON_GetObjectItem(root, "next_world");
        world.next_world = parameter->valuestring;

        parameter = cJSON_GetObjectItem(root, "previous_world");
        world.previous_world = parameter->valuestring;

        parameter = cJSON_GetObjectItem(root, "start_spawn");
        cJSON *x = cJSON_GetObjectItem(parameter, "x");
        cJSON *y = cJSON_GetObjectItem(parameter, "y");
        world.start_spawn.x = x->valueint;
        world.start_spawn.y = y->valueint;

        parameter = cJSON_GetObjectItem(root, "end_spawn");
        x = cJSON_GetObjectItem(parameter, "x");
        y = cJSON_GetObjectItem(parameter, "y");
        world.end_spawn.x = x->valueint;
        world.end_spawn.y = y->valueint;

        parameter = cJSON_GetObjectItem(root, "music");
        world.music = Mix_LoadMUS(parameter->valuestring);
        
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
                            world.matrice[i][j] = item->valueint;
                        } 
                        else {
                            fprintf(stderr, "Erreur : L'élément de la matrice n'est pas un nombre.\n");
                            world.matrice[i][j] = 0; // Valeur par défaut en cas d'erreur
                        }
                    }
                } 
                else {
                    fprintf(stderr, "Erreur : La ligne de la matrice n'est pas un tableau.\n");
                }
            }
            //printf("worlds_utils 1:1=%d", world.matrice[1][1]);
        }

        parameter = cJSON_GetObjectItem(root, "wall_texture");
        if(!parameter){
            printf("texture mur non trouvee.\n");
        }
        else{
            char location[] = "res/";
            char texture_path[256];
            strcpy(texture_path, location);
            strcat(texture_path, parameter->valuestring);
            printf("Mur: %s\n", texture_path);
            world.wall_texture_path = (char *)malloc(strlen(texture_path) + 1);
            if (world.wall_texture_path) {
                strcpy(world.wall_texture_path, texture_path);
            } else {
                fprintf(stderr, "Erreur : Echec de l'allocation de memoire pour wall_texture_path.\n");
            }
        }
    }
    else {
        add_log("GET_WORLD", "Erreur: Echec du parsing du fichier JSON\n");
        return world;
    }

    cJSON_Delete(root);
    free(data);
    return world;
}