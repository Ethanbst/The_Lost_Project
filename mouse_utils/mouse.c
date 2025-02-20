#include <SDL.h>
#include "../logs_utils/log.h"
#include "../cta_utils/cta.h"

// Fonction pour changer le curseur
int set_hand_cursor() {
    SDL_Cursor *cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND);
    SDL_SetCursor(cursor);
    add_log("change_mouse","Curseur changé\n");

    return SDL_SYSTEM_CURSOR_HAND;
}

// Fonction pour remettre le curseur à sa forme de base
int reset_cursor() {
    SDL_Cursor *cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
    SDL_SetCursor(cursor);
    add_log("change_mouse","Curseur réinitialisé\n");

    return SDL_SYSTEM_CURSOR_ARROW;
}

//Retourne 1 si la souris survole le CTA, 0 sinon
int is_mouse_on(CTA cta){
    int mouse_x, mouse_y;
    SDL_GetMouseState(&mouse_x, &mouse_y);

    if(mouse_x >= cta.pos_x && mouse_x <= cta.pos_x+cta.w && 
       mouse_y >= cta.pos_y && mouse_y <= cta.pos_y+cta.h){
        return 1;
       }
    else{
        return 0;
    }

}