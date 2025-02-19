#include <SDL.h>
#include "../cta_utils/cta.h"
#include "../logs_utils/log.h"


void pause(SDL_Renderer *renderer){
    add_log("PAUSE MENU","Entered");
    
    SDL_Event event;

    int screen_width, screen_height;
    //Récupère la taille de la fenêtre
    SDL_GetRendererOutputSize(renderer, &screen_width, &screen_height);

    SDL_Rect bg_rect = {
        screen_width / 4, 
        screen_height / 4, 
        screen_width / 2, 
        screen_height / 2
    };

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 128); // Semi-transparent black
    SDL_RenderFillRect(renderer, &bg_rect);

    TTF_Font *font = TTF_OpenFont("res/font/Jersey10-Regular.ttf", 72);
    if (!font) {
        add_log("MENU","Erreur lors du chargement de la police\n");
        //return;
    }


    int space = 100; //Space between every button
    CTA continue_button = draw_button(renderer, screen_width/2, screen_height/3, 2, "Continuer", font);
    CTA settings_button = draw_button(renderer, screen_width/2, screen_height/3+space, 2, "Option", font);
    CTA quit_button = draw_button(renderer, screen_width/2, screen_height/3+2*space, 2, "Quitter", font);

    SDL_RenderPresent(renderer);


    int continuer = 1;
    while(continuer){
        while(SDL_PollEvent(&event)){
            if(event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE){
                add_log("PAUSE_MENU", "ESCAPE PRESSED\n");
                continuer = 0;
                break;
            }
        }
    }
}