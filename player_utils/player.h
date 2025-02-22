#ifndef player_h
#define player_h

#include <SDL.h>
#include "../worlds/worlds_utils.h"

enum Direction {HAUT, BAS, GAUCHE, DROITE};

//Contient les informations du joueur
typedef struct player{
    SDL_Rect player_rect; //Rectangle qui reçoit la texture du joueur
    SDL_Texture *player_texture; //Texture actuelle du joueur
    SDL_Texture *img_dir_joueur[4]; //tableau contenant les textures du joueurs pour chaque direction
    int MposX; //Position du joueur en x dans la matrice
    int MposY; //Position du joueur en y dans la matrice
    int SposX; //Position du joueur en x sur la fenêtre
    int SposY; //Position du joueur en y sur la fenêtre
}player;

player init_player(SDL_Renderer *renderer, world world);

SDL_Texture* loadTexture(const char* path, SDL_Renderer* renderer);

void free_player(player player);
#endif
