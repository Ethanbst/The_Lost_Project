#include <SDL.h>
#include "../cta_utils/cta.h"
#include "../logs_utils/log.h"


void pause(SDL_Renderer *renderer){
    add_log("PAUSE MENU","Entered");
    
    SDL_Event event;

    SDL_Rect bg_rect = {1, 1, 100, 100};
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderDrawRect(renderer, &bg_rect);
    SDL_RenderPresent(renderer);

    int continuer = 1;

    int i = 0;
    while(continuer){
        //add_log("WHILE", "Hey\n");
        printf("%d", i);
        i++;

        switch (event.key.keysym.sym)
        {
        case SDLK_ESCAPE:
            add_log("PAUSE_MENU", "ESCAPE PRESSED\n");
            continuer = 0;
            break;
    
        }
    }


}