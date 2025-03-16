#include <SDL.h>
#include "../cta_utils/cta.h"
#include "../logs_utils/log.h"

//Affiche un menu de pause avec 3 choix, retourne 0 si le joueur veut continuer, -2 si il veut retourner au menu
int pause(SDL_Renderer *renderer){

    SDL_ShowCursor(SDL_ENABLE);

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

    color couleur = {255, 255, 255, 255}; //Couleur blanche

    int esp_bt = screen_height / 14;
    CTA settings_button = draw_button(renderer, screen_width/2, screen_height/2, 1, "Option", 1, 72, couleur);
    CTA continue_button = draw_button(renderer, screen_width/2, settings_button.pos_y-esp_bt, 1, "Continuer", 1, 72, couleur);
    CTA menu_button = draw_button(renderer, screen_width/2, settings_button.pos_y+(settings_button.h+esp_bt), 1, "Menu", 1, 72, couleur);

    SDL_RenderPresent(renderer);


    int continuer = 1;
    int actual_cursor;
    while(continuer){
        while(SDL_PollEvent(&event)){
            if((event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) || (event.type == SDL_MOUSEBUTTONDOWN && is_mouse_on(continue_button))){
                add_log("PAUSE_MENU", "ESCAPE OR CONTINUE PRESSED\n");
                reset_cursor();
                continuer = 0;
                TTF_CloseFont(font);
                break;
            }

            if(event.type == SDL_MOUSEMOTION){
                if(is_mouse_on(continue_button) || is_mouse_on(settings_button) || is_mouse_on(menu_button)){ //Si la souris survole un des boutons
                    if(actual_cursor != SDL_SYSTEM_CURSOR_HAND){
                        actual_cursor = set_hand_cursor();
                    }
                    break;
                }
                else if(actual_cursor != SDL_SYSTEM_CURSOR_ARROW){
                    actual_cursor = reset_cursor();
                    break;
                }
            }

            if(event.type == SDL_MOUSEBUTTONDOWN){
                if(is_mouse_on(settings_button)){
                    reset_cursor();
                    options(renderer);
                    continuer = 0;
                    TTF_CloseFont(font);
                    break;
                }
                if(is_mouse_on(menu_button)){
                    reset_cursor();
                    TTF_CloseFont(font);
                    printf("MENU cliqué\n");
                    return -2;
                }
            }
        }
    }
    SDL_ShowCursor(SDL_DISABLE);
    return 0;
}