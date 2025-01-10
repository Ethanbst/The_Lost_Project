// SDL VSCODE C/C++
//
// This project is part of a tutorial by The Science of Code:
// - English: https://thescienceofcode.com/sdl-vscode-c-cpp-debug/
// - Español: https://thescienceofcode.com/es/sdl-vscode-c-cpp-debug/
//
// NOTES:
// - This project uses unity build, includes inside secondary files are used only for IDE purpouses (code completition, syntax hl, etc).
// - "IWYU pragma: keep" comments are used to ignore clangd warnings related with unity builds.

#define UNITY_BUILD 1
#include <stdio.h>              // IWYU pragma: keep
#ifdef _WIN64
 #include <SDL.h>
 #include <SDL_image.h>
 #include <SDL_mixer.h>
 #include <SDL_ttf.h>
#else
 #include <SDL2/SDL.h>
 #include <SDL2/SDL_image.h>
 #include <SDL2/SDL_mixer.h>
 #include <SDL2/SDL_ttf.h>
#endif
#include "common.h"
#include "sdl_utils.h"
#include "logs_utils/log.c"
#include "menu.c"


//Change la position de notre image joueur en fonction de la touche pressée
void mouvement(SDL_Event event, SDL_Rect *pos_joueur, SDL_Surface **orientation_joueur, SDL_Surface *img_joueur[4]){
    add_log("MOUVEMENT","mouvement()\n");
    if(event.key.keysym.sym == SDLK_z){
        add_log("MOUVEMENT","Z pressee\n");
        pos_joueur->y -= 100;
        *orientation_joueur = img_joueur[0];
    }

    if(event.key.keysym.sym == SDLK_q){
        add_log("MOUVEMENT","Q pressee\n");
        pos_joueur->x -= 100;
        *orientation_joueur = img_joueur[2];
    }
    if(event.key.keysym.sym == SDLK_s){
        add_log("MOUVEMENT","S pressee\n");
        pos_joueur->y += 100;
        *orientation_joueur = img_joueur[1];
    }
    if(event.key.keysym.sym == SDLK_d){
        add_log("MOUVEMENT","D pressee\n");
        pos_joueur->x += 100;
        *orientation_joueur = img_joueur[3];
    }
    if(orientation_joueur){
        add_log("MOUVEMENT","Orientation changee.");
    }
    else{
        add_log("MOUVEMENT","Erreur d'orientation.");
    }
    return;
}


//Fonction de lancement du jeu
void jeu(SDL_Window *window, SDL_Renderer *renderer){

    add_log("JEU","Fonction jeu.\n");
    
    SDL_Surface *ecran = SDL_GetWindowSurface(window);

    //Implémentation des éléments du joueur:
    enum{VIDE, MUR, JOUEUR};
    enum{HAUT, BAS, GAUCHE, DROITE};
    SDL_Rect position, posJoueur; //Position de l'image & Position joueur

    SDL_Surface *img_joueur[4]={NULL}; //Liste de valeur possible pour la rotation du joueur
    SDL_Surface *orientation_joueur = NULL; //Image du personnage en fonction du dernier déplacement (H, B, G, D)

    //Chargement des images du joueur:
    img_joueur[BAS]=IMG_Load("res/joueur/joueurB.bmp");
    img_joueur[HAUT]=IMG_Load("res/joueur/joueurH.bmp");
    img_joueur[GAUCHE]=IMG_Load("res/joueur/joueurG.bmp");
    img_joueur[DROITE]=IMG_Load("res/joueur/joueurD.bmp");

    if(!img_joueur[BAS] || !img_joueur[HAUT] || !img_joueur[GAUCHE] || !img_joueur[DROITE]){
        add_log("JEU","Erreur de chargement de l'image du joueur\n");
    }

    orientation_joueur = img_joueur[BAS];
    posJoueur.x = 0;
    posJoueur.y = 0;

    position.x = posJoueur.x;
    position.y = posJoueur.y;

    //Rectangle contenan; l'image du joueur
    int h_joueur = 100;
    int l_joueur = 100;
    SDL_Rect destRect = { posJoueur.x, posJoueur.y, h_joueur, l_joueur};
        
    SDL_Event event; //Variable qui reçoit l'évènement

    int continuer = 1;
    add_log("JEU","Entré dans while jeu()\n");
    while(continuer){
        SDL_BlitScaled(orientation_joueur, NULL, ecran, &destRect);
        SDL_UpdateWindowSurface(window);
        SDL_WaitEvent(&event); //Modifie la variable event avec l'évènement reçu
        switch (event.type)
        {
            case SDL_QUIT:
                continuer = 0;
                break;
            case SDL_KEYDOWN:
            //printf("Touche pressee\n");
            mouvement(event, &posJoueur, &orientation_joueur, img_joueur);
            
            //On met à jour la position du joueur:
            SDL_FillRect(ecran, NULL, SDL_MapRGB(ecran->format, 0, 0, 0));
            destRect.x = posJoueur.x;
            destRect.y = posJoueur.y;
            SDL_BlitScaled(orientation_joueur, NULL, ecran, &destRect);
            SDL_UpdateWindowSurface(window);
            break;
            default:
                break;
        }
    }
    add_log("JEU","Sortie while jeu()\n");
    sdl_utils_Quit(window, renderer);
}


int main(int argc, char *args[])
{
    SDL_Window *window;
    SDL_Renderer *renderer;

    // Init SDL without texture filtering for better pixelart results
    if (sdl_utils_Init("The Lost Project", &window, &renderer, 0)) 
    {
        //Implémentation du logo de la fenêtre
        SDL_Surface *icon = NULL;
        icon = SDL_LoadBMP("res/icon.bmp");
        if(!icon){
            add_log("MAIN","Echec du chargement de l'icone.\n");
        }
        else{
            SDL_SetWindowIcon(window, icon);
        }

        menu(renderer, window);

    }
    else{
        add_log("MAIN","Echec initialisation de la fenetre.\n");
        return -1;
    }

    //Libération des images du joueur
    /*for(int i=0;i<4, i++){
        SDL_FreeSurface(joueur[i]);
    }*/
    sdl_utils_Quit(window, renderer);
    return 0;
}
