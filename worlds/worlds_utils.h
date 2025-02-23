#ifndef worlds_utils_h
#define worlds_utils_h

#include <SDL.h>
#include <SDL_mixer.h>
//Coordonnées x:y d'apparition du joueur lorsqu'il démarre sur le monde
typedef struct coords{
    int x;
    int y;
}coords;

//Contient les informations d'un monde (matrice, texture, positions du joueur dans la matrice et sur l'écran)
typedef struct world{
    int matrice[13][20]; //Matrice de la map
    coords start_spawn; //Coordonnées X.Y d'apparition du joueur sur le monde de la map
    coords end_spawn; //Coordonnées X.Y d'apparition du joueur si il reviens dans le monde
    coords back_portal; //Coordonnées X.Y du trigger pour téléporter le joueur au monde précédent
    coords next_portal; //Coordonnées X.Y du trigger pour téléporter le joueur au monde suivant
    //Mix_Music *music; //Musique du monde
    char *music_path;
    char *next_world; //Nom du monde suivant "worldX+1"
    char *previous_world; //Nom du monde précédent "worldX-1"
    char *actual_world; //Nom du monde actuel "worldX"
    SDL_Texture *global_texture; //Texture globale
    char *wall_texture_path;
}world;

//Récupère les information d'un fichier worldX.json donné en paramètre et retourne une structure contenant ces paramètres
world* get_world_info(char world_name[256]);

void print_world_info(world *w);

void free_world(world *world);
#endif