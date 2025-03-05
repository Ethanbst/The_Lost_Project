#include <SDL.h>
#include "../worlds/worlds_utils.h"
#include "../screen_utils/screen_utils.h"
#include <SDL_image.h>
#include "../logs_utils/log.h"

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

// Fonction pour charger une texture à partir d'un fichier BMP
SDL_Texture* loadTexture(const char* path, SDL_Renderer* renderer) {
    SDL_Texture* texture = IMG_LoadTexture(renderer, path);
    if(texture == NULL) {
        add_log("TEXTURE", "Erreur de chargement de la texture\n");
    }
    return texture;
}

//Retourne une structure joueur contenant toutes les données nécessaires à celui-ci
player init_player(SDL_Renderer *renderer, world world){
        //Tableau contenant les textures de chaques direction du joueur
        player player;

        player.img_dir_joueur[BAS]=loadTexture("res/joueur/joueurB.png", renderer);
        player.img_dir_joueur[HAUT]=loadTexture("res/joueur/joueurH.png", renderer);
        player.img_dir_joueur[GAUCHE]=loadTexture("res/joueur/joueurG.png", renderer);
        player.img_dir_joueur[DROITE]=loadTexture("res/joueur/joueurD.png", renderer);
        if(!player.img_dir_joueur[BAS] || !player.img_dir_joueur[HAUT] || !player.img_dir_joueur[GAUCHE] || !player.img_dir_joueur[DROITE]){
            //add_log("JEU","Erreur de chargement de l'image du joueur\n");
        }

        player.player_texture = player.img_dir_joueur[BAS]; //Orientation du joueur dès de départ

        player.MposX = world.start_spawn.x;
        player.MposY = world.start_spawn.y;

        SDL_DisplayMode ecran =  GetScreenSize();
        SDL_Rect playerRect = {ecran.w/20*player.MposX, ecran.w/20*player.MposY, ecran.w/20, ecran.w/20};

        player.player_rect = playerRect;

        return player;
}

void free_player(player player){
    
    for(int i=0; i<4; i++){
        if(player.img_dir_joueur[i]){
            SDL_DestroyTexture(player.img_dir_joueur[i]);
        }
    }
}