#ifndef worlds_utils_h
#define worlds_utils_h

#include <SDL.h>
#include <SDL_mixer.h>
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

//Récupère les information d'un fichier worldX.json donné en paramètre et retourne une structure contenant ces paramètres
world* get_world_info(char world_name[256]);

void print_world_info(world *w);

void free_world(world *world);
#endif