#include <stdlib.h>
#include <stdio.h>
#include <SDL.h>
#include <SDL_ttf.h>
#include "../cta_utils/cta.h"
#include "../logs_utils/log.h"

//Fonction qui affiche à l'écran le touches de déplacement
void display_controls(SDL_Renderer *renderer, int x, int y){
    TTF_Font *font = TTF_OpenFont("res/font/Jersey10-Regular.ttf", 72);
    if (!font) {
        add_log_error("menu.c - menu()", "Erreur lors du chargement de la police.");
        return;
    }

    draw_button(renderer, x, y, 2, "Z", font, 1);
    draw_button(renderer, x-100, y+100, 2, "Q", font, 1);
    draw_button(renderer, x, y+100, 2, "S", font, 1);
    draw_button(renderer, x+100, y+100, 2, "D", font, 1);

}