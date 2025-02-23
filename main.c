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
 //#include <SDL_ttf.h>
#else
 #include <SDL2/SDL.h>
 #include <SDL2/SDL_image.h>
 #include <SDL2/SDL_mixer.h>
 //#include <SDL2/SDL_ttf.h>
#endif
#include "common.h"
#include "sdl_utils.h"
#include "menu/menu.h"
#include "logs_utils/log.h"

int main(int argc, char *args[])
{
    add_log_info("main.c", "Démarrage du programme.");
    SDL_Window *window;
    SDL_Renderer *renderer;
    create_log_file();

    // Init SDL without texture filtering for better pixelart results
    if (sdl_utils_Init("The Lost Project", &window, &renderer, 0)) 
    {
        //Implémentation du logo de la fenêtre
        SDL_Surface *icon = NULL;
        icon = SDL_LoadBMP("res/icon.bmp");
        if(!icon){
            add_log_error("main.c","Echec du chargement de l'icone.");
        }
        else{
            SDL_SetWindowIcon(window, icon);
        }

        //On passe au Menu si l'initialisation de la fenêtre est réussie
        add_log_info("mainc.c", "Appel à menu()");
        menu(renderer, window);
    }

    else{
        add_log_error("main.c","Echec d'initialisation de la fenetre.");
        return -1;
    }
    
    sdl_utils_Quit(window, renderer);
    return 0;
}
